cbuffer Cbuf
{
    matrix transform;
    float4 color;
};

struct VSOut
{
    float4 color : Color;
    float2 texCoord : TexCoord;
    float4 position : SV_Position;
};

VSOut main( float3 pos : Position, float2 texCoord : TexCoord )
{
    VSOut vout;
    
    vout.position = mul( float4( pos, 1.0f ), transform );
    vout.texCoord = texCoord;
    vout.color = color;
    
    return vout;
}