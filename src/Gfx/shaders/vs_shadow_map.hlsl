struct InstanceData
{
	matrix world_xform : WORLD_XFORM;
	uint bone_start : BONE_START;
};

struct VSIn
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
	uint4 bone_indices : BONE_INDICES;
	float4 bone_weights : BONE_WEIGHTS;
	uint bone_count : BONE_COUNT;
	InstanceData instance;
};

StructuredBuffer<matrix> bones : register(t0);

struct VSOut
{
    float4 pos : SV_Position;
};

cbuffer LightingBuffer : register(b1)
{
	float3 light_position;
	matrix light_view_proj;
};

float4 animated_data(VSIn input)
{
	float total_weight = 0;

	float4 position = {0.0f, 0.0f, 0.0f, 1.0f};

	for (uint i = 0; i < input.bone_count; ++i)
	{
		matrix bone = bones[input.bone_indices[i] + input.instance.bone_start];

		float3 animated_vertex = mul(float4(input.pos, 1.0f), bone);
		position += float4(animated_vertex * input.bone_weights[i], 0.0f);

		total_weight += input.bone_weights[i];
	}
	
	float total_inv = 1.0f / total_weight;

	position.x *= total_inv;
	position.y *= total_inv;
	position.z *= total_inv;

	return position;
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
