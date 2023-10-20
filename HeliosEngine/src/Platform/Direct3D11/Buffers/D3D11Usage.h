#include "pch.h"
#include "Helios/Resources/Buffer.h"

#ifdef HELIOS_PLATFORM_WINDOWS
namespace Helios
{
    /**
	 * Retrieves the D3D11 usage mode based on the specified buffer usage.
	 *
	 * @param usage The buffer usage.
	 *
	 * @return The corresponding D3D11 usage mode.
	 */
	D3D11_USAGE GetBufferUsage(BufferUsage usage)
	{
		switch (usage)
		{
		case Helios::BufferUsage::Static:
            return D3D11_USAGE_IMMUTABLE;
		case Helios::BufferUsage::Dynamic:
		case Helios::BufferUsage::Stream:
            return D3D11_USAGE_DYNAMIC;
		case Helios::BufferUsage::Staging:
			return D3D11_USAGE_STAGING;
		}
	}
}
#endif