struct VSOut
{
    float4 col : Color;
    float4 pos : SV_Position;
};

VSOut main( float2 pos : Position, float4 col : Color )
{
    VSOut vout;
    vout.col = col;
    vout.pos = float4(pos, 0.0f, 0.0f);

    return vout;
}