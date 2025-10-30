#pragma once
#include <csvm/types/string.hpp>
#include <exception>

namespace csvm
{
	struct csym_info;

	class exception final : public std::exception
	{
		std::size_t mLine = 0;
		byte_string_borrower mFile, mCode, mWhat, mStr;

		static byte_string_t compose_what(const byte_string_borrower &file, std::size_t line, const byte_string_borrower &code, const byte_string_borrower &what)
		{
			return byte_string_t("File \"") + file.data() + "\", line " + std::to_string(line) + ": " + what.data() + "\n>\t" + code.data() + "\n";
		}

		static byte_string_t compose_what_internal(const byte_string_borrower &file, const byte_string_borrower &what)
		{
			return byte_string_t("File \"") + file.data() + "\", line <INTERNAL>: " + what.data() + "\n";
		}

	   public:
		exception() = delete;

		exception(std::size_t line, byte_string_borrower file, byte_string_borrower code, byte_string_borrower what) noexcept : mLine(line), mFile(std::move(file)), mCode(std::move(code)), mWhat(std::move(what))
		{
			mStr = compose_what(mFile, mLine, mCode, mWhat);
		}

		exception(const exception &) = default;

		exception(exception &&) noexcept = default;

		~exception() override = default;

		exception &operator=(const exception &) = default;

		exception &operator=(exception &&) = default;

		const byte_string_borrower &file() const noexcept
		{
			return mFile;
		}

		void relocate_to_csym(const csym_info &);

		const char *what() const noexcept override
		{
			return this->mStr.data();
		}
	};

	class compile_error final : public std::exception
	{
		byte_string_borrower mWhat = "Compile Error";

	   public:
		compile_error() = default;

		explicit compile_error(const byte_string_borrower &str) noexcept : mWhat(byte_string_t("Compile Error: ") + str.data()) {}

		compile_error(const compile_error &) = default;

		compile_error(compile_error &&) noexcept = default;

		~compile_error() override = default;

		compile_error &operator=(const compile_error &) = default;

		compile_error &operator=(compile_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.data();
		}
	};

	class runtime_error final : public std::exception
	{
		byte_string_borrower mWhat = "Runtime Error";

	   public:
		runtime_error() = default;

		explicit runtime_error(const byte_string_borrower &str) noexcept : mWhat(byte_string_t("Runtime Error: ") + str.data()) {}

		runtime_error(const runtime_error &) = default;

		runtime_error(runtime_error &&) noexcept = default;

		~runtime_error() override = default;

		runtime_error &operator=(const runtime_error &) = default;

		runtime_error &operator=(runtime_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.data();
		}
	};

	class internal_error final : public std::exception
	{
		byte_string_borrower mWhat = "Internal Error";

	   public:
		internal_error() = default;

		explicit internal_error(const byte_string_borrower &str) noexcept : mWhat(byte_string_t("Internal Error: ") + str.data()) {}

		internal_error(const internal_error &) = default;

		internal_error(internal_error &&) noexcept = default;

		~internal_error() override = default;

		internal_error &operator=(const internal_error &) = default;

		internal_error &operator=(internal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.data();
		}
	};

	class lang_error final
	{
		byte_string_borrower mWhat;

	   public:
		lang_error() = default;

		explicit lang_error(byte_string_borrower str) noexcept : mWhat(std::move(str)) {}

		lang_error(const lang_error &) = default;

		lang_error(lang_error &&) noexcept = default;

		~lang_error() = default;

		lang_error &operator=(const lang_error &) = default;

		lang_error &operator=(lang_error &&) = default;

		const char *what() const noexcept
		{
			return this->mWhat.data();
		}
	};

	class fatal_error final : public std::exception
	{
		byte_string_borrower mWhat = "Fatal Error";

	   public:
		fatal_error() = default;

		explicit fatal_error(const byte_string_borrower &str) noexcept : mWhat(byte_string_t("Fatal Error: ") + str.data()) {}

		fatal_error(const fatal_error &) = default;

		fatal_error(fatal_error &&) noexcept = default;

		~fatal_error() override = default;

		fatal_error &operator=(const fatal_error &) = default;

		fatal_error &operator=(fatal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.data();
		}
	};
} // namespace csvm