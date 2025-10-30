#pragma once
#include <covscript/runtime/instance.hpp>

namespace cs
{
	class statement_expression final : public statement_base
	{
		binary_tree<token_base *> mTree;

	   public:
		statement_expression() = delete;

		statement_expression(binary_tree<token_base *> tree, context_t c, token_base *ptr) : statement_base(std::move(c),
		                                                                                                    ptr),
		                                                                                     mTree(std::move(tree)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::expression_;
		}

		void run_impl() override;

		void repl_run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_import final : public statement_base
	{
		std::vector<std::pair<std::string, var>> m_var_list;

	   public:
		statement_import() = delete;

		statement_import(std::vector<std::pair<std::string, var>> var_list, context_t c, token_base *ptr)
		    : statement_base(std::move(c), ptr), m_var_list(std::move(var_list)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::import_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_involve final : public statement_base
	{
		bool mOverride = false;
		binary_tree<token_base *> mTree;

	   public:
		statement_involve() = delete;

		statement_involve(binary_tree<token_base *> tree, bool is_override, context_t c, token_base *ptr)
		    : statement_base(std::move(c), ptr), mOverride(is_override), mTree(std::move(tree)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::involve_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_var final : public statement_base
	{
		binary_tree<token_base *> mTree;
		bool link = false;

	   public:
		statement_var() = delete;

		statement_var(binary_tree<token_base *> tree, context_t c, bool _l, token_base *ptr) : statement_base(
		                                                                                           std::move(c), ptr),
		                                                                                       link(_l),
		                                                                                       mTree(std::move(tree)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::var_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_constant final : public statement_base
	{
		binary_tree<token_base *> mTree;

	   public:
		statement_constant() = delete;

		statement_constant(binary_tree<token_base *> tree, context_t c, token_base *ptr) : statement_base(std::move(c),
		                                                                                                  ptr),
		                                                                                   mTree(std::move(tree)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::var_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_break final : public statement_base
	{
	   public:
		statement_break() = default;

		statement_break(context_t c, token_base *ptr) : statement_base(std::move(c), ptr) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::break_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_continue final : public statement_base
	{
	   public:
		statement_continue() = default;

		statement_continue(context_t c, token_base *ptr) : statement_base(std::move(c), ptr) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::continue_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_block final : public statement_base
	{
		std::deque<statement_base *> mBlock;

	   public:
		statement_block() = delete;

		statement_block(std::deque<statement_base *> block, context_t c, token_base *ptr) : statement_base(std::move(c),
		                                                                                                   ptr),
		                                                                                    mBlock(std::move(block)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::block_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;

		const std::deque<statement_base *> &get_block() const
		{
			return this->mBlock;
		}
	};

	class statement_namespace final : public statement_base
	{
		std::string mName;
		std::deque<statement_base *> mBlock;

	   public:
		statement_namespace() = delete;

		statement_namespace(token_base *tbp, std::deque<statement_base *> block, context_t c, token_base *ptr)
		    : statement_base(std::move(c), ptr), mName(static_cast<token_id *>(tbp)->get_id()), mBlock(std::move(block)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::namespace_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_if final : public statement_base
	{
		binary_tree<token_base *> mTree;
		std::deque<statement_base *> mBlock;

	   public:
		statement_if() = delete;

		statement_if(binary_tree<token_base *> tree, std::deque<statement_base *> block, context_t c,
		             token_base *ptr) : statement_base(std::move(c), ptr), mTree(std::move(tree)), mBlock(std::move(block)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::if_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_ifelse final : public statement_base
	{
		binary_tree<token_base *> mTree;
		std::deque<statement_base *> mBlock;
		std::deque<statement_base *> mElseBlock;

	   public:
		statement_ifelse() = delete;

		statement_ifelse(binary_tree<token_base *> tree, std::deque<statement_base *> btrue,
		                 std::deque<statement_base *> bfalse, context_t c, token_base *ptr) : statement_base(std::move(c),
		                                                                                                     ptr),
		                                                                                      mTree(std::move(tree)),
		                                                                                      mBlock(std::move(btrue)),
		                                                                                      mElseBlock(std::move(
		                                                                                          bfalse)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::if_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_else final : public statement_base
	{
	   public:
		statement_else() = default;

		statement_types get_type() const noexcept override
		{
			return statement_types::else_;
		}

		void run_impl() override
		{
			throw runtime_error("Do not allowed standalone else statement.");
		}

		void dump(std::ostream &) const override
		{
			throw runtime_error("Do not allowed standalone else statement.");
		}
	};

	class statement_switch final : public statement_base
	{
		binary_tree<token_base *> mTree;
		statement_block *mDefault = nullptr;
		map_t<var, statement_block *> mCases;

	   public:
		statement_switch() = delete;

		statement_switch(binary_tree<token_base *> tree, map_t<var, statement_block *> cases,
		                 statement_block *dptr, context_t c, token_base *ptr) : statement_base(std::move(c), ptr),
		                                                                        mTree(std::move(tree)),
		                                                                        mDefault(dptr),
		                                                                        mCases(std::move(cases)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::switch_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_case final : public statement_base
	{
		var mTag;
		statement_block *mBlock;

	   public:
		statement_case() = delete;

		statement_case(const var &tag, const std::deque<statement_base *> &b, const context_t &c, token_base *ptr)
		    : statement_base(c, ptr), mTag(copy(tag)), mBlock(new statement_block(b, c, ptr)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::case_;
		}

		void run_impl() override
		{
			throw runtime_error("Do not allowed standalone case statement.");
		}

		void dump(std::ostream &) const override
		{
			throw runtime_error("Do not allowed standalone case statement.");
		}

		const var &get_tag() const
		{
			return this->mTag;
		}

		statement_block *get_block() const
		{
			return this->mBlock;
		}
	};

	class statement_default final : public statement_base
	{
		statement_block *mBlock;

	   public:
		statement_default() = delete;

		statement_default(const std::deque<statement_base *> &b, const context_t &c, token_base *ptr) : statement_base(
		                                                                                                    c, ptr),
		                                                                                                mBlock(new statement_block(
		                                                                                                    b, c,
		                                                                                                    ptr)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::default_;
		}

		void run_impl() override
		{
			throw runtime_error("Do not allowed standalone default statement.");
		}

		void dump(std::ostream &) const override
		{
			throw runtime_error("Do not allowed standalone default statement.");
		}

		statement_block *get_block() const
		{
			return this->mBlock;
		}
	};

	class statement_while final : public statement_base
	{
		binary_tree<token_base *> mTree;
		std::deque<statement_base *> mBlock;

	   public:
		statement_while() = delete;

		statement_while(binary_tree<token_base *> tree, std::deque<statement_base *> b, context_t c,
		                token_base *ptr) : statement_base(std::move(c), ptr), mTree(std::move(tree)), mBlock(std::move(b)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::while_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_until final : public statement_base
	{
		token_expr *mExpr = nullptr;

	   public:
		statement_until() = delete;

		statement_until(token_expr *expr, context_t c, token_base *ptr) : statement_base(std::move(c), ptr),
		                                                                  mExpr(expr) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::until_;
		}

		token_expr *get_expr() const
		{
			return mExpr;
		}

		void run_impl() override
		{
			throw runtime_error("Do not allowed standalone until statement.");
		}

		void dump(std::ostream &) const override
		{
			throw runtime_error("Do not allowed standalone until statement.");
		}
	};

	class statement_loop final : public statement_base
	{
		std::deque<statement_base *> mBlock;

	   public:
		statement_loop() = delete;

		statement_loop(std::deque<statement_base *> b, context_t c, token_base *ptr)
		    : statement_base(std::move(c), ptr), mBlock(std::move(b)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::loop_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_loop_until final : public statement_base
	{
		binary_tree<token_base *> mExpr;
		std::deque<statement_base *> mBlock;

	   public:
		statement_loop_until() = delete;

		statement_loop_until(binary_tree<token_base *> expr, std::deque<statement_base *> b, context_t c, token_base *ptr)
		    : statement_base(std::move(c), ptr), mExpr(std::move(expr)), mBlock(std::move(b)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::loop_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_for final : public statement_base
	{
		std::deque<binary_tree<token_base *>> mParallel;
		std::deque<statement_base *> mBlock;

	   public:
		statement_for() = delete;

		statement_for(std::deque<binary_tree<token_base *>> parallel_list, std::deque<statement_base *> block,
		              context_t c, token_base *ptr) : statement_base(std::move(c), ptr),
		                                              mParallel(std::move(parallel_list)),
		                                              mBlock(std::move(block)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::for_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_foreach final : public statement_base
	{
		std::string mIt;
		binary_tree<token_base *> mObj;
		std::deque<statement_base *> mBlock;

	   public:
		statement_foreach() = delete;

		statement_foreach(std::string it, binary_tree<token_base *> tree, std::deque<statement_base *> b, context_t c,
		                  token_base *ptr) : statement_base(std::move(c), ptr), mIt(std::move(it)), mObj(std::move(tree)), mBlock(std::move(b)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::foreach_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_struct final : public statement_base
	{
		std::string mName;
		struct_builder mBuilder;
		binary_tree<token_base *> mParent;
		std::deque<statement_base *> mBlock;

	   public:
		statement_struct() = delete;

		statement_struct(const std::string &name, const binary_tree<token_base *> &tree,
		                 const std::deque<statement_base *> &method, const context_t &c, token_base *ptr)
		    : statement_base(c,
		                     ptr),
		      mName(name),
		      mBuilder(c, name,
		               tree,
		               method),
		      mParent(tree),
		      mBlock(method) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::struct_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_function final : public statement_base
	{
		std::string mName;
		function mFunc;
		bool mOverride = false;
		bool mIsMemFn = false;
		// Debug Information
#ifdef CS_DEBUGGER
		std::string mDecl;
#endif
		std::vector<std::string> mArgs;
		std::deque<statement_base *> mBlock;

	   public:
		statement_function() = delete;

#ifdef CS_DEBUGGER

		statement_function(std::string name, const std::string &decl, const std::vector<std::string> &args,
		                   const std::deque<statement_base *> &body, bool is_override, bool is_vargs,
		                   const context_t &c,
		                   token_base *ptr)
		    : statement_base(c, ptr), mName(std::move(name)), mFunc(c, decl, this, args, body, is_vargs), mOverride(is_override), mDecl(decl), mArgs(args), mBlock(body) {}

#else

		statement_function(std::string name, const std::vector<std::string> &args,
		                   const std::deque<statement_base *> &body, bool is_override, bool is_vargs,
		                   const context_t &c,
		                   token_base *ptr)
		    : statement_base(c, ptr), mName(std::move(name)), mFunc(c, args, body, is_vargs), mOverride(is_override), mArgs(args), mBlock(body) {}

#endif

		statement_types get_type() const noexcept override
		{
			return statement_types::function_;
		}

		void set_mem_fn()
		{
			mFunc.add_reserve_var("this", true);
			mIsMemFn = true;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;

#ifdef CS_DEBUGGER

		const std::string &get_decl() const
		{
			return mDecl;
		}

#endif
	};

	class statement_return final : public statement_base
	{
		binary_tree<token_base *> mTree;

	   public:
		statement_return() = delete;

		statement_return(binary_tree<token_base *> tree, context_t c, token_base *ptr) : statement_base(std::move(c),
		                                                                                                ptr),
		                                                                                 mTree(std::move(tree)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::return_;
		}

		const binary_tree<token_base *> &get_tree() const
		{
			return mTree;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_try final : public statement_base
	{
		std::string mName;
		std::deque<statement_base *> mTryBody;
		std::deque<statement_base *> mCatchBody;

	   public:
		statement_try() = delete;

		statement_try(std::string name, std::deque<statement_base *> tbody,
		              std::deque<statement_base *> cbody, context_t c, token_base *ptr) : statement_base(std::move(c), ptr),
		                                                                                  mName(std::move(name)),
		                                                                                  mTryBody(std::move(tbody)),
		                                                                                  mCatchBody(
		                                                                                      std::move(cbody)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::try_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};

	class statement_catch final : public statement_base
	{
		std::string mName;

	   public:
		statement_catch() = delete;

		statement_catch(std::string name, context_t c, token_base *ptr) : statement_base(std::move(c), ptr),
		                                                                  mName(std::move(name)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::catch_;
		}

		const std::string &get_name() const
		{
			return this->mName;
		}

		void run_impl() override
		{
			throw runtime_error("Do not allowed standalone catch statement.");
		}

		void dump(std::ostream &) const override
		{
			throw runtime_error("Do not allowed standalone catch statement.");
		}
	};

	class statement_throw final : public statement_base
	{
		binary_tree<token_base *> mTree;

	   public:
		statement_throw() = delete;

		statement_throw(binary_tree<token_base *> tree, context_t c, token_base *ptr) : statement_base(std::move(c), ptr),
		                                                                                mTree(std::move(tree)) {}

		statement_types get_type() const noexcept override
		{
			return statement_types::throw_;
		}

		void run_impl() override;

		void dump(std::ostream &) const override;
	};
} // namespace cs
