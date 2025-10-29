#pragma once
#include <covscript/types/basic.hpp>
#include <covscript/types/string.hpp>
#include <covscript/types/numeric.hpp>
#include <covscript/types/exception.hpp>
#include <covscript/types/variable.hpp>

#ifndef CS_COMPATIBILITY_MODE
#include <parallel_hashmap/phmap.h>
#else
#include <unordered_map>
#include <unordered_set>
#endif

#include <list>
#include <forward_list>
#include <deque>
#include <vector>
#include <utility>

namespace cs
{
	using string = byte_string_t;
	using utf_string = unicode_string_t;
	using list = std::list<var>;
	using fwd_list = std::forward_list<var>;
	using array = std::deque<var>;
	using fwd_array = std::vector<var>;
	using pair = std::pair<var, var>;

#ifndef CS_COMPATIBILITY_MODE
	template <typename _kT, typename _vT>
	using map_t = phmap::flat_hash_map<_kT, _vT>;
	template <typename _Tp>
	using set_t = phmap::flat_hash_set<_Tp>;
#else
	template <typename _kT, typename _vT>
	using map_t = std::unordered_map<_kT, _vT>;
	template <typename _Tp>
	using set_t = std::unordered_set<_Tp>;
#endif

	using hash_set = set_t<var>;
	using hash_map = map_t<var, var>;
} // namespace cs

#include "covscript/types/xtra_impl.cpp"
