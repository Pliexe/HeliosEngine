struct VSOut
{
    float4 color : Color;
    uint id : TextureID;
    float2 texCoord : TexCoord;
    float4 position : SV_Position;
};

cbuffer Cbuf
{
    matrix viewMatrix;
};


VSOut main(float2 pos : Position, float2 texCoord : TexCoord, uint id : TextureID, float4 color : Color, matrix world : World)
{
    VSOut vso;
    vso.position = mul(float4(pos.x, pos.y, 0.0f, 1.0f), mul(world, viewMatrix));
    vso.color = color;
    vso.id = id;
    vso.texCoord = texCoord;
    return vso;
}