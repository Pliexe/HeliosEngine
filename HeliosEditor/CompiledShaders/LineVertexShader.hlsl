cbuffer Cbuf
{
    matrix viewProjection;
};

struct VSOut
{
    float4 col : Color;
    float thickness : Thickness;
    int mode : Mode;
    int id : Id;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float4 col : Color, int mode : Mode, float thickness : Thickness, int id : Id)
{
    VSOut vout;
    vout.pos = mul(float4(pos, 1.0f), viewProjection);
    vout.col = col;
    vout.id = id;
    vout.thickness = thickness;
    vout.mode = mode;
	return vout;
}