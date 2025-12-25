#pragma once

#include "Helios/Resources/Image.h"

namespace Helios
{
    struct ImageResourceKey
    {
        uint32 width;
        uint32 height;
        uint32 layers;
        uint32 samples;
        Image::Format format;
        Image::Usage usage;

        bool operator==(const ImageResourceKey& other) const
        {
            return width == other.width &&
                height == other.height &&
                layers == other.layers &&
                samples == other.samples &&
                format == other.format &&
                usage == other.usage;
        }
    };
}

namespace std
{
    template<>
    struct hash<Helios::ImageResourceKey>
    {
        size_t operator()(const Helios::ImageResourceKey& key) const noexcept
        {
            size_t h = 0;
            auto hashCombine = [&h](auto&& v)
            {
                h ^= std::hash<std::decay_t<decltype(v)>>{}(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
            };

            hashCombine(key.width);
            hashCombine(key.height);
            hashCombine(key.layers);
            hashCombine(key.samples);
            hashCombine(key.format);
            hashCombine(key.usage);

            return h;
        }
    };
}