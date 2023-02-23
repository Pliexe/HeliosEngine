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
    float4 engine_select : SV_Target1;
};

struct PSIn
{
    float4 color : Color;
    float2 texCoord : TexCoord;
    float entityId : EntityId;
    float3 normal : Normal;
    float3 worldPos : WorldPos;
};

PSOut main(PSIn pin)
{
    PSOut output;
    output.color = tex.Sample(samp, pin.texCoord) * pin.color;

    float3 lighting = ambient_color * (ambient_intensity * 0.1f);
    
	[unroll(MAX_DIRECTIONAL_LIGHTS)]
    for (int i = 0; i < directional_light_count; i++)
    {
        DirectionalLight light = directional_lights[i];
        float light_intensity = saturate(dot(light.direction, pin.normal));
        lighting += light_intensity * light.intensity * light.color;
    }
    
    output.color.rgb *= saturate(lighting);

    if (pin.color[3] == 0.0f)
        output.engine_select = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    else
        output.engine_select = float4(pin.entityId, 0.0f, 0.0f, 0.0f);

    return output;
}