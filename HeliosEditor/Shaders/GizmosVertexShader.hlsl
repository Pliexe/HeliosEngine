cbuffer CBuf
{
    matrix transform;
};

struct VSOut
{
    float4 color : Color;
    float4 position : SV_Position;
};

VSOut main( float3 pos : Position, float4 col : Color, float id : ID )
{
    VSOut o;
    o.color = col;
    o.position = float4(pos, 1.0f);
	return o;
}