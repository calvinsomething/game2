struct PSIn
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
};

cbuffer LightingBuffer : register(b1)
{
	float4 light_position_w_ambient_amount;
};

cbuffer MeshBuffer : register(b2)
{
	uint material_index;
};

struct Material
{
	float4 color;
	int diffuse_map_index;
	int normal_map_index;
	float roughness;
};

StructuredBuffer<Material> materials : register(t0);

Texture2D tex_0 : register (t1);
Texture2D tex_1 : register (t2);
Texture2D tex_2 : register (t3);
Texture2D tex_3 : register (t4);

SamplerState samp : register (s0);

float4 main(PSIn input) : SV_Target
{
	float3 normal = normalize(input.normal);
	float3 light_direction = normalize(input.light_direction);

	float4 color;

	switch (materials[material_index].diffuse_map_index)
	{
		case 0:
			color = tex_0.Sample(samp, input.diffuse_map_coordinates);
			break;
		case 1:
			color = tex_1.Sample(samp, input.diffuse_map_coordinates);
			break;
		case 2:
			color = tex_2.Sample(samp, input.diffuse_map_coordinates);
			break;
		case 3:
			color = tex_3.Sample(samp, input.diffuse_map_coordinates);
			break;
		default:
			color = materials[material_index].color;
			break;
	}

	bool has_bump_map = true;
	float3 bump;

	switch (materials[material_index].normal_map_index)
	{
		case 0:
			bump = tex_0.Sample(samp, input.normal_map_coordinates);
			break;
		case 1:
			bump = tex_1.Sample(samp, input.normal_map_coordinates);
			break;
		case 2:
			bump = tex_2.Sample(samp, input.normal_map_coordinates);
			break;
		case 3:
			bump = tex_3.Sample(samp, input.normal_map_coordinates);
			break;
		default:
			has_bump_map = false;
			break;
	}

	if (has_bump_map)
	{
		float3x3 tbn = {
			normalize(input.tangent),
			normalize(input.bitangent),
			normal};

		normal = mul(normalize(bump), tbn);
	}

	float illumination = dot(normal, light_direction) + light_position_w_ambient_amount.w;

	return float4(saturate(color.xyz * illumination), color.w);
}
