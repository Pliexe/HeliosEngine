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
    uint directional_light_count;
    DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
}

struct PSOut
{
    float4 color : SV_Target0;
    float4 engineSelect : SV_Target1;
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
    //[unroll(MAX_OTHER_LIGHTS)]
    //for (int i = 0; i < entityId; i++)
    //{
	//    
    //}

    PSOut output;
    output.color = tex.Sample(samp, pin.texCoord) * pin.color;

	[unroll(MAX_DIRECTIONAL_LIGHTS)]
    for (int i = 0; i < directional_light_count; i++)
    {
        DirectionalLight light = directional_lights[i];
        // Calculate object shading
        //float4 lightColor = light.color * light.intensity;
        const float3 lightDir = normalize(light.direction - pin.worldPos);
        const float lightIntensity = dot(lightDir, pin.normal);
        output.color.rgb *= saturate(lightIntensity);
    }

    if (pin.color[3] == 0.0f)
        output.engineSelect = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    else
        output.engineSelect = float4(pin.entityId, 0.0f, 0.0f, 0.0f);

    return output;
}