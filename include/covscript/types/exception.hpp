#pragma once

// TODO: Refactoring based on the new cs::string

#include <exception>
#include <string>

namespace cs {
	struct csym_info;

	class exception final : public std::exception {
		std::size_t mLine = 0;
		std::string mFile, mCode, mWhat, mStr;

		static std::string compose_what(const std::string &file, std::size_t line, const std::string &code, const std::string &what)
		{
			return "File \"" + file + "\", line " + std::to_string(line) + ": " + what + "\n>\t" + code + "\n";
		}

		static std::string compose_what_internal(const std::string &file, const std::string &what)
		{
			return "File \"" + file + "\", line <INTERNAL>: " + what + "\n";
		}

	public:
		exception() = delete;

		exception(std::size_t line, std::string file, std::string code, std::string what) noexcept:
			mLine(line), mFile(std::move(file)), mCode(std::move(code)), mWhat(std::move(what))
		{
			mStr = compose_what(mFile, mLine, mCode, mWhat);
		}

		exception(const exception &) = default;

		exception(exception &&) noexcept = default;

		~exception() override = default;

		exception &operator=(const exception &) = default;

		exception &operator=(exception &&) = default;

		const std::string &file() const noexcept
		{
			return mFile;
		}

		void relocate_to_csym(const csym_info &);

		const char *what() const noexcept override
		{
			return this->mStr.c_str();
		}
	};

	class compile_error final : public std::exception {
		std::string mWhat = "Compile Error";
	public:
		compile_error() = default;

		explicit compile_error(const std::string &str) noexcept:
			mWhat("Compile Error: " + str) {}

		compile_error(const compile_error &) = default;

		compile_error(compile_error &&) noexcept = default;

		~compile_error() override = default;

		compile_error &operator=(const compile_error &) = default;

		compile_error &operator=(compile_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class runtime_error final : public std::exception {
		std::string mWhat = "Runtime Error";
	public:
		runtime_error() = default;

		explicit runtime_error(const std::string &str) noexcept:
			mWhat("Runtime Error: " + str) {}

		runtime_error(const runtime_error &) = default;

		runtime_error(runtime_error &&) noexcept = default;

		~runtime_error() override = default;

		runtime_error &operator=(const runtime_error &) = default;

		runtime_error &operator=(runtime_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class internal_error final : public std::exception {
		std::string mWhat = "Internal Error";
	public:
		internal_error() = default;

		explicit internal_error(const std::string &str) noexcept:
			mWhat("Internal Error: " + str) {}

		internal_error(const internal_error &) = default;

		internal_error(internal_error &&) noexcept = default;

		~internal_error() override = default;

		internal_error &operator=(const internal_error &) = default;

		internal_error &operator=(internal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class lang_error final {
		std::string mWhat;
	public:
		lang_error() = default;

		explicit lang_error(std::string str) noexcept:
			mWhat(std::move(str)) {}

		lang_error(const lang_error &) = default;

		lang_error(lang_error &&) noexcept = default;

		~lang_error() = default;

		lang_error &operator=(const lang_error &) = default;

		lang_error &operator=(lang_error &&) = default;

		const char *what() const noexcept
		{
			return this->mWhat.c_str();
		}
	};

	class fatal_error final : public std::exception {
		std::string mWhat = "Fatal Error";
	public:
		fatal_error() = default;

		explicit fatal_error(const std::string &str) noexcept:
			mWhat("Fatal Error: " + str) {}

		fatal_error(const fatal_error &) = default;

		fatal_error(fatal_error &&) noexcept = default;

		~fatal_error() override = default;

		fatal_error &operator=(const fatal_error &) = default;

		fatal_error &operator=(fatal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
}