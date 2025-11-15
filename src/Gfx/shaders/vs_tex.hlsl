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

float4 animated_pos(VSIn input)
{
	float total_weight = 0;

	float4 pos = {0, 0, 0, 1.0f};

	for (uint i = 0; i < input.bone_count; ++i)
	{
		float4 animated_vertex = mul(input.pos, bones[input.bone_indices[i]]);
		pos[0] += animated_vertex[0] * input.bone_weights[i];
		pos[1] += animated_vertex[1] * input.bone_weights[i];
		pos[2] += animated_vertex[2] * input.bone_weights[i];
		total_weight += input.bone_weights[i];
	}
	
	float total_inv = 1.0f / total_weight;

	pos[0] *= total_inv;
	pos[1] *= total_inv;
	pos[2] *= total_inv;

	return pos;
}

VSOut main(VSIn input)
{
    VSOut output;

	float4 pos = input.pos;

	if (input.bone_count)
	{
		pos = animated_pos(input);
	}
	else
	{
		pos = input.pos;
	}

	output.pos = mul(pos, input.model_xform);
	output.pos = mul(output.pos, view_proj);

	output.tex_coord = input.tex_coord;
    
    return output;
}
