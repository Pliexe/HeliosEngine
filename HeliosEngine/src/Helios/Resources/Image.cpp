#include "Image.h"
#include "Helios/Core/Exceptions.h"

namespace Helios
{
    size_t Image::CalculateSize() const
    {
        size_t formatSize = 0;

        switch (m_format)
        {
            case Format::None: HL_EXCEPTION(true, "Format is None!"); // TODO: Remove HL_EXCEPTION dependency if possible? nah its core.
            case Format::R8G8B8A8_UNORM:
                formatSize = 4;     // uint8 * 4
                break;
            case Format::R32G32B32A32_FLOAT:
                formatSize = 4 * 4; // float * 4
                break;
            case Format::R32G32B32_FLOAT:
                formatSize = 4 * 3; // float * 3
                break;
            case Format::R32G32_FLOAT:
                formatSize = 4 * 2; // float * 2
                break;
            case Format::R32_FLOAT:
                formatSize = 4;     // float * 1
                break;
            case Format::R32G32B32A32_SINT:
                formatSize = 4 * 4; // int32 * 4
                break;
            case Format::R32G32B32_SINT:
                formatSize = 4 * 3; // int32 * 3
                break;
            case Format::R32G32_SINT:
                formatSize = 4 * 2; // int32 * 2
                break;
            case Format::R32_SINT:
                formatSize = 4;     // int32 * 1
                break;
            case Format::R32G32B32A32_UINT:
                formatSize = 4 * 4; // uint32 * 4
                break;
            case Format::R32G32B32_UINT:
                formatSize = 4 * 3; // uint32 * 3
                break;
            case Format::R32G32_UINT:
                formatSize = 4 * 2; // uint32 * 2
                break;
            case Format::R32_UINT:
                formatSize = 4;     // uint32 * 1
                break;
            case Format::R16G16B16A16_FLOAT:
                formatSize = 2 * 4; // float16 * 4
                break;
            case Format::R16G16_FLOAT:
                formatSize = 2 * 2; // float16 * 2
                break;
            case Format::R16_FLOAT:
                formatSize = 2;     // float16 * 1
                break;
            case Format::R16G16B16A16_UNORM:
                formatSize = 2 * 4; // uint16 * 4
                break;
            case Format::R16G16_UNORM:
                formatSize = 2 * 2; // uint16 * 2
                break;
            case Format::R16_UNORM:
                formatSize = 2;     // uint16 * 1
                break;
            case Format::B8G8R8A8_UNORM:
                formatSize = 1 * 4; // uint8 * 4
                break;
            case Format::R8G8_UNORM:
                formatSize = 1 * 2; // uint8 * 2
                break;
            case Format::R8_UNORM:
                formatSize = 1;     // uint8 * 1
                break;
            case Format::D24_UNORM_S8_UINT:
                formatSize = 4;     // __
                break;
            case Format::D32_FLOAT:
                formatSize = 4;     // float * 1
                break;
            default:
                HL_EXCEPTION(true, "Unknown format!");
                break;
        }

        return m_width * m_height * formatSize;
    }
}
