struct PSOut
{
    float4 color : SV_Target0;
    float4 EngineSelect : SV_Target1;
};

PSOut main(float4 color : Color, float data : Id)
{
    PSOut pso;
    pso.color = color;
    pso.EngineSelect = float4(data, 0, 0, 0);
    return pso;
}