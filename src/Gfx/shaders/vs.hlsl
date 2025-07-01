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

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	matrix model;
};

VSOut main(VSIn input)
{
    VSOut output;

	output.pos = mul(input.pos, model);

	output.color = input.color;
    
    return output;
}
