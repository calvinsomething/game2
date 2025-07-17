struct VSIn
{
    float4 pos : POSITION;
    float4 color : COLOR;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

cbuffer GlobalBuffer : register(b0)
{
	matrix view_proj;
};

VSOut main(VSIn input)
{
    VSOut output;

	output.pos = mul(input.pos, view_proj);

	output.color = input.color;
    
    return output;
}
