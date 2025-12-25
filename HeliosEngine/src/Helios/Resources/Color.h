#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios {

    static const uint32_t sc_redShift = 16;
    static const uint32_t sc_greenShift = 8;
    static const uint32_t sc_blueShift = 0;

    static const uint32_t sc_redMask = 0xff << sc_redShift;
    static const uint32_t sc_greenMask = 0xff << sc_greenShift;
    static const uint32_t sc_blueMask = 0xff << sc_blueShift;
    struct HELIOS_API Color {
        
        union {
            float c[4];
            struct {
                float r;
                float g;
                float b;
                float a;
            };
        };
		
#pragma region Static Colors
        const static Color AliceBlue;
        const static Color AntiqueWhite;
        const static Color Aqua;
        const static Color Aquamarine;
        const static Color Azure;
        const static Color Beige;
        const static Color Bisque;
        const static Color Black;
        const static Color BlanchedAlmond;
        const static Color Blue;
        const static Color BlueViolet;
        const static Color Brown;
        const static Color BurlyWood;
        const static Color CadetBlue;
        const static Color Chartreuse;
        const static Color Chocolate;
        const static Color Coral;
        const static Color CornflowerBlue;
        const static Color Cornsilk;
        const static Color Crimson;
        const static Color Cyan;
        const static Color DarkBlue;
        const static Color DarkCyan;
        const static Color DarkGoldenrod;
        const static Color DarkGray;
        const static Color DarkGreen;
        const static Color DarkKhaki;
        const static Color DarkMagenta;
        const static Color DarkOliveGreen;
        const static Color DarkOrange;
        const static Color DarkOrchid;
        const static Color DarkRed;
        const static Color DarkSalmon;
        const static Color DarkSeaGreen;
        const static Color DarkSlateBlue;
        const static Color DarkSlateGray;
        const static Color DarkTurquoise;
        const static Color DarkViolet;
        const static Color DeepPink;
        const static Color DeepSkyBlue;
        const static Color DimGray;
        const static Color DodgerBlue;
        const static Color Firebrick;
        const static Color FloralWhite;
        const static Color ForestGreen;
        const static Color Fuchsia;
        const static Color Gainsboro;
        const static Color GhostWhite;
        const static Color Gold;
        const static Color Goldenrod;
        const static Color Gray;
        const static Color Green;
        const static Color GreenYellow;
        const static Color Honeydew;
        const static Color HotPink;
        const static Color IndianRed;
        const static Color Indigo;
        const static Color Ivory;
        const static Color Khaki;
        const static Color Lavender;
        const static Color LavenderBlush;
        const static Color LawnGreen;
        const static Color LemonChiffon;
        const static Color LightBlue;
        const static Color LightCoral;
        const static Color LightCyan;
        const static Color LightGoldenrodYellow;
        const static Color LightGreen;
        const static Color LightGray;
        const static Color LightPink;
        const static Color LightSalmon;
        const static Color LightSeaGreen;
        const static Color LightSkyBlue;
        const static Color LightSlateGray;
        const static Color LightSteelBlue;
        const static Color LightYellow;
        const static Color Lime;
        const static Color LimeGreen;
        const static Color Linen;
        const static Color Magenta;
        const static Color Maroon;
        const static Color MediumAquamarine;
        const static Color MediumBlue;
        const static Color MediumOrchid;
        const static Color MediumPurple;
        const static Color MediumSeaGreen;
        const static Color MediumSlateBlue;
        const static Color MediumSpringGreen;
        const static Color MediumTurquoise;
        const static Color MediumVioletRed;
        const static Color MidnightBlue;
        const static Color MintCream;
        const static Color MistyRose;
        const static Color Moccasin;
        const static Color NavajoWhite;
        const static Color Navy;
        const static Color OldLace;
        const static Color Olive;
        const static Color OliveDrab;
        const static Color Orange;
        const static Color OrangeRed;
        const static Color Orchid;
        const static Color PaleGoldenrod;
        const static Color PaleGreen;
        const static Color PaleTurquoise;
        const static Color PaleVioletRed;
        const static Color PapayaWhip;
        const static Color PeachPuff;
        const static Color Peru;
        const static Color Pink;
        const static Color Plum;
        const static Color PowderBlue;
        const static Color Purple;
        const static Color Red;
        const static Color RosyBrown;
        const static Color RoyalBlue;
        const static Color SaddleBrown;
        const static Color Salmon;
        const static Color SandyBrown;
        const static Color SeaGreen;
        const static Color SeaShell;
        const static Color Sienna;
        const static Color Silver;
        const static Color SkyBlue;
        const static Color SlateBlue;
        const static Color SlateGray;
        const static Color Snow;
        const static Color SpringGreen;
        const static Color SteelBlue;
        const static Color Tan;
        const static Color Teal;
        const static Color Thistle;
        const static Color Tomato;
        const static Color Turquoise;
        const static Color Violet;
        const static Color Wheat;
        const static Color White;
        const static Color WhiteSmoke;
        const static Color Yellow;
        const static Color YellowGreen;
#pragma endregion

		Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        Color(const Color&) = default;
        Color(unsigned int rgb, float alpha = 1.0f) {
            r = static_cast<float>((rgb & sc_redMask) >> sc_redShift) / 255.f;
            g = static_cast<float>((rgb & sc_greenMask) >> sc_greenShift) / 255.f;
            b = static_cast<float>((rgb & sc_blueMask) >> sc_blueShift) / 255.f;
            a = alpha;
        }
        Color(float r, float g, float b, float alpha = 1.0f) { this->r = r; this->g = g; this->b = b; a = alpha; }
        Color(int r, int g, int b, float alpha = 1.0f) { this->r = r / 255.0f; this->g = g / 255.0f; this->b = b / 255.0f; a = alpha; }
        //Color(const Color&) = default;

        operator float* () { return &r; }
    };

    struct HELIOS_API ColorU8
    {
        #pragma region Static Colors
        const static ColorU8 AliceBlue;
        const static ColorU8 AntiqueWhite;
        const static ColorU8 Aqua;
        const static ColorU8 Aquamarine;
        const static ColorU8 Azure;
        const static ColorU8 Beige;
        const static ColorU8 Bisque;
        const static ColorU8 Black;
        const static ColorU8 BlanchedAlmond;
        const static ColorU8 Blue;
        const static ColorU8 BlueViolet;
        const static ColorU8 Brown;
        const static ColorU8 BurlyWood;
        const static ColorU8 CadetBlue;
        const static ColorU8 Chartreuse;
        const static ColorU8 Chocolate;
        const static ColorU8 Coral;
        const static ColorU8 CornflowerBlue;
        const static ColorU8 Cornsilk;
        const static ColorU8 Crimson;
        const static ColorU8 Cyan;
        const static ColorU8 DarkBlue;
        const static ColorU8 DarkCyan;
        const static ColorU8 DarkGoldenrod;
        const static ColorU8 DarkGray;
        const static ColorU8 DarkGreen;
        const static ColorU8 DarkKhaki;
        const static ColorU8 DarkMagenta;
        const static ColorU8 DarkOliveGreen;
        const static ColorU8 DarkOrange;
        const static ColorU8 DarkOrchid;
        const static ColorU8 DarkRed;
        const static ColorU8 DarkSalmon;
        const static ColorU8 DarkSeaGreen;
        const static ColorU8 DarkSlateBlue;
        const static ColorU8 DarkSlateGray;
        const static ColorU8 DarkTurquoise;
        const static ColorU8 DarkViolet;
        const static ColorU8 DeepPink;
        const static ColorU8 DeepSkyBlue;
        const static ColorU8 DimGray;
        const static ColorU8 DodgerBlue;
        const static ColorU8 Firebrick;
        const static ColorU8 FloralWhite;
        const static ColorU8 ForestGreen;
        const static ColorU8 Fuchsia;
        const static ColorU8 Gainsboro;
        const static ColorU8 GhostWhite;
        const static ColorU8 Gold;
        const static ColorU8 Goldenrod;
        const static ColorU8 Gray;
        const static ColorU8 Green;
        const static ColorU8 GreenYellow;
        const static ColorU8 Honeydew;
        const static ColorU8 HotPink;
        const static ColorU8 IndianRed;
        const static ColorU8 Indigo;
        const static ColorU8 Ivory;
        const static ColorU8 Khaki;
        const static ColorU8 Lavender;
        const static ColorU8 LavenderBlush;
        const static ColorU8 LawnGreen;
        const static ColorU8 LemonChiffon;
        const static ColorU8 LightBlue;
        const static ColorU8 LightCoral;
        const static ColorU8 LightCyan;
        const static ColorU8 LightGoldenrodYellow;
        const static ColorU8 LightGreen;
        const static ColorU8 LightGray;
        const static ColorU8 LightPink;
        const static ColorU8 LightSalmon;
        const static ColorU8 LightSeaGreen;
        const static ColorU8 LightSkyBlue;
        const static ColorU8 LightSlateGray;
        const static ColorU8 LightSteelBlue;
        const static ColorU8 LightYellow;
        const static ColorU8 Lime;
        const static ColorU8 LimeGreen;
        const static ColorU8 Linen;
        const static ColorU8 Magenta;
        const static ColorU8 Maroon;
        const static ColorU8 MediumAquamarine;
        const static ColorU8 MediumBlue;
        const static ColorU8 MediumOrchid;
        const static ColorU8 MediumPurple;
        const static ColorU8 MediumSeaGreen;
        const static ColorU8 MediumSlateBlue;
        const static ColorU8 MediumSpringGreen;
        const static ColorU8 MediumTurquoise;
        const static ColorU8 MediumVioletRed;
        const static ColorU8 MidnightBlue;
        const static ColorU8 MintCream;
        const static ColorU8 MistyRose;
        const static ColorU8 Moccasin;
        const static ColorU8 NavajoWhite;
        const static ColorU8 Navy;
        const static ColorU8 OldLace;
        const static ColorU8 Olive;
        const static ColorU8 OliveDrab;
        const static ColorU8 Orange;
        const static ColorU8 OrangeRed;
        const static ColorU8 Orchid;
        const static ColorU8 PaleGoldenrod;
        const static ColorU8 PaleGreen;
        const static ColorU8 PaleTurquoise;
        const static ColorU8 PaleVioletRed;
        const static ColorU8 PapayaWhip;
        const static ColorU8 PeachPuff;
        const static ColorU8 Peru;
        const static ColorU8 Pink;
        const static ColorU8 Plum;
        const static ColorU8 PowderBlue;
        const static ColorU8 Purple;
        const static ColorU8 Red;
        const static ColorU8 RosyBrown;
        const static ColorU8 RoyalBlue;
        const static ColorU8 SaddleBrown;
        const static ColorU8 Salmon;
        const static ColorU8 SandyBrown;
        const static ColorU8 SeaGreen;
        const static ColorU8 SeaShell;
        const static ColorU8 Sienna;
        const static ColorU8 Silver;
        const static ColorU8 SkyBlue;
        const static ColorU8 SlateBlue;
        const static ColorU8 SlateGray;
        const static ColorU8 Snow;
        const static ColorU8 SpringGreen;
        const static ColorU8 SteelBlue;
        const static ColorU8 Tan;
        const static ColorU8 Teal;
        const static ColorU8 Thistle;
        const static ColorU8 Tomato;
        const static ColorU8 Turquoise;
        const static ColorU8 Violet;
        const static ColorU8 Wheat;
        const static ColorU8 White;
        const static ColorU8 WhiteSmoke;
        const static ColorU8 Yellow;
        const static ColorU8 YellowGreen;
#pragma endregion

        union {
            std::uint8_t c[4];
            struct {
                std::uint8_t r;
                std::uint8_t g;
                std::uint8_t b;
                std::uint8_t a;
            };
        };

        ColorU8() : r(255u), g(255u), b(255u), a(255u) {}
        ColorU8(const ColorU8&) = default;
        ColorU8(unsigned int rgb, float alpha = 1.0f) {
            r = static_cast<std::uint8_t>((rgb & sc_redMask) >> sc_redShift);
            g = static_cast<std::uint8_t>((rgb & sc_greenMask) >> sc_greenShift);
            b = static_cast<std::uint8_t>((rgb & sc_blueMask) >> sc_blueShift);
            a = alpha * 255;
        }
        ColorU8(float r, float g, float b, float alpha = 1.0f) { this->r = r * 255.0f; this->g = g * 255.0f; this->b = b * 255.0f; a = alpha * 255.0f; }
        ColorU8(int r, int g, int b, float alpha = 1.0f) {  this->r = r; this->g = g; this->b = b; a = alpha * 255.0f; }

        operator std::uint8_t* () { return &r; }
    };

}