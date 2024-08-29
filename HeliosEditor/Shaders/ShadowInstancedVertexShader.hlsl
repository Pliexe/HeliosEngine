struct VSIn
{
    float3 position : Position;
    float2 texCoord : TexCoord;
    float3 normal : Normal;
    matrix worldViewProj : WorldViewProj;
    matrix worldProj : WorldProj;
    float4 color : Color;
    int entityId : EntityId;
    int sceneIndex : SceneIndex;
};

struct VSOut
{
    float4 color : Color;
    float2 texCoord : TexCoord;
    float4 position : SV_Position;
};

VSOut main(VSIn vin)
{
    VSOut vout;
    vout.position = mul(float4(vin.position, 1.0f), vin.worldViewProj);
    //vout.position = mul(float4(vin.position, 1.0f), vin.worldViewProj);
    vout.texCoord = vin.texCoord;
    vout.color = vin.color;
    return vout;
}