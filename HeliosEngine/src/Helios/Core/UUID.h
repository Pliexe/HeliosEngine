#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios
{
	class HELIOS_API UUID
	{
	public:

		UUID();

		UUID(const UUID& other) = default;
		UUID& operator=(const UUID& other) = default;
		UUID(std::uint8_t data[16]);

		bool operator==(const UUID& other) const;
		operator uint8_t* () { return m_UUID; }

		[[nodiscard]] std::string toString() const;

		size_t hash() const;

		static UUID fromString(const std::string& string);

		bool operator<(const UUID& other) const
		{
			return std::lexicographical_compare(
				std::begin(m_UUID), std::end(m_UUID),
				std::begin(other.m_UUID), std::end(other.m_UUID)
			);
		}

	private:
		std::uint8_t m_UUID[16];
	};


}

namespace std
{
	template<>
	struct hash<Helios::UUID>
	{
		size_t operator()(const Helios::UUID& uuid) const
		{
			return uuid.hash();
		}
	};
}