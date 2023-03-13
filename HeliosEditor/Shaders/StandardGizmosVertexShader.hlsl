struct VSOut
{
    float4 color : Color;
    float data : Data;
    float4 position : SV_Position;
};

cbuffer Cbuf
{
    matrix viewMatrix;
    matrix world;
}

VSOut main(float4 color : Color, float3 pos : Position, float data : Data)
{
    VSOut vso;
    vso.position = mul(float4(pos.x, pos.y, pos.z, 1.0f), mul(world, viewMatrix));
    vso.color = color;
    vso.data = data;
    return vso;
}