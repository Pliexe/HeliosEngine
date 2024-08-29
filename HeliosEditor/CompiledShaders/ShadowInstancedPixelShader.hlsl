Texture2D<float4> tex : register(t0);

sampler samp : register(s0);

struct PSIn
{
    float4 color : Color;
    float2 texCoord : TexCoord;
    float4 position : SV_Position;
};

float main(PSIn pin) : SV_DEPTH
{
    float4 color = tex.Sample(samp, pin.texCoord) * pin.color;

    clip(0.1f <= color.a ? 1 : 0);
    
    return pin.position.z / pin.position.w;
}