#include "directional_light.hlsl"

struct InstanceData
{
	matrix world_xform : WORLD_XFORM;
	uint bone_start : BONE_START;
};

struct VSIn
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 diffuse_map_coordinates : TEXCOORD;
    float2 normal_map_coordinates : TEXCOORD;
	uint4 bone_indices : BONE_INDICES;
	float4 bone_weights : BONE_WEIGHTS;
	uint bone_count : BONE_COUNT;
	InstanceData instance;
};

StructuredBuffer<matrix> bones : register(t0);

float4 animated_data(VSIn input)
{
	float total_weight = 0;

	float3 position = {0.0f, 0.0f, 0.0f};

	for (uint i = 0; i < input.bone_count; ++i)
	{
		matrix bone = bones[input.bone_indices[i] + input.instance.bone_start];

		float3 animated_vertex = mul(float4(input.pos, 1.0f), bone);
		position += animated_vertex * input.bone_weights[i];

		total_weight += input.bone_weights[i];
	}
	
	float total_inv = 1.0f / total_weight;

	position *= total_inv;

	return float4(position, 1.0f);
}

float4 main(VSIn input) : SV_Position
{
	float4 position;

	if (input.bone_count)
	{
		position = animated_data(input);
	}
	else
	{
		position = float4(input.pos, 1.0f);
	}

	position = mul(position, input.instance.world_xform);
	position = mul(position, light_view_proj);

    return position;
}

