struct VSIn
{
    float4 pos : POSITION;
    float2 tex_coord : TEXCOORD;
	matrix model_xform : MODEL_XFORM;
};

struct VSOut
{
    float4 pos : SV_Position;
    float2 tex_coord : TEXCOORD;
};

cbuffer GlobalBuffer : register(b0)
{
	matrix view_proj;
};

VSOut main(VSIn input)
{
    VSOut output;

	output.pos = mul(input.pos, input.model_xform);
	output.pos = mul(output.pos, view_proj);

	output.tex_coord = input.tex_coord;
    
    return output;
}
