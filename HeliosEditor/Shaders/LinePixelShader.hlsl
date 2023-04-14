struct GSOutput
{
    float4 col : Color;
    float progress : Progress;
    int mode : Mode;
    float id : Id;
    float4 pos : SV_Position;
};

struct PSOut
{
    float4 color : SV_Target0;
    float4 id : SV_Target1;
};

PSOut main(GSOutput pin)
{
    PSOut pout;


    pout.color = pin.col;

    clip(0.1f <= pout.color.a ? 1 : 0);

    switch (pin.mode)
    {
        case 1:
            if (floor(pin.progress) % 2)
                discard;
            break;
        default:
            break;
    }

    pout.id = float4(pin.id, 0.0f, 0.0f, 0.0f);

    return pout;
}