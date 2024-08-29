#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_OTHER_LIGHTS 128

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

struct DirectionalLight
{
    matrix viewproj;
    float3 direction;
    float intensity;
    float4 color;
};

cbuffer lightBuffer : register(b1)
{
    float3 ambient_color;
    float ambient_intensity;
    DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
    uint directional_light_count;
};

struct VSIn
{
    float3 position : Position;
    float2 texCoord : TexCoord;
    float3 normal : Normal;
    matrix worldViewProj : WorldViewProj;
    matrix worldMat : WorldProj;
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
    float4 posDirLightSpace[MAX_DIRECTIONAL_LIGHTS] : DirLightProj;
    float4 position : SV_Position;
};

VSOut main(VSIn vin)
{
    VSOut vout;
    vout.viewPos = (float3) mul(float4(vin.position, 1.0f), vin.worldMat);
    vout.position = mul(float4(vin.position, 1.0f), vin.worldViewProj);
    vout.texCoord =  vin.texCoord;
    vout.color = vin.color;
    vout.entityId = vin.entityId;
    vout.sceneIndex = vin.sceneIndex;
    vout.normal = normalize(mul(float4(vin.normal, 0.0f), vin.worldMat)).xyz;
    //vout.normal = mul(float4(vin.normal, 1.0f), vin.worldProj);

    [unroll(MAX_DIRECTIONAL_LIGHTS)]
    for (int i = 0; i < directional_light_count; i++)
    {
        const float4 world = mul(float4(vin.position, 1.0f), vin.worldMat);
        vout.posDirLightSpace[i] = mul(world, directional_lights[i].viewproj);
        //vout.posDirLightSpace[i] = mul(float4(vout.viewPos, 1.0f), directional_lights[i].viewproj);
    }
    
    vout.worldPos = mul(float4(vin.position, 1.0f), vin.worldMat).xyz;
    return vout;
}