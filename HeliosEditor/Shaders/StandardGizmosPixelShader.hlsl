struct PSOut
{
    float4 color : SV_Target0;
    int4 EngineSelect : SV_Target1;
};

PSOut main(float4 color : Color, int data : Id)
{
    PSOut pso;
    pso.color = color;
    pso.EngineSelect = int4(0, 0, data, 0);
    return pso;
}