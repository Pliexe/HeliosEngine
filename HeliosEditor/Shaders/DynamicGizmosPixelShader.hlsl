struct PSOut
{
    float4 color : SV_Target0;
    float4 id : SV_Target1;
};

PSOut main(float4 color : Color, float id : Id)
{
    PSOut pout;
    pout.color = color;
    pout.id = float4(id, 0.0f, 0.0f, 0.0f);
    return pout;
}