Texture2D<float4> tex : register(t0);

sampler samp : register(s0);

struct PSOut
{
    float4 color : SV_Target0;
    float4 engineSelect : SV_Target1;
};

PSOut main(float4 color : Color, float2 texCoord : TexCoord, float entityId : EntityId)
{
    PSOut output;
    output.color = tex.Sample(samp, texCoord) * color;
    if(color[3] == 0.0f)
        output.engineSelect = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    else
        output.engineSelect = float4(entityId, 0.0f, 0.0f, 0.0f);

    return output;
}