struct PSIn
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

StructuredBuffer<Material> materials : register(t1);

Texture2D tex_0 : register(t2);
Texture2D tex_1 : register(t3);
Texture2D tex_2 : register(t4);
Texture2D tex_3 : register(t5);

SamplerState samp : register(s0);

float4 load_color(int map_index, float2 uv)
{
	switch (map_index)
	{
		case 0:
			return tex_0.Sample(samp, uv);
		case 1:
			return tex_1.Sample(samp, uv);
		case 2:
			return tex_2.Sample(samp, uv);
		case 3:
			return tex_3.Sample(samp, uv);
		default:
			return float4(materials[material_index].albedo_color * 2.0f + float3(0.2f, 0.2f, 0.2f), 1.0f);
	}
}

bool load_bump(int map_index, float2 uv, out float3 bump)
{
	float3 sampled = {0,0,0};
	switch (map_index)
	{
		case 0:
			sampled = tex_0.Sample(samp, uv);
			break;
		case 1:
			sampled = tex_1.Sample(samp, uv);
			break;
		case 2:
			sampled = tex_2.Sample(samp, uv);
			break;
		case 3:
			sampled = tex_3.Sample(samp, uv);
			break;
		default:
			return false;
	}

	bump = sampled * 2 - 1.0f;

	return true;
}

float4 main(PSIn input, bool is_front_face : SV_IsFrontFace) : SV_Target
{
	float4 color = load_color(materials[material_index].diffuse_map_index, input.diffuse_map_coordinates);

	// fully transparent values can be discarded
	// translucent values should be drawn in a seperate pass
	// with a depth stencil state that allows blending with more forward pixels
	if (color.w < 0.01)
	{
		discard;
	}

	float3 normal = normalize(input.normal);
	if (!is_front_face)
	{
		normal = -normal;
	}

	float3 bump;
	if (load_bump(materials[material_index].normal_map_index, input.normal_map_coordinates, bump))
	{
		float3x3 tbn = {
			normalize(input.tangent),
			normalize(input.bitangent),
			normal
		};

		normal = normalize(mul(bump, tbn));
	}

	float3 light_dir = normalize(light_position_w_ambient_amount.xyz - input.world_position);

	float illumination = max(0, 0.3f + dot(normal, light_dir)) + light_position_w_ambient_amount.w;

	float3 light_reflection = reflect(-light_dir, normal) * (1.0f - materials[material_index].roughness);

	float3 camera_dir = normalize(camera_position.xyz - input.world_position);

	float highlight = pow(max(0, dot(light_reflection, camera_dir)), 5) * (illumination > 0.0f);

	return float4(saturate(color.xyz * illumination + highlight), color.w);
}
