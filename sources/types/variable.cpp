#include <covscript/types/variable.hpp>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

#else

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#endif

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "DbgHelp")
namespace cs_impl
{
	cs::byte_string_t cxx_demangle(const char *name)
	{
		char buffer[1024];
		DWORD length = UnDecorateSymbolName(name, buffer, sizeof(buffer), 0);
		if (length > 0)
			return cs::byte_string_t(buffer, length);
		else
			return name;
	}
} // namespace cs_impl
#elif defined(__GNUC__) || defined(__clang__)

#include <cxxabi.h>

namespace cs_impl
{
	cs::byte_string_t cxx_demangle(const char *name)
	{
		char buffer[1024] = {0};
		size_t size = sizeof(buffer);
		int status;
		char *ret = abi::__cxa_demangle(name, buffer, &size, &status);
		if (ret != nullptr)
			return cs::byte_string_t(ret);
		else
			return name;
	}
} // namespace cs_impl
#endif