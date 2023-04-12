#pragma once

#include "pch.h"
#include "Helios/Core/Base.h"

namespace Helios {

    static const UINT32 sc_redShift = 16;
    static const UINT32 sc_greenShift = 8;
    static const UINT32 sc_blueShift = 0;

    static const UINT32 sc_redMask = 0xff << sc_redShift;
    static const UINT32 sc_greenMask = 0xff << sc_greenShift;
    static const UINT32 sc_blueMask = 0xff << sc_blueShift;

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
        Color(UINT32 rgb, float alpha = 1.0f) {
            r = static_cast<FLOAT>((rgb & sc_redMask) >> sc_redShift) / 255.f;
            g = static_cast<FLOAT>((rgb & sc_greenMask) >> sc_greenShift) / 255.f;
            b = static_cast<FLOAT>((rgb & sc_blueMask) >> sc_blueShift) / 255.f;
            a = alpha;
        }
        Color(float r, float g, float b, float alpha = 1.0f) { this->r = r; this->g = g; this->b = b; a = alpha; }
        Color(int r, int g, int b, float alpha = 1.0f) { this->r = r / 255.0f; this->g = g / 255.0f; this->b = b / 255.0f; a = alpha; }
        //Color(const Color&) = default;

        operator D2D1_COLOR_F () {
            return D2D1::ColorF(r, g, b, a);
        }

        operator float* () { return &r; }
    };
}