struct PSOut
{
    float4 color : SV_Target0;
    int4 id : SV_Target1;
};

PSOut main(float4 color : Color, int id : Id)
{
    PSOut pout;
    pout.color = color;
    pout.id = int4(0.0f, 0.0f, id, 0.0f);
    return pout;
}