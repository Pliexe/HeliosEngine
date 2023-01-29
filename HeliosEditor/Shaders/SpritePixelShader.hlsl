Texture2D<float4> tex[128] : register(t0); // 128 textures, 0-127

sampler samp : register(s0);

struct PSOut
{
    float4 Color : SV_Target0;
    float4 EngineSelect : SV_Target1;
};

PSOut main(float4 color : Color, uint id : TextureID, float2 texCoord : TexCoord, float entityId : EntityId)
{
    PSOut o;

    switch (id)
    {
        case   0: o.Color = tex[  0].Sample(samp, texCoord) * color; break;
        case   1: o.Color = tex[  1].Sample(samp, texCoord) * color; break;
        case   2: o.Color = tex[  2].Sample(samp, texCoord) * color; break;
        case   3: o.Color = tex[  3].Sample(samp, texCoord) * color; break;
        case   4: o.Color = tex[  4].Sample(samp, texCoord) * color; break;
        case   5: o.Color = tex[  5].Sample(samp, texCoord) * color; break;
        case   6: o.Color = tex[  6].Sample(samp, texCoord) * color; break;
        case   7: o.Color = tex[  7].Sample(samp, texCoord) * color; break;
        case   8: o.Color = tex[  8].Sample(samp, texCoord) * color; break;
        case   9: o.Color = tex[  9].Sample(samp, texCoord) * color; break;
        case  10: o.Color = tex[ 10].Sample(samp, texCoord) * color; break;
        case  11: o.Color = tex[ 11].Sample(samp, texCoord) * color; break;
        case  12: o.Color = tex[ 12].Sample(samp, texCoord) * color; break;
        case  13: o.Color = tex[ 13].Sample(samp, texCoord) * color; break;
        case  14: o.Color = tex[ 14].Sample(samp, texCoord) * color; break;
        case  15: o.Color = tex[ 15].Sample(samp, texCoord) * color; break;
        case  16: o.Color = tex[ 16].Sample(samp, texCoord) * color; break;
        case  17: o.Color = tex[ 17].Sample(samp, texCoord) * color; break;
        case  18: o.Color = tex[ 18].Sample(samp, texCoord) * color; break;
        case  19: o.Color = tex[ 19].Sample(samp, texCoord) * color; break;
        case  20: o.Color = tex[ 20].Sample(samp, texCoord) * color; break;
        case  21: o.Color = tex[ 21].Sample(samp, texCoord) * color; break;
        case  22: o.Color = tex[ 22].Sample(samp, texCoord) * color; break;
        case  23: o.Color = tex[ 23].Sample(samp, texCoord) * color; break;
        case  24: o.Color = tex[ 24].Sample(samp, texCoord) * color; break;
        case  25: o.Color = tex[ 25].Sample(samp, texCoord) * color; break;
        case  26: o.Color = tex[ 26].Sample(samp, texCoord) * color; break;
        case  27: o.Color = tex[ 27].Sample(samp, texCoord) * color; break;
        case  28: o.Color = tex[ 28].Sample(samp, texCoord) * color; break;
        case  29: o.Color = tex[ 29].Sample(samp, texCoord) * color; break;
        case  30: o.Color = tex[ 30].Sample(samp, texCoord) * color; break;
        case  31: o.Color = tex[ 31].Sample(samp, texCoord) * color; break;
        case  32: o.Color = tex[ 32].Sample(samp, texCoord) * color; break;
        case  33: o.Color = tex[ 33].Sample(samp, texCoord) * color; break;
        case  34: o.Color = tex[ 34].Sample(samp, texCoord) * color; break;
        case  35: o.Color = tex[ 35].Sample(samp, texCoord) * color; break;
        case  36: o.Color = tex[ 36].Sample(samp, texCoord) * color; break;
        case  37: o.Color = tex[ 37].Sample(samp, texCoord) * color; break;
        case  38: o.Color = tex[ 38].Sample(samp, texCoord) * color; break;
        case  39: o.Color = tex[ 39].Sample(samp, texCoord) * color; break;
        case  40: o.Color = tex[ 40].Sample(samp, texCoord) * color; break;
        case  41: o.Color = tex[ 41].Sample(samp, texCoord) * color; break;
        case  42: o.Color = tex[ 42].Sample(samp, texCoord) * color; break;
        case  43: o.Color = tex[ 43].Sample(samp, texCoord) * color; break;
        case  44: o.Color = tex[ 44].Sample(samp, texCoord) * color; break;
        case  45: o.Color = tex[ 45].Sample(samp, texCoord) * color; break;
        case  46: o.Color = tex[ 46].Sample(samp, texCoord) * color; break;
        case  47: o.Color = tex[ 47].Sample(samp, texCoord) * color; break;
        case  48: o.Color = tex[ 48].Sample(samp, texCoord) * color; break;
        case  49: o.Color = tex[ 49].Sample(samp, texCoord) * color; break;
        case  50: o.Color = tex[ 50].Sample(samp, texCoord) * color; break;
        case  51: o.Color = tex[ 51].Sample(samp, texCoord) * color; break;
        case  52: o.Color = tex[ 52].Sample(samp, texCoord) * color; break;
        case  53: o.Color = tex[ 53].Sample(samp, texCoord) * color; break;
        case  54: o.Color = tex[ 54].Sample(samp, texCoord) * color; break;
        case  55: o.Color = tex[ 55].Sample(samp, texCoord) * color; break;
        case  56: o.Color = tex[ 56].Sample(samp, texCoord) * color; break;
        case  57: o.Color = tex[ 57].Sample(samp, texCoord) * color; break;
        case  58: o.Color = tex[ 58].Sample(samp, texCoord) * color; break;
        case  59: o.Color = tex[ 59].Sample(samp, texCoord) * color; break;
        case  60: o.Color = tex[ 60].Sample(samp, texCoord) * color; break;
        case  61: o.Color = tex[ 61].Sample(samp, texCoord) * color; break;
        case  62: o.Color = tex[ 62].Sample(samp, texCoord) * color; break;
        case  63: o.Color = tex[ 63].Sample(samp, texCoord) * color; break;
        case  64: o.Color = tex[ 64].Sample(samp, texCoord) * color; break;
        case  65: o.Color = tex[ 65].Sample(samp, texCoord) * color; break;
        case  66: o.Color = tex[ 66].Sample(samp, texCoord) * color; break;
        case  67: o.Color = tex[ 67].Sample(samp, texCoord) * color; break;
        case  68: o.Color = tex[ 68].Sample(samp, texCoord) * color; break;
        case  69: o.Color = tex[ 69].Sample(samp, texCoord) * color; break;
        case  70: o.Color = tex[ 70].Sample(samp, texCoord) * color; break;
        case  71: o.Color = tex[ 71].Sample(samp, texCoord) * color; break;
        case  72: o.Color = tex[ 72].Sample(samp, texCoord) * color; break;
        case  73: o.Color = tex[ 73].Sample(samp, texCoord) * color; break;
        case  74: o.Color = tex[ 74].Sample(samp, texCoord) * color; break;
        case  75: o.Color = tex[ 75].Sample(samp, texCoord) * color; break;
        case  76: o.Color = tex[ 76].Sample(samp, texCoord) * color; break;
        case  77: o.Color = tex[ 77].Sample(samp, texCoord) * color; break;
        case  78: o.Color = tex[ 78].Sample(samp, texCoord) * color; break;
        case  79: o.Color = tex[ 79].Sample(samp, texCoord) * color; break;
        case  80: o.Color = tex[ 80].Sample(samp, texCoord) * color; break;
        case  81: o.Color = tex[ 81].Sample(samp, texCoord) * color; break;
        case  82: o.Color = tex[ 82].Sample(samp, texCoord) * color; break;
        case  83: o.Color = tex[ 83].Sample(samp, texCoord) * color; break;
        case  84: o.Color = tex[ 84].Sample(samp, texCoord) * color; break;
        case  85: o.Color = tex[ 85].Sample(samp, texCoord) * color; break;
        case  86: o.Color = tex[ 86].Sample(samp, texCoord) * color; break;
        case  87: o.Color = tex[ 87].Sample(samp, texCoord) * color; break;
        case  88: o.Color = tex[ 88].Sample(samp, texCoord) * color; break;
        case  89: o.Color = tex[ 89].Sample(samp, texCoord) * color; break;
        case  90: o.Color = tex[ 90].Sample(samp, texCoord) * color; break;
        case  91: o.Color = tex[ 91].Sample(samp, texCoord) * color; break;
        case  92: o.Color = tex[ 92].Sample(samp, texCoord) * color; break;
        case  93: o.Color = tex[ 93].Sample(samp, texCoord) * color; break;
        case  94: o.Color = tex[ 94].Sample(samp, texCoord) * color; break;
        case  95: o.Color = tex[ 95].Sample(samp, texCoord) * color; break;
        case  96: o.Color = tex[ 96].Sample(samp, texCoord) * color; break;
        case  97: o.Color = tex[ 97].Sample(samp, texCoord) * color; break;
        case  98: o.Color = tex[ 98].Sample(samp, texCoord) * color; break;
        case  99: o.Color = tex[ 99].Sample(samp, texCoord) * color; break;
        case 100: o.Color = tex[100].Sample(samp, texCoord) * color; break;
        case 101: o.Color = tex[101].Sample(samp, texCoord) * color; break;
        case 102: o.Color = tex[102].Sample(samp, texCoord) * color; break;
        case 103: o.Color = tex[103].Sample(samp, texCoord) * color; break;
        case 104: o.Color = tex[104].Sample(samp, texCoord) * color; break;
        case 105: o.Color = tex[105].Sample(samp, texCoord) * color; break;
        case 106: o.Color = tex[106].Sample(samp, texCoord) * color; break;
        case 107: o.Color = tex[107].Sample(samp, texCoord) * color; break;
        case 108: o.Color = tex[108].Sample(samp, texCoord) * color; break;
        case 109: o.Color = tex[109].Sample(samp, texCoord) * color; break;
        case 110: o.Color = tex[110].Sample(samp, texCoord) * color; break;
        case 111: o.Color = tex[111].Sample(samp, texCoord) * color; break;
        case 112: o.Color = tex[112].Sample(samp, texCoord) * color; break;
        case 113: o.Color = tex[113].Sample(samp, texCoord) * color; break;
        case 114: o.Color = tex[114].Sample(samp, texCoord) * color; break;
        case 115: o.Color = tex[115].Sample(samp, texCoord) * color; break;
        case 116: o.Color = tex[116].Sample(samp, texCoord) * color; break;
        case 117: o.Color = tex[117].Sample(samp, texCoord) * color; break;
        case 118: o.Color = tex[118].Sample(samp, texCoord) * color; break;
        case 119: o.Color = tex[119].Sample(samp, texCoord) * color; break;
        case 120: o.Color = tex[120].Sample(samp, texCoord) * color; break;
        case 121: o.Color = tex[121].Sample(samp, texCoord) * color; break;
        case 122: o.Color = tex[122].Sample(samp, texCoord) * color; break;
        case 123: o.Color = tex[123].Sample(samp, texCoord) * color; break;
        case 124: o.Color = tex[124].Sample(samp, texCoord) * color; break;
        case 125: o.Color = tex[125].Sample(samp, texCoord) * color; break;
        case 126: o.Color = tex[126].Sample(samp, texCoord) * color; break;
        case 127: o.Color = tex[127].Sample(samp, texCoord) * color; break;
        default: o.Color = color; break;
    }

    if(o.Color[3] == 0.0f)
        o.EngineSelect = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    else
        o.EngineSelect = float4(entityId, 0.0f, 0.0f, 0.0f);

    return o;
}