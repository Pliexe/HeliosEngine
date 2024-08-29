#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_OTHER_LIGHTS 128

Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMaps[MAX_DIRECTIONAL_LIGHTS] : register(t5);

sampler samp : register(s0);
SamplerComparisonState depthSamp : register(s1);

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

struct PSOut
{
    float4 color : SV_Target0;
    int4 engine_select : SV_Target1;
};

struct PSIn
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

float ShadowCalc(float4 posLightSpace, Texture2D depthTex)
{
    float3 projCoords = posLightSpace.xyz / posLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;
    float currentDepth = projCoords.z;
    float closestDepth = depthTex.Sample(samp, projCoords.xy).r;
    float bias = 0.0005f;
    float tmp = currentDepth - bias;
    float test2 = tmp > closestDepth ? 1.0f : 0.0f;
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}

PSOut main(PSIn pin)
{
    PSOut output;
    output.color = tex.Sample(samp, pin.texCoord) * pin.color;

    clip(0.1f <= output.color.a ? 1 : 0);
    //if(dot(pin.normal, pin.fragPos) >= 0.0f)
    //{
    //    pin.normal = -pin.normal;
    //}

    float3 ambient_col = ambient_color * (ambient_intensity * 1.0f);
    float3 lighting = ambient_col;
    
	[unroll(MAX_DIRECTIONAL_LIGHTS)]
    for (int i = 0; i < directional_light_count; i++)
    {
        DirectionalLight light = directional_lights[i];
        
        float nDotL = dot(light.direction, pin.normal);
        
        if (nDotL > 0.0f)
        {
            // diffuse
                // float diffuse = saturate(nDotL) * light.color.xyz * light.intensity;
                float3 diffuse = saturate(nDotL) * light.color.xyz * light.intensity;
                
                float3 diffuseColor = diffuse * light.color.xyz;
            // shadow
                float shadow = ShadowCalc(pin.posDirLightSpace[i], shadowMaps[i]);

                lighting += (1.0f - shadow) * diffuseColor;
            //lighting = max(lighting, 0.0f);
        }
    }
    
    output.color.rgb *= saturate(lighting);

    if (pin.color[3] == 0.0f)
        output.engine_select = int4(0, 0, -1, 0);
    else
        output.engine_select = int4(pin.entityId, 0, pin.sceneIndex, 0);

    return output;
}