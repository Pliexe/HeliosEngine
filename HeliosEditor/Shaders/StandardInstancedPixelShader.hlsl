#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_OTHER_LIGHTS 128

Texture2D<float4> tex : register(t0);

sampler samp : register(s0);

struct DirectionalLight
{
    float3 direction;
    float intensity;
    float4 color;
};

cbuffer lightBuffer : register(b0)
{
    float3 ambient_color;
    float ambient_intensity;
    DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
    uint directional_light_count;
};

struct PSOut
{
    float4 color : SV_Target0;
    int4 engine_select : SV_Target1;
};

struct PSIn
{
    float3 fragPos : Position;
    float4 color : Color;
    float2 texCoord : TexCoord;
    int entityId : EntityId;
    int sceneIndex : SceneIndex;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
    float4 position : SV_Position;
};

PSOut main(PSIn pin)
{
    PSOut output;
    output.color = tex.Sample(samp, pin.texCoord) * pin.color;

    clip(0.1f <= output.color.a ? 1 : 0);
    if(dot(pin.normal, pin.fragPos) >= 0.0f)
    {
        pin.normal = -pin.normal;
    }

    float3 lighting = ambient_color * (ambient_intensity * 1.0f);
    
	[unroll(MAX_DIRECTIONAL_LIGHTS)]
    for (int i = 0; i < directional_light_count; i++)
    {
        DirectionalLight light = directional_lights[i];
        float light_intensity = saturate(dot(light.direction, pin.normal));
        lighting += light_intensity * light.intensity * light.color;
    }
    
    output.color.rgb *= saturate(lighting);

    if (pin.color[3] == 0.0f)
        output.engine_select = int4(0, 0, -1, 0);
    else
        output.engine_select = int4(pin.entityId, 0, pin.sceneIndex, 0);

    return output;
}