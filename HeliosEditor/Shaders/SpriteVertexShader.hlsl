struct VSOut
{
    float4 color : Color;
    uint instanceID : SV_InstanceID;
    float4 position : SV_Position;
};

cbuffer Cbuf
{
    matrix viewMatrix;
};


VSOut main(float2 pos : Position, float4 color : Color, matrix world : World, uint instanceID : SV_InstanceID)
{
    VSOut vso;
    vso.position = mul(float4(pos.x, pos.y, 0.0f, 1.0f), mul(world, viewMatrix));
    //vso.position = float4(pos.x, pos.y, 0.0f, 1.0f);
    vso.color = color;
    vso.instanceID = instanceID;
    return vso;
}