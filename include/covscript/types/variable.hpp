#include <covscript/common/platform.hpp>
#include <covscript/types/basic.hpp>
#include <covscript/types/numeric.hpp>
#include <covscript/types/string.hpp>
#include <covscript/types/exception.hpp>
#include <type_traits>
#include <typeindex>
#include <cstring>
#include <cstdint>
#include <bitset>
#include <new>

namespace cs_impl {
	// Name Demangle
	cs::byte_string_t cxx_demangle(const char *);

	template <typename T>
	inline cs::byte_string_view get_name_of_type()
	{
		static cs::byte_string_t name = cxx_demangle(typeid(T).name());
		return name;
	}

	// Type support auto-detection(SFINAE)
	template <typename...>
	using void_t = void;

	// Compare
	template <typename _Tp>
	class compare_helper {
		template <typename T, typename X = bool>
		struct matcher;

		template <typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template <typename T>
		static constexpr bool match(matcher<T, decltype(std::declval<T>() == std::declval<T>())> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template <typename, bool>
	struct compare_if;

	template <typename T>
	struct compare_if<T, true> {
		static bool compare(const T &a, const T &b)
		{
			return a == b;
		}
	};

	template <typename T>
	struct compare_if<T, false> {
		static bool compare(const T &a, const T &b)
		{
			return &a == &b;
		}
	};

	// To String
	template <typename _Tp>
	class to_string_helper {
		template <typename T, typename X>
		struct matcher;

		template <typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template <typename T>
		static constexpr bool match(matcher<T, decltype(std::to_string(std::declval<T>()))> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template <typename, bool>
	struct to_string_if;

	template <typename T>
	struct to_string_if<T, true> {
		static cs::byte_string_borrower to_string(const T &val)
		{
			return std::to_string(val);
		}
	};

	// To Integer
	template <typename, bool>
	struct to_integer_if;

	template <typename T>
	struct to_integer_if<T, true> {
		static cs::integer_t to_integer(const T &val)
		{
			return static_cast<cs::integer_t>(val);
		}
	};

	template <typename T>
	struct to_integer_if<T, false> {
		static cs::integer_t to_integer(const T &)
		{
			throw cs::lang_error(cs::byte_string_t("Target type ") +
			                     get_name_of_type<T>().data() + " cannot convert to integer.");
		}
	};

	// Hash
	template <typename T, typename = void>
	struct hash_helper {
		static constexpr bool value = false;
	};

	template <typename T>
	struct hash_helper<T, void_t<decltype(std::hash<T> {}(std::declval<T>()))>> {
		static constexpr bool value = true;
	};

	template <typename T>
	struct hash_gen_base {
		static std::size_t base_code;
	};

	template <typename T>
	std::size_t hash_gen_base<T>::base_code = typeid(T).hash_code();

	template <typename, typename, bool>
	struct hash_if;

	template <typename T, typename X>
	struct hash_if<T, X, true> {
		static std::size_t hash(const X &val)
		{
			static std::hash<T> gen;
			return gen(static_cast<const T>(val)) + hash_gen_base<X>::base_code;
		}
	};

	template <typename T>
	struct hash_if<T, T, true> {
		static std::size_t hash(const T &val)
		{
			static std::hash<T> gen;
			return gen(val);
		}
	};

	template <typename T, typename X>
	struct hash_if<T, X, false> {
		static std::size_t hash(const X &val)
		{
			throw cs::lang_error(cs::byte_string_t("Target type ") +
			                     get_name_of_type<T>().data() + " unhashable.");
		}
	};

	template <typename, bool>
	struct hash_enum_resolver;

	template <typename T>
	struct hash_enum_resolver<T, true> {
		using type = hash_if<std::size_t, T, true>;
	};

	template <typename T>
	struct hash_enum_resolver<T, false> {
		using type = hash_if<T, T, hash_helper<T>::value>;
	};

	/*
	 * Type support
	 * You can specialize template functions to support type-related functions.
	 * But if you do not have specialization, CovScript can also automatically detect related functions.
	 */
	template <typename T>
	static bool compare(const T &a, const T &b)
	{
		return compare_if<T, compare_helper<T>::value>::compare(a, b);
	}

	template <typename T>
	static cs::byte_string_borrower to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

	template <typename T>
	static cs::integer_t to_integer(const T &val)
	{
		return to_integer_if<T, std::is_convertible<T, cs::integer_t>::value>::to_integer(val);
	}

	template <typename T>
	static std::size_t hash(const T &val)
	{
		using type = typename hash_enum_resolver<T, std::is_enum<T>::value>::type;
		return type::hash(val);
	}

	template <typename T>
	static void detach(T &val)
	{
		// Do something if you want when data is copying.
	}

	template <typename T>
	struct to_string_if<T, false> {
		static cs::byte_string_borrower to_string(const T &)
		{
			return "[" + cs::byte_string_t(get_name_of_type<T>()) + "]";
		}
	};

	// template<typename T>
	// static cs::namespace_t &get_ext()
	// {
	// 	throw cs::runtime_error(std::string("Target type \"") + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) +
	// 	                        "\" doesn't have extension field.");
	// }

	template <typename _Target>
	struct type_conversion_cs {
		using source_type = _Target;
		using _not_specialized = void;
	};

	template <typename _Source>
	struct type_conversion_cpp {
		using target_type = _Source;
		using _not_specialized = void;
	};

	template <typename _From, typename _To>
	struct type_convertor {
		using _not_specialized = void;
		template <typename T>
		static inline _To convert(T &&val) noexcept
		{
			return std::move(static_cast<_To>(std::forward<T>(val)));
		}
	};

	template <typename T>
	struct type_convertor<T, T> {
		template <typename X>
		static inline X &&convert(X &&val) noexcept
		{
			return std::forward<X>(val);
		}
	};

	template <typename T>
	struct type_convertor<T, void> {
		template <typename X>
		static inline void convert(X &&) noexcept {}
	};

	template <typename T>
	struct var_storage {
		using type = T;
	};

	template <typename T>
	using var_storage_t = typename var_storage<std::decay_t<T>>::type;
}

/*
 * Small Variable Optimization
 * Aligned to 64 bytes cache line
 */
#ifndef COVSCRIPT_SVO_ALIGN
#define COVSCRIPT_SVO_ALIGN (std::hardware_destructive_interference_size)
#endif

namespace cs {
	struct null_t {};

	template <std::size_t align_size,
	          template <typename> class allocator_type_t = default_allocator>
	class basic_var final {
		template <typename T>
		static allocator_type_t<T> &get_allocator()
		{
			static allocator_type_t<T> allocator;
			return allocator;
		}

		static_assert(align_size % 8 == 0, "align_size must be a multiple of 8.");
		static_assert(align_size >= alignof(std::max_align_t),
		              "align_size must greater than alignof(std::max_align_t).");

		using aligned_storage_t = std::aligned_storage_t<align_size - alignof(std::max_align_t), alignof(std::max_align_t)>;

		enum class var_op {
			access,
			compare,
			copy,
			move,
			destroy,
			rtti_type,
			type_name,
			to_integer,
			to_string,
			hash,
			detach,
		};

		union var_op_result {
			const std::type_info *_typeid;
			integer_t _int;
			std::size_t _hash;
			void *_ptr;
			var_op_result() : _ptr(nullptr) {}
		};

		template <typename T>
		struct var_op_svo_dispatcher {
			template <typename... ArgsT>
			static void construct(basic_var *val, ArgsT &&...args)
			{
				::new (&val->m_store.buffer) T(std::forward<ArgsT>(args)...);
			}
			static var_op_result dispatcher(var_op op, const basic_var *lhs, void *rhs)
			{
				const T *ptr = reinterpret_cast<const T *>(&lhs->m_store.buffer);
				var_op_result result;
				switch (op) {
				case var_op::access:
					result._ptr = const_cast<T *>(ptr);
					break;
				case var_op::compare:
					result._int = cs_impl::compare(*ptr, *static_cast<const T *>(rhs));
					break;
				case var_op::copy:
					::new (&static_cast<basic_var *>(rhs)->m_store.buffer) T(*ptr);
					break;
				case var_op::move:
					::new (&static_cast<basic_var *>(rhs)->m_store.buffer) T(std::move(*ptr));
					break;
				case var_op::destroy:
					ptr->~T();
					break;
				case var_op::rtti_type:
					result._typeid = &typeid(T);
					break;
				case var_op::type_name:
					*static_cast<byte_string_borrower *>(rhs) = cs_impl::get_name_of_type<T>();
					break;
				case var_op::to_integer:
					result._int = cs_impl::to_integer(*ptr);
					break;
				case var_op::to_string:
					*static_cast<byte_string_borrower *>(rhs) = cs_impl::to_string(*ptr);
					break;
				case var_op::hash:
					result._hash = cs_impl::hash<T>(*ptr);
					break;
				case var_op::detach:
					cs_impl::detach(*const_cast<T *>(ptr));
					break;
				}
				return result;
			}
		};

		template <typename T>
		struct var_op_heap_dispatcher {
			template <typename... ArgsT>
			static void construct(basic_var *val, ArgsT &&...args)
			{
				T *ptr = get_allocator<T>().allocate(1);
				::new (ptr) T(std::forward<ArgsT>(args)...);
				val->m_store.ptr = ptr;
			}
			static var_op_result dispatcher(var_op op, const basic_var *lhs, void *rhs)
			{
				const T *ptr = static_cast<const T *>(lhs->m_store.ptr);
				var_op_result result;
				switch (op) {
				case var_op::access:
					result._ptr = const_cast<T *>(ptr);
					break;
				case var_op::compare:
					result._int = cs_impl::compare(*ptr, *static_cast<const T *>(rhs));
					break;
				case var_op::copy: {
					T *nptr = get_allocator<T>().allocate(1);
					::new (nptr) T(*ptr);
					static_cast<basic_var *>(rhs)->m_store.ptr = nptr;
					break;
				}
				case var_op::move: {
					T *nptr = get_allocator<T>().allocate(1);
					::new (nptr) T(std::move(*ptr));
					static_cast<basic_var *>(rhs)->m_store.ptr = nptr;
					break;
				}
				case var_op::destroy:
					ptr->~T();
					get_allocator<T>().deallocate(const_cast<T *>(ptr), 1);
					break;
				case var_op::rtti_type:
					result._typeid = &typeid(T);
					break;
				case var_op::type_name:
					*static_cast<byte_string_borrower *>(rhs) = cs_impl::get_name_of_type<T>();
					break;
				case var_op::to_integer:
					result._int = cs_impl::to_integer(*ptr);
					break;
				case var_op::to_string:
					*static_cast<byte_string_borrower *>(rhs) = cs_impl::to_string(*ptr);
					break;
				case var_op::hash:
					result._hash = cs_impl::hash<T>(*ptr);
					break;
				case var_op::detach:
					cs_impl::detach(*const_cast<T *>(ptr));
					break;
				}
				return result;
			}
		};

		using dispatcher_t = var_op_result (*)(var_op, const basic_var *, void *);

		template <typename T>
		using dispatcher_class = std::conditional_t<(sizeof(T) > sizeof(aligned_storage_t)),
		      var_op_heap_dispatcher<T>, var_op_svo_dispatcher<T>>;

		dispatcher_t m_dispatcher = nullptr;
		union store_impl {
			aligned_storage_t buffer;
			void *ptr;

			store_impl() : ptr(nullptr) {}
		} m_store;

		template <typename T, typename... ArgsT>
		inline void construct_store(ArgsT &&...args)
		{
			destroy_store();
			m_dispatcher = &dispatcher_class<T>::dispatcher;
			dispatcher_class<T>::construct(this, std::forward<ArgsT>(args)...);
		}

		inline void destroy_store()
		{
			if (m_dispatcher != nullptr) {
				m_dispatcher(var_op::destroy, this, nullptr);
				m_dispatcher = nullptr;
			}
		}

		inline void copy_store(const basic_var &other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr) {
				other.m_dispatcher(var_op::copy, &other, this);
				m_dispatcher = other.m_dispatcher;
			}
		}

		inline void move_store(basic_var &&other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr) {
				other.m_dispatcher(var_op::move, &other, this);
				m_dispatcher = other.m_dispatcher;
				other.m_dispatcher = nullptr;
			}
		}

	public:
		static constexpr std::size_t internal_svo_threshold()
		{
			return sizeof(store_impl);
		}

		template <typename T, typename... ArgsT>
		static basic_var make(ArgsT &&...args)
		{
			basic_var data;
			data.construct_store<T>(std::forward<ArgsT>(args)...);
			return data;
		}

		void swap(basic_var &other) noexcept
		{
			std::swap(m_dispatcher, other.m_dispatcher);
			std::swap(m_store, other.m_store);
		}

		inline bool usable() const noexcept
		{
			return m_dispatcher != nullptr;
		}

		inline bool is_null() const noexcept
		{
			return m_dispatcher == nullptr;
		}

		basic_var() noexcept = default;

		template <typename T, typename store_t = cs_impl::var_storage_t<T>,
		          typename = std::enable_if_t<!std::is_same_v<store_t, basic_var>>>
		basic_var(T &&val)
		{
			construct_store<store_t>(std::forward<T>(val));
		}

		basic_var(const basic_var &v)
		{
			copy_store(v);
		}

		basic_var(basic_var &&v) noexcept
		{
			move_store(std::move(v));
		}

		~basic_var()
		{
			destroy_store();
		}

		const std::type_info &type() const
		{
			if (usable())
				return *m_dispatcher(var_op::rtti_type, this, nullptr)._typeid;
			else
				return typeid(null_t);
		}

		template <typename T, typename store_t = cs_impl::var_storage_t<T>>
		bool is_type_of() const
		{
			return usable() ? m_dispatcher == &dispatcher_class<store_t>::dispatcher : std::is_same_v<store_t, null_t>;
		}

		integer_t to_integer() const
		{
			if (usable())
				return m_dispatcher(var_op::to_integer, this, nullptr)._int;
			else
				return 0;
		}

		basic_var to_integer_var() const
		{
			return make<numeric_t>(to_integer());
		}

		byte_string_borrower to_string() const
		{
			if (usable())
			{
				byte_string_borrower borrower;
				m_dispatcher(var_op::to_string, this, &borrower);
				return borrower;
			}
			else
				return "null";
		}

		std::size_t hash() const
		{
			if (usable())
				return m_dispatcher(var_op::hash, this, nullptr)._hash;
			else
				return 0;
		}

		void detach()
		{
			if (usable())
				m_dispatcher(var_op::detach, this, nullptr);
		}

		byte_string_borrower type_name() const
		{
			if (usable())
			{
				byte_string_borrower borrower;
				m_dispatcher(var_op::type_name, this, &borrower);
				return borrower;
			}
			else
				return cs_impl::get_name_of_type<null_t>();
		}

		basic_var &operator=(const basic_var &obj)
		{
			if (&obj != this)
				copy_store(obj);
			return *this;
		}

		basic_var &operator=(basic_var &&obj) noexcept
		{
			if (&obj != this)
			{
				destroy_store();
				m_dispatcher = obj.m_dispatcher;
				m_store = obj.m_store;
				obj.m_dispatcher = nullptr;
			}
			return *this;
		}

		bool compare(const basic_var &obj) const
		{
			if (usable() && m_dispatcher == obj.m_dispatcher)
				return m_dispatcher(var_op::compare, this,
				                    m_dispatcher(var_op::access, &obj, nullptr)._ptr)._int;
			else
				return obj.is_null();
		}

		bool operator==(const basic_var &obj) const
		{
			return compare(obj);
		}

		bool operator!=(const basic_var &obj) const
		{
			return !compare(obj);
		}

		template <typename T, typename store_t = cs_impl::var_storage_t<T>>
		inline store_t &val()
		{
			if (m_dispatcher == &dispatcher_class<store_t>::dispatcher)
				return *static_cast<store_t *>(m_dispatcher(var_op::access, this, nullptr)._ptr);
			else if (m_dispatcher == nullptr)
				throw runtime_error("Instance null variable.");
			else
				throw runtime_error("Instance variable with wrong type. Provided " +
				                    cs_impl::cxx_demangle(typeid(T).name()) + ", expected " + type_name().data());
		}

		template <typename T, typename store_t = cs_impl::var_storage_t<T>>
		inline const store_t &const_val() const
		{
			if (m_dispatcher == &dispatcher_class<store_t>::dispatcher)
				return *static_cast<const store_t *>(m_dispatcher(var_op::access, this, nullptr)._ptr);
			else if (m_dispatcher == nullptr)
				throw runtime_error("Instance null variable.");
			else
				throw runtime_error("Instance variable with wrong type. Provided " +
				                    cs_impl::cxx_demangle(typeid(T).name()) + ", expected " + type_name().data());
		}

		template <typename T>
		explicit operator T &()
		{
			return this->val<T>();
		}

		template <typename T>
		explicit operator const T &() const
		{
			return this->const_val<T>();
		}
	};

	using var = basic_var<COVSCRIPT_SVO_ALIGN>;
}

namespace cs_impl {
	template <>
	cs::integer_t to_integer<cs::numeric_t>(const cs::numeric_t &val)
	{
		return val.as_integer();
	}

	template <>
	cs::byte_string_borrower to_string<cs::numeric_t>(const cs::numeric_t &val)
	{
		return val.to_string();
	}

	template <>
	cs::byte_string_borrower to_string<cs::byte_string_t>(const cs::byte_string_t &str)
	{
		return cs::byte_string_view(str);
	}

	template <>
	cs::byte_string_borrower to_string<cs::unicode_string_t>(const cs::unicode_string_t &str)
	{
		return cs::unicode::unicode_to_byte(str);
	}

	template <>
	cs::byte_string_borrower to_string<bool>(const bool &v)
	{
		if (v)
			return "true";
		else
			return "false";
	}

	template <int N>
	struct var_storage<char[N]>
	{
		using type = std::string;
	};

	template <>
	struct var_storage<std::type_info>
	{
		using type = std::type_index;
	};
}

// std::ostream &operator<<(std::ostream &, const cs::var &);

namespace std {
	template <>
	struct hash<cs::var>
	{
		std::size_t operator()(const cs::var &val) const
		{
			return val.hash();
		}
	};
}
