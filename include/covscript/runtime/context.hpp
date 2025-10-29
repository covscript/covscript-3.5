#pragma once
#include <covscript/common/platform.hpp>
#include <covscript/types/types.hpp>
#include <memory>
#include <vector>

namespace cs
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

	// namespace fiber {
	// 	fiber_t create(const context_t &, std::function<var()>);

	// 	void resume(const fiber_t &);

	// 	void yield();
	// }

	class context final
	{
	   public:
		// Settings
		std::vector<string> import_path;
		std::vector<string> shell_args;
		string package_name = "<Unknown>";
		string file_path = "<Unknown>";
		std::size_t stack_size = COVSCRIPT_STACK_PRESERVE;
		stack<var> call_stack;
		stack<fiber_t> fiber_stack;

		inline std::size_t child_stack_size() const
		{
			if (stack_size >= 1000)
				return stack_size / 10;
			else
				return COVSCRIPT_STACK_PRESERVE;
		}

		void stack_reserve(std::size_t size)
		{
			stack_size = size;
			call_stack.resize(size);
			fiber_stack.resize(child_stack_size());
		}
	};
} // namespace cs