#include "covscript/common/platform.hpp"
#include "basic.hpp"
#include "numeric.hpp"
#include "string.hpp"
#include <cstdint>
#include <bitset>

// Small Variable Optimization
#ifndef COVSCRIPT_SVO_SIZE
#ifdef COVSCRIPT_64BIT
#define COVSCRIPT_SVO_SIZE 6
#else
#define COVSCRIPT_SVO_SIZE 14
#endif
#endif

namespace cs {
	template<typename T, std::size_t blck_size, template<typename> class allocator_t=std::allocator>
	class allocator_type final {
		T *mPool[blck_size];
		allocator_t<T> mAlloc;
		std::size_t mOffset = 0;
	public:
		allocator_type()
		{
			while (mOffset < blck_size / 2)
				mPool[mOffset++] = mAlloc.allocate(1);
		}

		allocator_type(const allocator_type &) = delete;

		~allocator_type()
		{
			while (mOffset > 0)
				mAlloc.deallocate(mPool[--mOffset], 1);
		}

		template<typename...ArgsT>
		inline T *alloc(ArgsT &&...args)
		{
			T *ptr = nullptr;
			if (mOffset > 0 && global_thread_counter == 0)
				ptr = mPool[--mOffset];
			else
				ptr = mAlloc.allocate(1);
			mAlloc.construct(ptr, std::forward<ArgsT>(args)...);
			return ptr;
		}

		inline void free(T *ptr)
		{
			mAlloc.destroy(ptr);
			if (mOffset < blck_size && global_thread_counter == 0)
				mPool[mOffset++] = ptr;
			else
				mAlloc.deallocate(ptr, 1);
		}
	};

	class alignas(64) var final {
		class base_store {
		public:
			base_store() = default;

			virtual ~base_store() = default;

			virtual const std::type_info &type() const = 0;

			virtual base_store *duplicate(size_t *) = 0;

			virtual bool compare(const base_store *) const = 0;

			virtual numeric_t to_numeric() const = 0;

			virtual byte_string_view to_string() const = 0;

			virtual size_t hash() const = 0;

			virtual void detach() = 0;

			virtual void kill() = 0;

			// virtual cs::namespace_t &get_ext() const = 0;

			virtual byte_string_view get_type_name() const = 0;
		};

		template<typename T>
		class store : public base_store {
		protected:
			T mDat;
		public:
			static default_allocator<store<T>> &get_allocator()
			{
				static default_allocator<store<T>> allocator;
				return allocator;
			}

			store() = default;

			template<typename...ArgsT>
			explicit store(ArgsT &&...args):mDat(std::forward<ArgsT>(args)...) {}

			~ store() override = default;

			const std::type_info &type() const override
			{
				return typeid(T);
			}

			base_store *duplicate(size_t *mem) override
			{
				return ::new (mem) T(mDat);
			}

			bool compare(const base_store *obj) const override
			{
				if (obj->type() == this->type())
					return cs_impl::compare(mDat, static_cast<const store<T> *>(obj)->data());
				else
					return false;
			}

			long to_integer() const override
			{
				return cs_impl::to_integer(mDat);
			}

			byte_string_view to_string() const override
			{
				return cs_impl::to_string(mDat);
			}

			size_t hash() const override
			{
				return cs_impl::hash<T>(mDat);
			}

			void detach() override
			{
				cs_impl::detach(mDat);
			}

			void kill() override
			{
				allocator.free(this);
			}

			// virtual cs::namespace_t &get_ext() const override
			// {
			// 	return cs_impl::get_ext<T>();
			// }

			const char *get_type_name() const override
			{
				return cs_impl::get_name_of_type<T>();
			}

			T &data()
			{
				return mDat;
			}

			const T &data() const
			{
				return mDat;
			}

			void data(const T &dat)
			{
				mDat = dat;
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
	};
}