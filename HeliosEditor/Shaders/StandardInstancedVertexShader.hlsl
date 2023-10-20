Texture2D<float4> tex : register(t25);

cbuffer Cbuf : register(b0)
{
    //matrix worldViewPortProjection;
    //matrix worldProjection;
    //float4 color;
    //int entityIdlow;
    //int entityIdhigh;
    //int sceneIndex;
};

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
    float3 viewPos : Position;
    float4 color : Color;
    float2 texCoord : TexCoord;
    int entityId : EntityId;
    int sceneIndex : SceneIndex;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float4 position : SV_Position;
};

VSOut main(VSIn vin)
{
    VSOut vout;
    vout.viewPos = (float3) mul(float4(vin.position, 1.0f), vin.worldProj);
    vout.position = mul(float4(vin.position, 1.0f), vin.worldViewProj);
    vout.texCoord =  vin.texCoord;
    vout.color = vin.color;
    vout.entityId = vin.entityId;
    vout.sceneIndex = vin.sceneIndex;
    vout.normal = normalize(mul(float4(vin.normal, 0.0f), vin.worldProj)).xyz;
    //vout.normal = mul(float4(vin.normal, 1.0f), vin.worldProj);
    vout.worldPos = mul(float4(vin.position, 1.0f), vin.worldProj).xyz;
    return vout;
}