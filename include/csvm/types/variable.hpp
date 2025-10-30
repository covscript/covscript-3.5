#pragma once
#include <csvm/common/platform.hpp>
#include <csvm/types/basic.hpp>
#include <csvm/types/numeric.hpp>
#include <csvm/types/string.hpp>
#include <csvm/types/exception.hpp>
#include <csvm/types/support.hpp>
#include <type_traits>
#include <typeindex>
#include <cstring>
#include <cstdint>
#include <bitset>

#ifndef CSVM_SVO_ALIGN_SIZE
#define CSVM_SVO_ALIGN_SIZE CSVM_CACHELINE_SIZE
#endif

namespace csvm
{
	struct null_t
	{
	};

	template <std::size_t align_size, template <typename> class allocator_t>
	class basic_var_borrower;

	template <std::size_t align_size,
	          template <typename> class allocator_t = default_allocator>
	class basic_var final
	{
		template <typename T>
		static allocator_t<T> &get_allocator()
		{
			static allocator_t<T> allocator;
			return allocator;
		}

		static_assert(align_size % 8 == 0, "align_size must be a multiple of 8.");
		static_assert(align_size >= std::max(alignof(std::max_align_t), 2 * sizeof(void *)),
		              "align_size must greater than alignof(std::max_align_t).");

		using aligned_storage_t = std::aligned_storage_t<align_size - (std::max) (alignof(std::max_align_t), 2 * sizeof(void *)), alignof(std::max_align_t)>;

		enum class var_op
		{
			get,
			copy,
			move,
			destroy,
			rtti_type,
			type_name,
			to_integer,
			to_string,
			hash,
			mark_reachable
		};

		union var_op_result
		{
			const std::type_info *_typeid;
			integer_t _int;
			std::size_t _hash;
			void *_ptr;
			var_op_result() : _ptr(nullptr) {}
		};

		template <typename T>
		struct var_op_svo_dispatcher
		{
			template <typename... ArgsT>
			static void construct(basic_var *val, ArgsT &&...args)
			{
				::new (&val->m_store.buffer) T(std::forward<ArgsT>(args)...);
			}
			static var_op_result dispatcher(var_op, const basic_var *, void *);
		};

		template <typename T>
		struct var_op_heap_dispatcher
		{
			template <typename... ArgsT>
			static void construct(basic_var *val, ArgsT &&...args)
			{
				T *ptr = get_allocator<T>().allocate(1);
				::new (ptr) T(std::forward<ArgsT>(args)...);
				val->m_store.ptr = ptr;
			}
			static var_op_result dispatcher(var_op, const basic_var *, void *);
		};

		using dispatcher_t = var_op_result (*)(var_op, const basic_var *, void *);

		template <typename T>
		using dispatcher_class = std::conditional_t<(sizeof(T) > sizeof(aligned_storage_t)),
		                                            var_op_heap_dispatcher<T>, var_op_svo_dispatcher<T>>;

		using operators_type = csvm_impl::operators::type;

		template <typename T>
		static basic_var_borrower<align_size, allocator_t> call_operator(operators_type, bool, const basic_var *, void *);

		using operator_t = basic_var_borrower<align_size, allocator_t> (*)(operators_type, bool, const basic_var *, void *);

		dispatcher_t m_dispatcher = nullptr;
		operator_t m_operator = nullptr;
		union store_impl
		{
			aligned_storage_t buffer;
			void *ptr;

			store_impl() : ptr(nullptr) {}
		} m_store;

		template <typename T>
		inline T &unchecked_get() noexcept
		{
			return *static_cast<T *>(m_dispatcher(var_op::get, this, nullptr)._ptr);
		}

		template <typename T>
		inline const T &unchecked_get() const noexcept
		{
			return *static_cast<const T *>(m_dispatcher(var_op::get, this, nullptr)._ptr);
		}

		template <typename T, typename... ArgsT>
		inline void construct_store(ArgsT &&...args)
		{
			destroy_store();
			m_dispatcher = &dispatcher_class<T>::dispatcher;
			m_operator = &call_operator<T>;
			dispatcher_class<T>::construct(this, std::forward<ArgsT>(args)...);
		}

		inline void destroy_store()
		{
			if (m_dispatcher != nullptr)
			{
				m_dispatcher(var_op::destroy, this, nullptr);
				m_dispatcher = nullptr;
			}
		}

		inline void copy_store(const basic_var &other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr)
			{
				other.m_dispatcher(var_op::copy, &other, this);
				m_dispatcher = other.m_dispatcher;
				m_operator = other.m_operator;
			}
		}

		inline void move_store(basic_var &&other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr)
			{
				other.m_dispatcher(var_op::move, &other, this);
				m_dispatcher = other.m_dispatcher;
				m_operator = other.m_operator;
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
			std::swap(m_operator, other.m_operator);
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

		template <typename T, typename store_t = csvm_impl::var_storage_t<T>,
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

		template <typename T, typename store_t = csvm_impl::var_storage_t<T>>
		bool is_type_of() const
		{
			return usable() ? (m_dispatcher == &dispatcher_class<store_t>::dispatcher || type() == typeid(T)) : std::is_same_v<store_t, null_t>;
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

		void gc_mark_reachable() const
		{
			if (usable())
				m_dispatcher(var_op::mark_reachable, this, nullptr);
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
				return csvm_impl::get_name_of_type<null_t>();
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
				m_operator = obj.m_operator;
				m_store = obj.m_store;
				obj.m_dispatcher = nullptr;
			}
			return *this;
		}

		bool compare(const basic_var &obj) const
		{
			if (usable() && m_dispatcher == obj.m_dispatcher)
				return m_operator(operators_type::compare,
				                  true, this, (void *) &obj)
				    .data()
				    ->template unchecked_get<bool_t>();
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

		template <typename T, typename store_t = csvm_impl::var_storage_t<T>>
		inline store_t &val()
		{
			if (m_dispatcher == &dispatcher_class<store_t>::dispatcher || type() == typeid(T))
				return *static_cast<store_t *>(m_dispatcher(var_op::get, this, nullptr)._ptr);
			else if (m_dispatcher == nullptr)
				throw runtime_error("Instance null variable.");
			else
				throw runtime_error("Instance variable with wrong type. Provided " +
				                    csvm_impl::cxx_demangle(typeid(T).name()) + ", expected " + type_name().data());
		}

		template <typename T, typename store_t = csvm_impl::var_storage_t<T>>
		inline const store_t &const_val() const
		{
			if (m_dispatcher == &dispatcher_class<store_t>::dispatcher || type() == typeid(T))
				return *static_cast<const store_t *>(m_dispatcher(var_op::get, this, nullptr)._ptr);
			else if (m_dispatcher == nullptr)
				throw runtime_error("Instance null variable.");
			else
				throw runtime_error("Instance variable with wrong type. Provided " +
				                    csvm_impl::cxx_demangle(typeid(T).name()) + ", expected " + type_name().data());
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

	template <std::size_t align_size,
	          template <typename> class allocator_t = default_allocator>
	class basic_var_borrower final
	{
		using var = basic_var<align_size, allocator_t>;
		using allocator_type = allocator_t<var>;

		static allocator_type &get_allocator()
		{
			static allocator_type allocator;
			return allocator;
		}

		const var *m_data = nullptr;
		bool m_const = false;
		bool m_own = false;

		void destroy()
		{
			if (m_own && m_data)
			{
				m_data->~basic_var();
				get_allocator().deallocate(const_cast<var *>(m_data), 1);
			}
			m_data = nullptr;
			m_const = false;
			m_own = false;
		}

	   public:
		basic_var_borrower() noexcept = default;

		basic_var_borrower(var &val) noexcept : m_data(&val), m_const(false), m_own(false) {}

		basic_var_borrower(const var &val) noexcept : m_data(&val), m_const(true), m_own(false) {}

		basic_var_borrower(var &&val) : m_own(true), m_const(false)
		{
			var *p = get_allocator().allocate(1);
			::new (p) var(std::move(val));
			m_data = p;
		}

		basic_var_borrower(const basic_var_borrower &other) : m_own(other.m_own), m_const(other.m_const)
		{
			if (other.m_own)
			{
				var *p = get_allocator().allocate(1);
				::new (p) var(*other.m_data);
				m_data = p;
			}
			else
				m_data = other.m_data;
		}

		basic_var_borrower(basic_var_borrower &&other) noexcept
		    : m_data(other.m_data), m_const(other.m_const), m_own(other.m_own)
		{
			other.m_data = nullptr;
			other.m_const = false;
			other.m_own = false;
		}

		template <typename T, typename store_t = csvm_impl::var_storage_t<T>,
		          typename = std::enable_if_t<!std::is_same_v<store_t, var> && !std::is_same_v<store_t, basic_var_borrower>>>
		basic_var_borrower(T &&val) : m_own(true), m_const(false)
		{
			var *p = get_allocator().allocate(1);
			::new (p) var(std::forward<T>(val));
			m_data = p;
		}

		~basic_var_borrower()
		{
			destroy();
		}

		basic_var_borrower &operator=(const basic_var_borrower &other)
		{
			if (this != &other)
			{
				destroy();
				m_own = other.m_own;
				m_const = other.m_const;
				if (other.m_own)
				{
					var *p = get_allocator().allocate(1);
					::new (p) var(*other.m_data);
					m_data = p;
				}
				else
					m_data = other.m_data;
			}
			return *this;
		}

		basic_var_borrower &operator=(basic_var_borrower &&other) noexcept
		{
			if (this != &other)
			{
				destroy();
				m_data = other.m_data;
				m_const = other.m_const;
				m_own = other.m_own;
				other.m_data = nullptr;
				other.m_const = false;
				other.m_own = false;
			}
			return *this;
		}

		var *data() const noexcept
		{
			if (m_const)
				return nullptr;
			else
				return const_cast<var *>(m_data);
		}

		const var *const_data() const noexcept
		{
			return m_data;
		}

		bool usable() const noexcept
		{
			return m_data != nullptr && m_data->usable();
		}

		operator bool() const noexcept
		{
			return usable();
		}

		bool is_null() const noexcept
		{
			return m_data == nullptr || m_data->is_null();
		}

		template <typename T, typename store_t = csvm_impl::var_storage_t<T>>
		const store_t &const_val()
		{
			if (m_data != nullptr)
				return m_data->template val<T>();
			else
				throw runtime_error("Instance null variable.");
		}
	};

	using var = basic_var<CSVM_SVO_ALIGN_SIZE, default_allocator>;
	using var_borrower = basic_var_borrower<CSVM_SVO_ALIGN_SIZE, default_allocator>;
} // namespace csvm
