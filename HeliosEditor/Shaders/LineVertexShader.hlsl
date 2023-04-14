cbuffer Cbuf
{
    matrix viewProjection;
};

struct VSOut
{
    float4 col : Color;
    float width : Width;
    int mode : Mode;
    float id : Id;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position, float4 col : Color, int mode : Mode, float width : Width, float id : Id )
{
    VSOut vout;
    vout.pos = mul(float4(pos, 1.0f), viewProjection);
    vout.col = col;
    vout.id = id;
    vout.width = width;
    vout.mode = mode;
	return vout;
}