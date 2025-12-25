#include "UUID.h"

namespace Helios
{
	static std::random_device s_RandomDevice;
	static std::mt19937 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
	{
		for (int i = 0; i < 2; i++)
		{
			uint64_t random = s_UniformDistribution(s_Engine);
			std::memcpy(&m_UUID[i * 8], &random, sizeof(uint64_t));
		}

		m_UUID[6] = (m_UUID[6] & 0x0F) | 0x40; // Set version bits (4)
		m_UUID[8] = (m_UUID[8] & 0x3F) | 0x80; // Set variant bits (10)
	}

	UUID::UUID(std::uint8_t data[16])
	{
		std::memcpy(m_UUID, data, 16);
	}

	bool UUID::operator==(const UUID& other) const
	{
		return std::memcmp(m_UUID, other.m_UUID, 16) == 0;
	}

	std::string UUID::toString() const
	{
		std::stringstream ss;
		ss << std::hex << std::setfill('0');

		for (int i = 0; i < 16; i++)
		{
			ss << std::setw(2) << static_cast<unsigned>(m_UUID[i]);
			if (i == 3 || i == 5 || i == 7 || i == 9)
				ss << '-';
		}
		return ss.str();
	}

	size_t UUID::hash() const
	{
		uint64_t a = *reinterpret_cast<const uint64_t*>(m_UUID);
		uint64_t b = *reinterpret_cast<const uint64_t*>(&m_UUID[8]);

		return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
	}

	UUID UUID::fromString(const std::string& string)
	{
		UUID uuid;

		for (int i = 0, j = 0; i < string.size(); i++) {
			if (string[i] == '-') {
				continue;
			}

			char highNibble = string[i];
			char lowNibble = string[i + 1];

			// Convert the hexadecimal digits to 4-bit values
			std::uint8_t highNibbleValue = (highNibble <= '9') ? (highNibble - '0') : (highNibble - 'a' + 10);
			std::uint8_t lowNibbleValue = (lowNibble <= '9') ? (lowNibble - '0') : (lowNibble - 'a' + 10);

			// Combine the 4-bit values into a single 8-bit value
			uuid.m_UUID[j++] = (highNibbleValue << 4) | lowNibbleValue;

			i++; // Skip the next character
		}

		return uuid;
	}
}
