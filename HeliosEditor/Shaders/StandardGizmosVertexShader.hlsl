struct VSOut
{
    float4 color : Color;
    float id : Id;
    float4 position : SV_Position;
};

cbuffer Cbuf
{
    matrix viewMatrix;
}

VSOut main(float3 position : Position, matrix transform : Transform, float4 color : Color, float id : Id)
{
    VSOut vso;
    //vso.position = float4(position, 1.0f);
    vso.position = mul(float4(position, 1.0f), mul(transform, viewMatrix));
    vso.color = color;
    vso.id = id;
    return vso;
}