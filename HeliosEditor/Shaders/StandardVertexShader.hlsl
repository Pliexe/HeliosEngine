Texture2D<float4> tex : register(t25);

cbuffer Cbuf
{
    matrix transform;
    float4 color;
    float entityId;
};

struct VSOut
{
    float4 color : Color;
    float2 texCoord : TexCoord;
    float entityId : EntityId;
    float4 position : SV_Position;
};

VSOut main( float3 pos : Position, float2 texCoord : TexCoord )
{
    VSOut vout;
    vout.position = mul( float4(pos, 1.0f), transform );
    vout.texCoord = texCoord;
    vout.color = color;
    vout.entityId = entityId;
    return vout;
}