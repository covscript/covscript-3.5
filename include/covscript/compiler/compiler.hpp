#pragma once
#include <covscript/compiler/symbols.hpp>

namespace cs
{
	template <typename T>
	class garbage_collector final
	{
		set_t<T *> table;

	   public:
		garbage_collector() = default;

		garbage_collector(const garbage_collector &) = delete;

		~garbage_collector()
		{
			collect();
		}

		void collect()
		{
			for (auto &ptr : table)
				::operator delete(ptr);
			table.clear();
		}

		void add(void *ptr)
		{
			table.emplace(static_cast<T *>(ptr));
		}

		void remove(void *ptr)
		{
			table.erase(static_cast<T *>(ptr));
		}
	};

	class translator_type final
	{
		using data_type = std::pair<std::deque<token_base *>, method_base *>;

		static bool compare(const token_base *a, const token_base *b)
		{
			if (a == nullptr)
				return b == nullptr;
			if (b == nullptr)
				return a == nullptr;
			if (a->get_type() != b->get_type())
				return false;
			return a->get_type() != token_types::action || static_cast<const token_action *>(a)->get_action() ==
			                                                   static_cast<const token_action *>(b)->get_action();
		}

		std::list<std::shared_ptr<data_type>> m_data;

	   public:
		translator_type() = default;

		translator_type(const translator_type &) = delete;

		~translator_type() = default;

		translator_type &add_method(const std::deque<token_base *> &grammar, method_base *method)
		{
			m_data.emplace_back(std::make_shared<data_type>(grammar, method));
			return *this;
		}

		method_base *match(const std::deque<token_base *> &raw)
		{
			if (raw.size() <= 1)
				throw compile_error("Empty input when matching grammar.");
			std::list<std::shared_ptr<data_type>> stack;
			for (auto &it : m_data)
				if (cs::translator_type::compare(it->first.front(), raw.front()))
					stack.push_back(it);
			stack.remove_if([&](const std::shared_ptr<data_type> &dat)
			                { return dat->first.size() != raw.size(); });
			stack.remove_if([&](const std::shared_ptr<data_type> &dat)
			                {
				for (std::size_t i = 1; i < raw.size() - 1; ++i) {
					if (!compare(raw.at(i), dat->first.at(i)))
						return true;
				}
				return false; });
			if (stack.empty())
				throw compile_error("Unknown grammar.");
			if (stack.size() > 1)
				throw compile_error("Ambiguous grammar.");
			return stack.front()->second;
		}

		void match_grammar(const context_t &, std::deque<token_base *> &);

		void translate(const context_t &, const std::deque<std::deque<token_base *>> &, std::deque<statement_base *> &,
		               bool);
	};

	enum class charset
	{
		ascii,
		utf8,
		gbk
	};

	struct csym_info
	{
		std::string file;
		std::vector<std::size_t> map;
		std::vector<std::string> codes;
	};

	class compiler_type final
	{
	   public:
		// Symbol Table
		mapping<std::string, signal_types> signal_map;
		mapping<std::string, action_types> action_map;
		mapping<std::string, std::function<token_base *()>> reserved_map;
		mapping<char32_t, char32_t> escape_map;
		set_t<char32_t> signals;
		mapping<signal_types, int> signal_level_map;
		set_t<signal_types> signal_left_associative;

		// Lexer
		bool issignal(char32_t ch)
		{
			return signals.count(ch) > 0;
		}

		garbage_collector<token_base *> token_gc;
		template <typename T, typename... ArgsT>
		T *gcnew_token(ArgsT &&...args)
		{
			T *ptr = new T(std::forward<ArgsT>(args)...);
			token_gc.add(ptr);
			return ptr;
		}

	   private:
		// Constants Pool
		std::vector<var> constant_pool;
		// Status
		bool inside_lambda = false;
		bool no_optimize = false;
		// Context
		context_t context;
		// Translator
		translator_type translator;

		// Preprocessor
		class preprocessor;

		// Lexer
		void process_char_buff(const std::deque<char> &, std::deque<token_base *> &, charset);

		void process_token_buff(std::deque<token_base *> &, std::deque<std::deque<token_base *>> &);

		void translate_into_tokens(const std::deque<char> &, std::deque<token_base *> &, charset);

		void process_empty_brackets(std::deque<token_base *> &);

		void process_brackets(std::deque<token_base *> &);

		// AST Builder
		int get_signal_level(token_base *ptr)
		{
			if (ptr == nullptr)
				throw compile_error("Get the level of null token.");
			if (ptr->get_type() != token_types::signal)
				throw compile_error("Get the level of non-signal token.");
			return signal_level_map.match(static_cast<token_signal *>(ptr)->get_signal());
		}

		bool is_left_associative(token_base *ptr)
		{
			if (ptr == nullptr)
				throw compile_error("Get the level of null token.");
			if (ptr->get_type() != token_types::signal)
				throw compile_error("Get the level of non-signal token.");
			return signal_left_associative.count(static_cast<token_signal *>(ptr)->get_signal()) > 0;
		}

		void kill_brackets(std::deque<token_base *> &, std::size_t = 1);

		void split_token(std::deque<token_base *> &raw, std::deque<token_base *> &, std::deque<token_base *> &);

		void build_tree(binary_tree<token_base *> &, std::deque<token_base *> &, std::deque<token_base *> &);

		void gen_tree(binary_tree<token_base *> &, std::deque<token_base *> &);

		void kill_expr(std::deque<token_base *> &);

		// Optimizer
		bool optimizable(const binary_tree<token_base *>::iterator &it)
		{
			if (!it.usable())
				return false;
			token_base *token = it.data();
			if (token == nullptr)
				return true;
			switch (token->get_type())
			{
				default:
					break;
				case token_types::value:
					return true;
					break;
			}
			return false;
		}

		enum class trim_type
		{
			normal,
			no_expr_fold,
			no_this_deduce
		};

		enum class optm_type
		{
			normal,
			enable_namespace_optm
		};

		void trim_expression(binary_tree<token_base *> &tree, trim_type do_trim = trim_type::normal)
		{
			trim_expr(tree, tree.root(), do_trim);
		}

		void optimize_expression(binary_tree<token_base *> &tree, optm_type do_optm = optm_type::normal)
		{
			trim_expr(tree, tree.root(), trim_type::normal);
			if (!disable_optimizer && !no_optimize)
				opt_expr(tree, tree.root(), do_optm);
		}

		static bool find_id_ref(binary_tree<token_base *>::iterator, const std::string &);

		void trim_expr(binary_tree<token_base *> &, binary_tree<token_base *>::iterator, trim_type);

		void opt_expr(binary_tree<token_base *> &, binary_tree<token_base *>::iterator, optm_type);

	   public:
		// map_t<string, namespace_t> modules;
		map_t<string, csym_info> csyms;

		void try_fix_this_deduction(binary_tree<token_base *>::iterator);

		compiler_type() = delete;

		explicit compiler_type(context_t c) : context(std::move(c))
		{
			// type_id::inherit_map.clear();
			// struct_builder::reset_counter();
		}

		compiler_type(const compiler_type &) = delete;

		~compiler_type() = default;

		// Settings
		bool disable_optimizer = false;
		bool fold_expr = true;

		// Context
		context_t swap_context(context_t cxt)
		{
			std::swap(context, cxt);
			return cxt;
		}

		// Metadata
		void import_csym(const std::string &, const std::string &);

		void clear_metadata()
		{
			constant_pool.clear();
		}

		void utilize_metadata()
		{
			// for (auto &it : constant_pool)
			// 	it.constant();
		}

		void add_constant(const var &val)
		{
			// if (!val.is_protect())
			// {
			// 	constant_pool.push_back(val);
			// 	constant_pool.back().protect();
			// 	constant_pool.back().mark_as_rvalue(false);
			// }
			constant_pool.push_back(val);
		}

		token_value *new_value(const var &val)
		{
			add_constant(val);
			return new token_value(val);
		}

		// Wrapped Method
		void build_expr(const std::deque<char> &buff, binary_tree<token_base *> &tree, charset encoding = charset::utf8)
		{
			std::deque<token_base *> tokens;
			process_char_buff(buff, tokens, encoding);
			process_brackets(tokens);
			kill_brackets(tokens);
			gen_tree(tree, tokens);
		}

		void process_line(std::deque<token_base *> &line)
		{
			process_brackets(line);
			kill_brackets(line, static_cast<token_endline *>(line.back())->get_line_num());
			kill_expr(line);
		}

		void
		build_line(const std::deque<char> &buff, std::deque<std::deque<token_base *>> &ast, std::size_t line_num = 1,
		           charset encoding = charset::utf8)
		{
			std::deque<token_base *> tokens;
			process_char_buff(buff, tokens, encoding);
			tokens.push_back(new token_endline(line_num));
			process_token_buff(tokens, ast);
			for (auto &line : ast)
				process_line(line);
		}

		void build_ast(const std::deque<char> &buff, std::deque<std::deque<token_base *>> &ast,
		               charset encoding = charset::utf8)
		{
			std::deque<token_base *> tokens, tmp;
			translate_into_tokens(buff, tokens, encoding);
			process_token_buff(tokens, ast);
		}

		compiler_type &add_method(const std::deque<token_base *> &grammar, method_base *method)
		{
			translator.add_method(grammar, method);
			return *this;
		}

		method_base *match_method(const std::deque<token_base *> &raw)
		{
			return translator.match(raw);
		}

		void translate(const std::deque<std::deque<token_base *>> &ast, std::deque<statement_base *> &code)
		{
			translator.translate(context, ast, code, false);
		}

		void code_gen(const std::deque<std::deque<token_base *>> &ast, std::deque<statement_base *> &code)
		{
			translator.translate(context, ast, code, true);
		}

		// AST Debugger
		static void dump_expr(binary_tree<token_base *>::iterator, std::ostream &);
	};
} // namespace cs
