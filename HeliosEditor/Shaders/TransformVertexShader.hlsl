cbuffer buff
{
    matrix viewMatrix;
};

struct VSOut
{
    float4 color : Color;
    float4 position : SV_Position;
};
VSOut main( float3 pos : Position, float4 color : Color )
{
    VSOut vsout;
    vsout.position = float4(pos, 1.0f);
    vsout.color = color;
	return vsout;
}