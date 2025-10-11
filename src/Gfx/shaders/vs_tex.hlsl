struct VSIn
{
    float4 pos : POSITION;
    float2 tex_coord : TEXCOORD;
	uint4 bone_indices : BONE_INDICES;
	float4 bone_weights : BONE_WEIGHTS;
	uint bone_count : BONE_COUNT;
	matrix model_xform : MODEL_XFORM;
};

StructuredBuffer<matrix> bones : register(t0);

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

	float4 pos = input.pos;

	for (uint i = 0; i < input.bone_count; ++i)
	{
		pos = mul(pos, bones[input.bone_indices[i]]);

		break;
	}

	output.pos = mul(pos, input.model_xform);
	output.pos = mul(output.pos, view_proj);

	output.tex_coord = input.tex_coord;
    
    return output;
}
