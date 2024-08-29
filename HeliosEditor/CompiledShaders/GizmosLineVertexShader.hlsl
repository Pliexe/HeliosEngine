struct VSOut
{
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position )
{
    VSOut vsout = { float4(pos, 1.0f) };
    return vsout;
}