#include <csvm/types/types.hpp>
#include <csvm/context/context.hpp>
#include <cstring>

namespace csvm
{
	using byte_t = std::uint8_t;

	inline bool is_big_endian()
	{
		std::uint32_t i = 1;
		std::uint8_t *p = reinterpret_cast<std::uint8_t *>(&i);
		return *p == 0;
	}

	inline void to_vm_endian(byte_t *ptr, std::size_t count)
	{
		if (!is_big_endian())
		{
			static byte_t buff[32];
			for (size_t idx = 0; idx < count; ++idx)
				buff[count - idx - 1] = ptr[idx];
			memcpy(ptr, buff, count * sizeof(byte_t));
		}
	}

	inline void to_local_endian(byte_t *ptr, std::size_t count)
	{
		if (!is_big_endian())
		{
			static byte_t buff[32];
			for (size_t idx = 0; idx < count; ++idx)
				buff[idx] = ptr[count - idx - 1];
			memcpy(ptr, buff, count * sizeof(byte_t));
		}
	}

	enum class vm_op : std::uint8_t
	{

	};

    class instance final 
    {
        memory_manager memory;
    };
} // namespace csvm