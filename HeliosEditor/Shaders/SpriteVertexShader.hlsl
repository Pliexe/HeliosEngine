struct VSOut
{
    float4 color : Color;
    uint id : TextureID;
    float2 texCoord : TexCoord;
    float4 position : SV_Position;
    float entityId : EntityId;
};

cbuffer Cbuf
{
    matrix viewMatrix;
};


VSOut main(float2 pos : Position, float2 texCoord : TexCoord, matrix world : World, float4 color: Color, uint id : TextureID, uint entityId : EntityId)
{
    VSOut vso;
    //vso.position = mul(float4(pos.x, pos.y, 0.0f, 1.0f), viewMatrix);
    vso.position = mul(float4(pos.x, pos.y, 0.0f, 1.0f), mul(world, viewMatrix));
    vso.color = color;
    vso.id = id;
    vso.texCoord = texCoord;
    vso.entityId = entityId;
    return vso;
}