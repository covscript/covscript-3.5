#include <covscript/common/platform.hpp>
#include <covscript/types/basic.hpp>
#include <covscript/types/numeric.hpp>
#include <covscript/types/string.hpp>
#include <covscript/types/exception.hpp>
#include <type_traits>
#include <cstdint>
#include <bitset>

namespace cs_impl {
	// Name Demangle
	cs::byte_string_t cxx_demangle(const char *);

	template <typename T>
	inline cs::byte_string_t get_name_of_type()
	{
		return cxx_demangle(typeid(T).name());
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
			                     get_name_of_type<T>() + " cannot convert to integer.");
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
			                     get_name_of_type<T>() + " unhashable.");
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
			return "[" + get_name_of_type<T>() + "]";
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
}

template <>
cs::integer_t cs_impl::to_integer<cs::numeric_t>(const cs::numeric_t &val)
{
	return val.as_integer();
}

template <>
cs::byte_string_borrower cs_impl::to_string<cs::numeric_t>(const cs::numeric_t &val)
{
	return val.to_string();
}

template <>
cs::byte_string_borrower cs_impl::to_string<cs::byte_string_t>(const cs::byte_string_t &str)
{
	return cs::byte_string_view(str);
}

template <>
cs::byte_string_borrower cs_impl::to_string<cs::unicode_string_t>(const cs::unicode_string_t &str)
{
	return cs::unicode::unicode_to_byte(str);
}

// Small Variable Optimization
#ifndef COVSCRIPT_SVO_SIZE
#ifdef COVSCRIPT_64BIT
#define COVSCRIPT_SVO_SIZE 6
#else
#define COVSCRIPT_SVO_SIZE 14
#endif
#endif

#ifndef COVSCRIPT_BLOCK_ALLOCATOR_SIZE
#define COVSCRIPT_BLOCK_ALLOCATOR_SIZE 64
#endif

namespace cs {
	template <typename T,
	          std::size_t block_size = COVSCRIPT_BLOCK_ALLOCATOR_SIZE,
	          template <typename> class allocator_type_t = std::allocator>
	class allocator_type {
		allocator_type_t<T> m_alloc;
		T *m_pool[block_size];
		std::size_t m_offset;

	public:
		allocator_type() : m_offset(0)
		{
			for (std::size_t i = 0; i < block_size / 2; ++i)
				m_pool[i] = m_alloc.allocate(1);
			m_offset = block_size / 2;
		}

		~allocator_type()
		{
			for (std::size_t i = 0; i < m_offset; ++i)
				m_alloc.deallocate(m_pool[i], 1);
		}

		allocator_type(const allocator_type &) = delete;
		allocator_type &operator=(const allocator_type &) = delete;

		inline T *allocate(std::size_t n)
		{
			if (n == 1 && m_offset > 0)
				return m_pool[--m_offset];
			else
				return m_alloc.allocate(n);
		}

		inline void deallocate(T *ptr, std::size_t n)
		{
			if (n == 1 && m_offset < block_size)
				m_pool[m_offset++] = ptr;
			else
				m_alloc.deallocate(ptr, n);
		}
	};

#ifdef COVSCRIPT_COMPATIBILITY_MODE
	template <typename T>
	using default_allocator = std::allocator<T>;
#else
	template <typename T>
	using default_allocator = allocator_type<T>;
#endif

	struct null_t {
	};

	class alignas(64) var final {
		class base_store {
		public:
			virtual ~base_store() = default;

			virtual const std::type_info &type() const = 0;

			virtual void svo_construct(void *) = 0;

			virtual base_store *duplicate() = 0;

			virtual bool compare(const base_store *) const = 0;

			virtual integer_t to_integer() const = 0;

			virtual byte_string_borrower to_string() const = 0;

			virtual size_t hash() const = 0;

			virtual void svo_destroy() = 0;

			virtual void deallocate() = 0;

			virtual void detach() = 0;

			// virtual cs::namespace_t &get_ext() const = 0;

			virtual byte_string_t get_type_name() const = 0;
		};

		template <typename T>
		class store : public base_store {
		protected:
			T m_data;

		public:
			static default_allocator<store<T>> &get_allocator()
			{
				static default_allocator<store<T>> allocator;
				return allocator;
			}

			store() = default;

			template <typename... ArgsT>
			explicit store(ArgsT &&...args) : m_data(std::forward<ArgsT>(args)...) {}

			~store() override = default;

			const std::type_info &type() const override
			{
				return typeid(T);
			}

			virtual void svo_construct(void *mem) override
			{
				::new (mem) store(m_data);
			}

			base_store *duplicate() override
			{
				store *ptr = get_allocator().allocate(1);
				::new (ptr) store(m_data);
				return ptr;
			}

			bool compare(const base_store *obj) const override
			{
				if (obj->type() == this->type())
					return cs_impl::compare(m_data, static_cast<const store<T> *>(obj)->data());
				else
					return false;
			}

			integer_t to_integer() const override
			{
				return cs_impl::to_integer(m_data);
			}

			byte_string_borrower to_string() const override
			{
				return cs_impl::to_string(m_data);
			}

			size_t hash() const override
			{
				return cs_impl::hash<T>(m_data);
			}

			void svo_destroy() override
			{
				this->~store();
			}

			void deallocate() override
			{
				get_allocator().deallocate(this, 1);
				this->~store();
			}

			void detach() override
			{
				cs_impl::detach(m_data);
			}

			// virtual cs::namespace_t &get_ext() const override
			// {
			// 	return cs_impl::get_ext<T>();
			// }

			byte_string_t get_type_name() const override
			{
				return cs_impl::get_name_of_type<T>();
			}

			T &data()
			{
				return m_data;
			}

			const T &data() const
			{
				return m_data;
			}

			void data(const T &dat)
			{
				m_data = dat;
			}
		};

		// Aligned to 64bit cache line
		size_t m_svo_store[COVSCRIPT_SVO_SIZE];

		std::bitset<sizeof(size_t)> m_flags;
		enum class flags : unsigned {
			svo_enabled = 0
		};

		inline bool get_flag(flags idx) const noexcept
		{
			return m_flags[static_cast<unsigned>(idx)];
		}

		inline void set_flag(flags idx, bool val) noexcept
		{
			m_flags[static_cast<unsigned>(idx)] = val;
		}

		base_store *m_ptr = nullptr;

		inline void destroy_store()
		{
			if (m_ptr != nullptr) {
				if (get_flag(flags::svo_enabled))
					m_ptr->svo_destroy();
				else
					m_ptr->deallocate();
				m_ptr = nullptr;
			}
		}

		template <typename T, typename... ArgsT>
		inline void construct_store(ArgsT &&...args)
		{
			destroy_store();
			using store_type = store<T>;
			if (sizeof(store_type) <= sizeof(m_svo_store)) {
				m_ptr = reinterpret_cast<base_store *>(m_svo_store);
				set_flag(flags::svo_enabled, true);
			}
			else {
				m_ptr = store_type::get_allocator().allocate(1);
				set_flag(flags::svo_enabled, false);
			}
			::new (m_ptr) store_type(std::forward<ArgsT>(args)...);
		}

		inline void copy_store(const var &other)
		{
			destroy_store();
			if (other.m_ptr != nullptr) {
				if (other.get_flag(flags::svo_enabled)) {
					m_ptr = reinterpret_cast<base_store *>(m_svo_store);
					other.m_ptr->svo_construct(m_ptr);
					set_flag(flags::svo_enabled, true);
				}
				else {
					m_ptr = other.m_ptr->duplicate();
					set_flag(flags::svo_enabled, false);
				}
			}
		}

	public:
		template <typename T, typename... ArgsT>
		static var make(ArgsT &&...args)
		{
			var data;
			data.construct_store<T>(std::forward<ArgsT>(args)...);
			return data;
		}

		inline bool internal_svo_enabled() const noexcept
		{
			return m_ptr != nullptr && get_flag(flags::svo_enabled);
		}

		void swap(var &other) noexcept
		{
			bool lhs_svo = this->internal_svo_enabled();
			bool rhs_svo = other.internal_svo_enabled();
			if (lhs_svo && rhs_svo) {
				size_t temp_svo_store[COVSCRIPT_SVO_SIZE];
				std::swap_ranges(m_svo_store, m_svo_store + COVSCRIPT_SVO_SIZE, other.m_svo_store);
				std::swap(m_flags, other.m_flags);
			}
			else if (!lhs_svo && !rhs_svo) {
				std::swap(m_flags, other.m_flags);
				std::swap(m_ptr, other.m_ptr);
			}
			else {
				if (lhs_svo) {
					std::memcpy(other.m_svo_store, m_svo_store, sizeof(m_svo_store));
					std::swap(m_flags, other.m_flags);
					m_ptr = other.m_ptr;
					other.m_ptr = reinterpret_cast<base_store *>(other.m_svo_store);
				}
				else {
					std::memcpy(m_svo_store, other.m_svo_store, sizeof(m_svo_store));
					std::swap(m_flags, other.m_flags);
					other.m_ptr = m_ptr;
					m_ptr = reinterpret_cast<base_store *>(m_svo_store);
				}
			}
		}

		inline bool usable() const noexcept
		{
			return m_ptr != nullptr;
		}

		// alias to usable
		inline bool is_null() const noexcept
		{
			return usable();
		}

		var() noexcept = default;

		var(const var &v)
		{
			copy_store(v);
		}

		var(var &&v) noexcept
		{
			swap(v);
		}

		~var()
		{
			destroy_store();
		}

		const std::type_info &type() const
		{
			return usable() ? m_ptr->type() : typeid(null_t);
		}

		integer_t to_integer() const
		{
			return usable() ? m_ptr->to_integer() : 0;
		}

		var to_integer_var() const
		{
			return make<numeric_t>(to_integer());
		}

		byte_string_borrower to_string() const
		{
			return usable() ? m_ptr->to_string() : "null";
		}

		std::size_t hash() const
		{
			return usable() ? m_ptr->hash() : 0;
		}

		void detach() const
		{
			if (usable())
				m_ptr->detach();
		}
	};
}