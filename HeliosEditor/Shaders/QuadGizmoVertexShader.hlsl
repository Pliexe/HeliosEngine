struct VSOut
{
    float4 color : Color;
    float data : Data;
    float4 position : SV_Position;
};

cbuffer Cbuf
{
    matrix viewMatrix;
}

VSOut main(float2 pos : Position, matrix world : World, float4 color : Color, float data : Data)
{
    VSOut vso;
    vso.position = mul(float4(pos.x, pos.y, 0.0f, 1.0f), mul(world, viewMatrix));
    vso.color = color;
    vso.data = data;
	return vso;
}