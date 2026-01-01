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
	float3 albedo_color;
	float3 emissive_color;
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

void load_color(int map_index, float2 uv, out float4 color)
{
	switch (map_index)
	{
		case 0:
			color = tex_0.Sample(samp, uv);
			break;
		case 1:
			color = tex_1.Sample(samp, uv);
			break;
		case 2:
			color = tex_2.Sample(samp, uv);
			break;
		case 3:
			color = tex_3.Sample(samp, uv);
			break;
		default:
			color = float4(materials[material_index].albedo_color, 1.0f);
			break;
	}
}

bool load_bump(int map_index, float2 uv, out float3 bump)
{
	switch (map_index)
	{
		case 0:
			bump = tex_0.Sample(samp, uv);
			return true;
		case 1:
			bump = tex_1.Sample(samp, uv);
			return true;
		case 2:
			bump = tex_2.Sample(samp, uv);
			return true;
		case 3:
			bump = tex_3.Sample(samp, uv);
			return true;
	}

	return false;
}

float4 main(PSIn input) : SV_Target
{
	float3 normal = normalize(input.normal);
	float3 light_direction = normalize(input.light_direction);

	float4 color;
	load_color(materials[material_index].diffuse_map_index, input.diffuse_map_coordinates, color);

	float3 bump;
	if (load_bump(materials[material_index].normal_map_index, input.normal_map_coordinates, bump))
	{
		float3x3 tbn = {
			normalize(input.tangent),
			normalize(input.bitangent),
			normal};

		normal = mul(normalize(bump), tbn);
	}

	// if (dot(normal, light_direction) > 0.9f)
	// {
	// 	return float4(0.0f,1.0f,0.0f,color.w);
	// }

	float illumination = max(0, dot(normal, light_direction)) + light_position_w_ambient_amount.w;

	return float4(saturate(color.xyz * illumination), color.w);
}
