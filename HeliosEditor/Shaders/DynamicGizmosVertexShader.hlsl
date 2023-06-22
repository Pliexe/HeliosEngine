cbuffer CBuf
{
    matrix view_matrix;
}

struct VSOut
{
    float4 color : Color;
    int id : Id;
    float4 position : SV_Position;
};

VSOut main( float3 pos : Position, float4 col : Color, int id : Id)
{
    VSOut vout;
    vout.position = mul(float4(pos, 1.0f), view_matrix);
    vout.color = col;
    vout.id = id;
	return vout;
}