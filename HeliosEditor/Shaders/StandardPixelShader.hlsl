Texture2D<float4> tex : register(t0);

sampler samp : register(s0);

float4 main(float4 color : Color, float2 texCoord : TexCoord) : SV_TARGET
{
    return tex.Sample(samp, texCoord) * color;
}