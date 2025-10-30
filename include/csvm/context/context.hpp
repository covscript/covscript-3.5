#pragma once
#include <csvm/common/platform.hpp>
#include <csvm/types/types.hpp>
#include <csvm/context/memory.hpp>
#include <memory>
#include <vector>

namespace csvm
{
	enum class fiber_state
	{
		ready,
		running,
		suspended,
		finished
	};

	class fiber_type
	{
	   protected:
		fiber_type() = default;

	   public:
		fiber_type(const fiber_type &) = delete;
		fiber_type &operator=(const fiber_type &) = delete;

		virtual ~fiber_type() = default;

		virtual fiber_state get_state() const = 0;

		virtual var return_value() const = 0;
	};

	using fiber_t = std::shared_ptr<fiber_type>;

	class callable final
	{
	   public:
		using function_type = std::function<var(fwd_array &)>;
		enum class types
		{
			normal,
			request_fold,
			member_fn,
			member_visitor,
			force_regular
		};

	   private:
		function_type mFunc;
		types mType = types::normal;

	   public:
		callable() = delete;

		callable(const callable &) = default;

		explicit callable(function_type func, types type = types::normal) : mFunc(std::move(func)), mType(type) {}

		bool is_request_fold() const
		{
			return mType == types::request_fold;
		}

		bool is_member_fn() const
		{
			return mType == types::member_fn;
		}

		types type() const
		{
			return mType;
		}

		var call(fwd_array &args) const
		{
			return mFunc(args);
		}

		const function_type &get_raw_data() const
		{
			return mFunc;
		}
	};

	class compiler_type;
	class instance_type;

	using compile_t = std::shared_ptr<compiler_type>;
	using instance_t = std::shared_ptr<instance_type>;

	class context final
	{
	   public:
		// Settings
		std::vector<string> import_path;
		std::vector<string> shell_args;
		string package_name = "<Unknown>";
		string file_path = "<Unknown>";
		std::size_t stack_size = CSVM_STACK_PRESERVE;
		stack<var> call_stack;
		stack<fiber_t> fiber_stack;
		memory_manager memory;
		compile_t compiler;
		instance_t instance;

		inline std::size_t child_stack_size() const
		{
			if (stack_size >= 1000)
				return stack_size / 10;
			else
				return CSVM_STACK_PRESERVE;
		}

		void stack_reserve(std::size_t size)
		{
			stack_size = size;
			call_stack.resize(size);
			fiber_stack.resize(child_stack_size());
		}
	};

	using context_t = std::shared_ptr<context>;

	namespace fiber
	{
		fiber_t create(const context_t &, std::function<var()>);

		void resume(const fiber_t &);

		void yield();
	} // namespace fiber
} // namespace csvm