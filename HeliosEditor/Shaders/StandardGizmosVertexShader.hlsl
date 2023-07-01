struct VSOut
{
    float4 color : Color;
    int id : Id;
    float3 cameraNormal : CameraNormal;
    float4 position : SV_Position;
};

cbuffer Cbuf
{
    matrix viewMatrix;
}

float3 GetCameraNormal()
{
    return normalize(viewMatrix[2].xyz);
}

VSOut main(float3 position : Position, matrix transform : Transform, float4 color : Color, int id : Id)
{
    VSOut vso;
    //vso.position = float4(position, 1.0f);
    vso.position = mul(float4(position, 1.0f), mul(transform, viewMatrix));
    vso.color = color;
    vso.id = id;
    vso.cameraNormal = GetCameraNormal();
    return vso;
}