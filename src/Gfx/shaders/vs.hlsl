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

VSOut main(VSIn input)
{
    VSOut output;

	output.pos = input.pos;
	output.color = input.color;
    
    return output;
}
