#include "LineHeader.hlsli"

struct GSOutput
{
    float4 col : Color;
    float3 progress : Progress;
    float thickness : Thickness;
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

    // Check mode, Use discard; to accomidate for the different modes

    int i = floor(pin.progress.x);
    switch (pin.mode)
    {
        case 0:
            break;
        case DASHED_LINE:
            if ((i + 1) % 3 == 0)
                discard;
            break;
        case DOTTED_LINE:
            if (i % 2)
                discard;
            break;
        case DASH_DOT_LINE:
            if ((i + 5) % 5 && !(((i + 7) % 5) == 0 || ((i + 8) % 5) == 0))
                discard;
            break;
        case DASH_DOT_DOT_LINE:
            int diff = i % 7;
            if (diff == 2 || diff == 4 || diff == 6)
                discard;
            break;
        case ROUNDED_DASHED_LINE:
	    {
		    // Each dash is 2 units long and are rounded at the ends
        
        // Draw half circle at the start of the line if current drawing position is inside the circle
        // Check the distance from the center of the circle to the current drawing position
        // If the distance is less than the radius of the circle, draw the circle
        
                if ((i + 1) % 3 == 0)
                    discard;
        // If out of range, discard
                if (pin.progress.z < (i + (i + 1) % 3 * -1 + 3))
                    discard;

        // Enters if i is not a multiple of 3 > 0 % 3 = 0 so it wont trigger, only 1 and 2 will trigger
                float x = (pin.progress.x + 1.0f) % 3.0f - 2.0f; // returns -1 to 1
                float distance = (x * x);

        //distance += pin.thicknessProgress > 0 ? pin.thicknessProgress : pin.thicknessProgress * -1;
                distance += pin.progress.y * pin.progress.y * 0.5f;
                if (distance > 1.0f)
                    discard;
            
                break;
            }
        case ROUNDED_DOTTED_LINE:
            if (i % 2)
                discard;

            if (pin.progress.z < (i + 1))
                discard;

            float x = (pin.progress.x % 2.0f) * 2.0f - 1.0f; // returns -1 to 1
            float distance = (x * x) + pin.progress.y * pin.progress.y;
        
            if (distance > 1.0f)
                discard;
            break;
        case ROUNDED_SOLID_LINE:
    {
                float x = min(pin.progress.x, pin.progress.z - pin.progress.x);
                float y = (pin.progress.y * pin.progress.y);
                float d = sin(x) + cos(y) * 0.5f;

                if (d < 0.5f && x <= 0.5f)
                    discard;
                break;
            }
        case ROUNDED_DASH_DOT_LINE:
    {
                
		        if (i % 5 == 0)
		        {
                    if (pin.progress.z < (i + 1))
                        discard;

                    float x = (pin.progress.x % 5.0f) * 2.0f - 1.0f; // returns -1 to 1
                    float distance = (x * x) + pin.progress.y * pin.progress.y;

                    if (distance > 1.0f)
                        discard;
                }
                else if ((i + 5) % 5 && !(((i + 7) % 5) == 0 || ((i + 8) % 5) == 0))
                    discard;
                else
                {
        // If out of range, discard
                    //if ((pin.progress.z + 2.0f) < (((i % 5)) + (((i % 5)) + 1) % 3 * -1 + 3))
                    //    discard; // doesnt work

        // Enters if i is not a multiple of 3 > 0 % 3 = 0 so it wont trigger, only 1 and 2 will trigger
                    float x = ((pin.progress.x % 5.0f) + 2.0f) % 3.0f - 2.0f; // returns -1 to 1
                    float distance = (x * x);

        //distance += pin.thicknessProgress > 0 ? pin.thicknessProgress : pin.thicknessProgress * -1;
                    distance += pin.progress.y * pin.progress.y * 0.5f;
                    if (distance > 1.0f)
                        discard;
                }

                break;
            }
            case ROUNDED_DASH_DOT_DOT_LINE:
            {
                if (i % 7 == 3 || i % 7 == 5)
                {
                    if (pin.progress.z < (i + 1))
                        discard;

                    float x = ((pin.progress.x % 7 + 3.0f) % 2.0f) * 2.0f - 1.0f; // returns -1 to 1
                    float distance = (x * x) + pin.progress.y * pin.progress.y;

                    if (distance > 1.0f)
                        discard;
                }
                else if (i % 7 == 2 || i % 7 == 4 || i % 7 == 6)
                    discard;
                else
                {
                    float x = ((pin.progress.x % 7.0f) + 1.0f) - 2.0f; // returns -1 to 1
                    float distance = (x * x);
                
                    distance += pin.progress.y * pin.progress.y * 0.5f;
                    if (distance > 1.0f)
                        discard;
                }

                break;
            }
    }

    pout.id = float4(pin.id, 0.0f, 0.0f, 0.0f);

    return pout;
}