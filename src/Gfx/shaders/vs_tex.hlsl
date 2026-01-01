struct InstanceData
{
	matrix model_xform : MODEL_XFORM;
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

struct VSOut
{
    float4 pos : SV_Position;
	float3 normal : NORMAL0;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
    float2 diffuse_map_coordinates : TEXCOORD0;
    float2 normal_map_coordinates : TEXCOORD0;
	float3 light_direction : NORMAL1;
};

cbuffer CameraBuffer : register(b0)
{
	vector camera_position;
	matrix view_proj;
};

cbuffer LightingBuffer : register(b1)
{
	float3 light_position;
};

float4 animated_pos(VSIn input)
{
	float total_weight = 0;

	float4 pos = {0, 0, 0, 1.0f};

	for (uint i = 0; i < input.bone_count; ++i)
	{
		float4 animated_vertex = mul(float4(input.pos, 1.0f), bones[input.bone_indices[i] + input.instance.bone_start]);
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

	float4 pos;

	if (input.bone_count)
	{
		pos = animated_pos(input);
	}
	else
	{
		pos = float4(input.pos, 1.0f);
	}

	output.pos = mul(pos, input.instance.model_xform);

	output.light_direction = light_position - output.pos.xyz;

	output.normal = mul(input.normal, input.instance.model_xform);
	output.tangent = mul(input.tangent, input.instance.model_xform);
	output.bitangent = mul(input.bitangent, input.instance.model_xform);

	output.pos = mul(output.pos, view_proj);
    
	output.diffuse_map_coordinates = input.diffuse_map_coordinates;
	output.normal_map_coordinates = input.normal_map_coordinates;

    return output;
}
