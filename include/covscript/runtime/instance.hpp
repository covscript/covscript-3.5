#pragma once
#include <covscript/runtime/op_parser.hpp>

namespace cs
{
	context_t create_context(const array &);

	context_t create_subcontext(const context_t &);

	class instance_type final
	{
		friend class repl;

		friend context_t cs::create_context(const array &);

		friend context_t cs::create_subcontext(const context_t &);

		// Statements
		std::deque<statement_base *> statements;

		memory_manager *fiber_manager = nullptr, *global_manager = nullptr;

	   public:
		// Status
		bool return_fcall = false;
		bool break_block = false;
		bool continue_block = false;
		// Context
		context_t context;
		op_parser_t op_parser;

		// Constructor and destructor
		instance_type() = delete;

		explicit instance_type(context_t c) : context(std::move(c)), op_parser(c), global_manager(&c->memory) {}

		instance_type(const instance_type &) = delete;

		~instance_type() = default;

		// Wrapped Method
		// namespace_t import(const std::string &, const std::string &);

		// namespace_t source_import(const std::string &);

		void compile(const std::string &);

		void compile(std::istream &);

		void interpret();

		void dump_ast(std::ostream &);

		// Parse variable definition
		void check_declar_var(binary_tree<token_base *>::iterator, bool = false);

		void check_define_var(binary_tree<token_base *>::iterator, bool = false, bool = false);

		void parse_define_var(binary_tree<token_base *>::iterator, bool = false, bool = false);

		void check_define_structured_binding(binary_tree<token_base *>::iterator, bool = false);

		void parse_define_structured_binding(binary_tree<token_base *>::iterator, bool = false, bool = false);

		// Parse using statement
		void parse_using(binary_tree<token_base *>::iterator, bool = false);

		// Coroutines
		memory_manager *swap_context(memory_manager *manager)
		{
			std::swap(fiber_manager, manager);
			return manager;
		}

		void enter_domain(string_view name = "<Temporary>", bool declare = false)
		{
			if (fiber_manager != nullptr)
				fiber_manager->enter_domain(name, declare);
			else
				global_manager->enter_domain(name, declare);
		}

		void leave_domain(bool force_clear = false)
		{
			if (fiber_manager != nullptr)
				fiber_manager->leave_domain(force_clear);
			else
				global_manager->leave_domain(force_clear);
		}

		void declare_var(const string &name, const var &value, bool override = false)
		{
			if (fiber_manager != nullptr)
				fiber_manager->declare_var(name, value, override);
			else
				global_manager->declare_var(name, value, override);
		}

		var &access(memory_manager::stack_visitor &v)
		{
			if (fiber_manager != nullptr)
			{
				var *data = fiber_manager->access_opt(v);
				if (data != nullptr)
					return *data;
				data = global_manager->access_opt(v);
				if (data != nullptr)
					return *data;
				throw runtime_error("Use of undefined variable \"" + v.name + "\".");
			}
			else
				return global_manager->access(v);
		}

		var *access_opt(memory_manager::stack_visitor &v) noexcept
		{
			if (fiber_manager != nullptr)
			{
				var *data = fiber_manager->access_opt(v);
				if (data != nullptr)
					return data;
				data = global_manager->access_opt(v);
				if (data != nullptr)
					return data;
				return nullptr;
			}
			else
				return global_manager->access_opt(v);
		}

		memory_manager::heap_pointer *gcnew()
		{
			if (fiber_manager != nullptr)
				return fiber_manager->gcnew();
			else
				return global_manager->gcnew();
		}

		template <typename T, typename... ArgsT>
		memory_manager::heap_pointer *gcnew(ArgsT &&...args)
		{
			if (fiber_manager != nullptr)
				return fiber_manager->gcnew(std::forward<ArgsT>(args)...);
			else
				return global_manager->gcnew(std::forward<ArgsT>(args)...);
		}

		// void clear_context()
		// {
		// 	if (fiber_stack != nullptr)
		// 	{
		// 		while (!fiber_stack->empty())
		// 			fiber_stack->pop_no_return();
		// 		fiber_stack = nullptr;
		// 	}
		// }
	};

	// Repl
	class repl final
	{
		std::deque<std::deque<token_base *>> tmp;
		stack<method_base *> methods;
		charset encoding = charset::utf8;
		std::size_t line_num = 0;
		bool multi_line = false;
		string line_buff;
		string cmd_buff;

		void interpret(const string &, std::deque<token_base *> &);

		void run(const string &);

	   public:
		context_t context;

		bool echo = true;

		repl() = delete;

		explicit repl(context_t);

		repl(const repl &) = delete;

		void exec(const string &);

		void reset_status()
		{
			tmp.clear();
			while (!methods.empty())
				methods.pop_no_return();
			multi_line = false;
			line_buff.clear();
			cmd_buff.clear();
			context->compiler->utilize_metadata();
			// context->instance->storage.clear_set();
		}

		std::size_t get_level() const
		{
			return methods.size();
		}
	};

	// Guarder
	class scope_guard final
	{
		const context_t &context;

	   public:
		scope_guard() = delete;

		explicit scope_guard(const context_t &c) : context(c)
		{
			context->instance->enter_domain();
		}

		~scope_guard()
		{
			context->instance->leave_domain();
		}

		// const domain_type &get() const
		// {
		// 	return context->instance->storage.get_domain();
		// }

		// void clear() const
		// {
		// 	context->instance->storage.next_domain();
		// }

		// void reset() const
		// {
		// 	context->instance->storage.clear_domain();
		// }
	};

	class fcall_guard final
	{
		const context_t &context;

	   public:
		fcall_guard() = delete;
		fcall_guard(const context_t &c) : context(c)
		{
			context->call_stack.push();
		}

		~fcall_guard()
		{
			context->call_stack.pop_no_return();
		}

		var &get() const
		{
			return context->call_stack.top();
		}
	};

	std::string get_sdk_path();
} // namespace cs
