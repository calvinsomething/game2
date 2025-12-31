struct PSIn
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float2 diffuse_map_coordinates : TEXCOORD;
    float2 normal_map_coordinates : TEXCOORD;
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

	// TODO transform sampled normal from tangent space to world space
	switch (materials[material_index].normal_map_index)
	{
		case 0:
			// normal = tex_0.Sample(samp, input.normal_map_coordinates);
			break;
		case 1:
			// normal = tex_1.Sample(samp, input.normal_map_coordinates);
			break;
		case 2:
			// normal = tex_2.Sample(samp, input.normal_map_coordinates);
			break;
		case 3:
			// normal = tex_3.Sample(samp, input.normal_map_coordinates);
			break;
	}

	float illumination = dot(normal, light_direction) + light_position_w_ambient_amount.w;

	return float4(color.xyz * illumination, color.w);
}
