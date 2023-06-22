#include "LineHeader.hlsli"
#define PI 3.1415926535897932384626433832795f
#define SEGMENTS 8

struct VSIn
{
    float4 col : Color;
    float thickness : Thickness;
    int mode : Mode;
    int id : Id;
    float4 pos : SV_Position;
};

struct GSOutput
{
    float4 col : Color;
    float3 progress : Progress;
    float thickness : Thickness;
    int mode : Mode;
    int id : Id;
    float4 pos : SV_Position;
};

void AddHalfCircle(inout TriangleStream<GSOutput> triangle_stream, VSIn pt, float angle)
{
    // Create a half circle starting at the given angle and ending at the given angle + PI
    // The circle is centered at the given point and has the given thickness
    // The circle is divided into SEGMENTS segments
    // The half circle consists of SEGMENTS triangles

    float angleStep = PI / SEGMENTS;
    float r = pt.thickness * 0.5f;

    angle -= angleStep;

    float2 center = pt.pos.xy;

    float2 p0 = center + r * float2(cos(angle), sin(angle));
    float2 p1 = center + r * float2(cos(angle + angleStep), sin(angle + angleStep));

    for (int i = 0; i <= SEGMENTS; i++)
    {
        GSOutput g0;
        g0.progress = float3(0.0f, 0.0f, 0.0f);
        g0.mode = pt.mode;
        g0.col = pt.col;
        g0.id = pt.id;
        g0.thickness = pt.thickness;
        g0.pos = float4(p0, pt.pos.zw);

        GSOutput g1 = g0;
        g1.pos = float4(p1, pt.pos.zw);

        GSOutput g2 = g0;
        g2.pos = float4(center, pt.pos.zw);

        triangle_stream.Append(g0);
        triangle_stream.Append(g1);
        triangle_stream.Append(g2);

        p0 = p1;
        p1 = center + r * float2(cos(angle + angleStep * (i + 2)), sin(angle + angleStep * (i + 2)));
    }

    triangle_stream.RestartStrip();
}

[maxvertexcount(4 + 8 * 4 * 2 + 1)]
void main(
	line VSIn input[2], 
	inout TriangleStream< GSOutput > output
)
{
    VSIn p0 = input[0], p1 = input[1];

    float wAv = max((p0.pos.w + p1.pos.w) * 0.5f, 0.1f);
    p0.thickness *= wAv * 0.01f;
    p1.thickness *= wAv * 0.01f;

    float2 a = p0.pos;
    float2 b = p1.pos;
    float2 c = normalize(float2(a.y - b.y, b.x - a.x));

    //switch (p0.mode)
    //{
    //case ROUNDED_DASHED_LINE:
    //case ROUNDED_DASH_DOT_DOT_LINE:
    //case ROUNDED_DASH_DOT_LINE:
    //case ROUNDED_DOTTED_LINE:
    //case ROUNDED_SOLID_LINE:
	//    AddHalfCircle(output, p0, atan2(c.y, c.x));
	//    AddHalfCircle(output, p1, atan2(c.y, c.x) + PI);
    //    break;
    //default:
    //    break;
    //}

    switch (p0.mode)
    {
    case ROUNDED_DASHED_LINE:
    case ROUNDED_DASH_DOT_DOT_LINE:
    case ROUNDED_DASH_DOT_LINE:
    case ROUNDED_DOTTED_LINE:
    case ROUNDED_SOLID_LINE:
        // Extend the line by half the thickness of the line
        // This is necessary for half circles to be drawn correctly or other way to say it adding the offset of the half circle
        float3 dir = normalize(p1.pos.xyz - p0.pos.xyz);
        p0.pos.xyz -= dir * p0.thickness * 0.5f;
        p1.pos.xyz += dir * p1.thickness * 0.5f;
        break;
    default:
        break;
    }
    
    GSOutput defp0;
    defp0.progress = float3(0.0f, 0.0f, distance(p0.pos, p1.pos) / max(p0.thickness, p1.thickness));
    defp0.mode = p0.mode;
    defp0.col = p0.col;
    defp0.id = p0.id;
    defp0.thickness = p0.thickness;

    GSOutput defp1 = defp0;
    defp1.progress.xy = float2(defp1.progress.z, 0.0f);
    defp1.col = p1.col;
    defp1.id = p1.id;
    
    GSOutput g0 = defp0;
    g0.pos = float4(p0.pos.xy - c * p0.thickness * 0.5f, p0.pos.zw);
    g0.progress.y = -1.0f;
    GSOutput g1 = defp0;
    g1.pos = float4(p0.pos.xy + c * p0.thickness * 0.5f, p0.pos.zw);
    g1.progress.y = 1.0f;
    GSOutput g2 = defp1;
    g2.progress.y = -1.0f;
    g2.pos = float4(p1.pos.xy - c * p1.thickness * 0.5f, p1.pos.zw);
    GSOutput g3 = defp1;
    g3.progress.y = 1.0f;
    g3.pos = float4(p1.pos.xy + c * p1.thickness * 0.5f, p1.pos.zw);

    output.Append(g0);
    output.Append(g1);
    output.Append(g2);
    output.Append(g3);

    output.RestartStrip();
}