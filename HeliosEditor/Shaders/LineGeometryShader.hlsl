cbuffer Cbuf
{
    matrix camera_angle;
}

struct VSIn
{
    float4 col : Color;
    float width : Width;
    int mode : Mode;
    float id : Id;
    float4 pos : SV_Position;
};

struct GSOutput
{
    float4 col : Color;
    float progress : Progress;
    int mode : Mode;
    float id : Id;
    float4 pos : SV_Position;
};

//[maxvertexcount(2)]
//void main(
//	line VSIn input[2],
//	inout LineStream<GSOutput> output
//)
//{
//    for (int i = 0; i < 2; i++)
//    {
//        GSOutput element;
//        element.progress = 0.0f;
//        element.mode = input[i].mode;
//        element.col = input[i].col;
//        element.id = input[i].id;
//        element.pos = input[i].pos;
//        output.Append(element);
//    }
//}

[maxvertexcount(4)]
void main(
	line VSIn input[2], 
	inout TriangleStream< GSOutput > output
)
{
    VSIn p0 = input[0], p1 = input[1];
    
    // Adjust width scale
    p0.width /= 100.0f;
    p1.width /= 100.0f;
    
	p0.width *= p0.pos.w;
    p1.width *= p1.pos.w;

    float2 a = p0.pos.xy;
    float2 b = p1.pos.xy;
    float2 c1 = normalize(float2(a.y - b.y, b.x - a.x)) * p0.width;
    float2 c2 = normalize(float2(a.y - b.y, b.x - a.x)) * p1.width;
    
    GSOutput defp0;
    defp0.progress = 0.0f;
    defp0.mode = p0.mode;
    defp0.col = p0.col;
    defp0.id = p0.id;

    GSOutput defp1 = defp0;
    defp1.progress = distance(p0.pos, p1.pos) / max(p0.width * 2.0f, p1.width * 2.0f);
    defp1.mode = p1.mode;
    defp1.col = p1.col;
    defp1.id = p1.id;

    GSOutput g0 = defp0;
    g0.pos = float4(p0.pos.xy - c1, p0.pos.zw);
    GSOutput g1 = defp0;
    g1.progress = max(p0.width * 2.0f, p1.width * 2.0f);
    g1.pos = float4(p0.pos.xy + c1, p0.pos.zw);
    GSOutput g2 = defp1;          
    g2.pos = float4(p1.pos.xy - c2, p1.pos.zw);
    GSOutput g3 = defp1;
    g3.progress = distance(p0.pos, p1.pos) / max(p0.width * 2.0f, p1.width * 2.0f) + g1.progress;
    g3.pos = float4(p1.pos.xy + c2, p1.pos.zw);

    output.Append(g0);
    output.Append(g1);
    output.Append(g2);
    output.Append(g3);

    output.RestartStrip();
}