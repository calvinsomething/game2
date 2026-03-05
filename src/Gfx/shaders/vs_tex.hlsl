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
	float3 world_position : POSITION;
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

struct AnimatedData
{
	float4 position;
	float3 normal;
	float3 tangent;
	float3 bitangent;
};

AnimatedData animated_data(VSIn input)
{
	float total_weight = 0;

	AnimatedData output = {
		{0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};

	for (uint i = 0; i < input.bone_count; ++i)
	{
		matrix bone = bones[input.bone_indices[i] + input.instance.bone_start];

		float3 animated_vertex = mul(float4(input.pos, 1.0f), bone);
		output.position += float4(animated_vertex * input.bone_weights[i], 0.0f);

		float3 animated_normal = mul(input.normal, bone);
		output.normal += animated_normal;

		float3 animated_tangent = mul(input.tangent, bone);
		output.tangent += animated_tangent;

		float3 animated_bitangent = mul(input.bitangent, bone);
		output.bitangent += animated_bitangent;

		total_weight += input.bone_weights[i];
	}
	
	float total_inv = 1.0f / total_weight;

	output.position.x *= total_inv;
	output.position.y *= total_inv;
	output.position.z *= total_inv;

	output.normal *= total_inv;

	output.tangent *= total_inv;

	output.bitangent *= total_inv;

	return output;
}

VSOut main(VSIn input)
{
    VSOut output;

	float4 position;
	float3 normal;
	float3 tangent;
	float3 bitangent;

	if (input.bone_count)
	{
		AnimatedData ad = animated_data(input);

		position = ad.position;
		normal = ad.normal;
		tangent = ad.tangent;
		bitangent = ad.bitangent;
	}
	else
	{
		position = float4(input.pos, 1.0f);

		normal = input.normal;
		tangent = input.tangent;
		bitangent = input.bitangent;
	}

	position = mul(position, input.instance.model_xform);
	output.normal = mul(normal, input.instance.model_xform);
	output.tangent = mul(tangent, input.instance.model_xform);
	output.bitangent = mul(bitangent, input.instance.model_xform);

	output.world_position = position;
	output.pos = mul(position, view_proj);
    
	output.diffuse_map_coordinates = input.diffuse_map_coordinates;
	output.normal_map_coordinates = input.normal_map_coordinates;

    return output;
}
