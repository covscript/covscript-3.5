#pragma once
#include <csvm/types/basic.hpp>
#include <type_traits>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace csvm
{
	class numeric_t final
	{
		union
		{
			float_t _num;
			integer_t _int;
		} data;
		bool type = 1;

		inline static std::uint8_t get_composite_type(bool lhs, bool rhs) noexcept
		{
			return lhs << 1 | rhs;
		}

	   public:
		numeric_t()
		{
			data._int = 0;
		}

		numeric_t(float_t num)
		{
			type = 0;
			data._num = num;
		}

		numeric_t(integer_t num)
		{
			type = 1;
			data._int = num;
		}

		numeric_t(const numeric_t &rhs) : data(rhs.data), type(rhs.type) {}

		numeric_t(numeric_t &&rhs) noexcept : data(rhs.data), type(rhs.type) {}

		~numeric_t() = default;

		numeric_t operator+(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num + rhs.data._num;
				case 0b01:
					return data._num + rhs.data._int;
				case 0b10:
					return data._int + rhs.data._num;
				case 0b11:
					return data._int + rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		numeric_t operator+(T &&rhs) const noexcept
		{
			if (type)
				return data._int + rhs;
			else
				return data._num + rhs;
		}

		numeric_t operator-(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num - rhs.data._num;
				case 0b01:
					return data._num - rhs.data._int;
				case 0b10:
					return data._int - rhs.data._num;
				case 0b11:
					return data._int - rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		numeric_t operator-(T &&rhs) const noexcept
		{
			if (type)
				return data._int - rhs;
			else
				return data._num - rhs;
		}

		numeric_t operator*(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num * rhs.data._num;
				case 0b01:
					return data._num * rhs.data._int;
				case 0b10:
					return data._int * rhs.data._num;
				case 0b11:
					return data._int * rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		numeric_t operator*(T &&rhs) const noexcept
		{
			if (type)
				return data._int * rhs;
			else
				return data._num * rhs;
		}

		numeric_t operator/(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num / rhs.data._num;
				case 0b01:
					return data._num / rhs.data._int;
				case 0b10:
					return data._int / rhs.data._num;
				case 0b11:
					std::lldiv_t divres = std::lldiv(data._int, rhs.data._int);
					if (divres.rem == 0)
						return divres.quot;
					else
						return static_cast<float_t>(data._int) / rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		numeric_t operator/(T &&rhs) const noexcept
		{
			if (type)
				return data._int / rhs;
			else
				return data._num / rhs;
		}

		numeric_t &operator=(const numeric_t &num)
		{
			if (this != &num)
			{
				data = num.data;
				type = num.type;
			}
			return *this;
		}

		numeric_t &operator=(float_t num)
		{
			type = 0;
			data._num = num;
			return *this;
		}

		numeric_t &operator=(integer_t num)
		{
			type = 1;
			data._int = num;
			return *this;
		}

		bool operator<(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num < rhs.data._num;
				case 0b01:
					return data._num < rhs.data._int;
				case 0b10:
					return data._int < rhs.data._num;
				case 0b11:
					return data._int < rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		bool operator<(T &&rhs) const noexcept
		{
			if (type)
				return data._int < rhs;
			else
				return data._num < rhs;
		}

		bool operator<=(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num <= rhs.data._num;
				case 0b01:
					return data._num <= rhs.data._int;
				case 0b10:
					return data._int <= rhs.data._num;
				case 0b11:
					return data._int <= rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		bool operator<=(T &&rhs) const noexcept
		{
			if (type)
				return data._int <= rhs;
			else
				return data._num <= rhs;
		}

		bool operator>(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num > rhs.data._num;
				case 0b01:
					return data._num > rhs.data._int;
				case 0b10:
					return data._int > rhs.data._num;
				case 0b11:
					return data._int > rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		bool operator>(T &&rhs) const noexcept
		{
			if (type)
				return data._int > rhs;
			else
				return data._num > rhs;
		}

		bool operator>=(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num >= rhs.data._num;
				case 0b01:
					return data._num >= rhs.data._int;
				case 0b10:
					return data._int >= rhs.data._num;
				case 0b11:
					return data._int >= rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		bool operator>=(T &&rhs) const noexcept
		{
			if (type)
				return data._int >= rhs;
			else
				return data._num >= rhs;
		}

		bool operator==(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num == rhs.data._num;
				case 0b01:
					return data._num == rhs.data._int;
				case 0b10:
					return data._int == rhs.data._num;
				case 0b11:
					return data._int == rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		bool operator==(T &&rhs) const noexcept
		{
			if (type)
				return data._int == rhs;
			else
				return data._num == rhs;
		}

		bool operator!=(const numeric_t &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type))
			{
				default:
				case 0b00:
					return data._num != rhs.data._num;
				case 0b01:
					return data._num != rhs.data._int;
				case 0b10:
					return data._int != rhs.data._num;
				case 0b11:
					return data._int != rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric_t>::value>>
		bool operator!=(T &&rhs) const noexcept
		{
			if (type)
				return data._int != rhs;
			else
				return data._num != rhs;
		}

		numeric_t &operator++() noexcept
		{
			if (type)
				++data._int;
			else
				++data._num;
			return *this;
		}

		numeric_t &operator--() noexcept
		{
			if (type)
				--data._int;
			else
				--data._num;
			return *this;
		}

		numeric_t operator++(int) noexcept
		{
			if (type)
				return data._int++;
			else
				return data._num++;
		}

		numeric_t operator--(int) noexcept
		{
			if (type)
				return data._int--;
			else
				return data._num--;
		}

		numeric_t operator-() const noexcept
		{
			if (type)
				return -data._int;
			else
				return -data._num;
		}

		bool is_integer() const noexcept
		{
			return type;
		}

		bool is_float() const noexcept
		{
			return !type;
		}

		integer_t as_integer() const noexcept
		{
			if (type)
				return data._int;
			else
				return data._num;
		}

		float_t as_float() const noexcept
		{
			if (type)
				return data._int;
			else
				return data._num;
		}

		byte_string_t to_string() const;
	};
} // namespace csvm