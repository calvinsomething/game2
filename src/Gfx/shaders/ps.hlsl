struct PSIn
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
	float3 world_position : POSITION;
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

float4 main(PSIn input) : SV_Target
{
	float3 normal = normalize(input.normal);

	float3 light_dir = normalize(light_position_w_ambient_amount.xyz - input.world_position);

	float illumination = max(0, 0.3f + dot(normal, normalize(light_dir)) * 0.7f) + light_position_w_ambient_amount.w;

	float3 color = (materials[material_index].albedo_color + float3(0.1f, 0.1f, 0.1f)) * illumination * 2.0f;

	float3 light_reflection = reflect(-light_dir, normal) * (1.0f - materials[material_index].roughness);

	float3 camera_dir = normalize(camera_position.xyz - input.world_position);

	float highlight = pow(max(0, dot(light_reflection, camera_dir)), 5) * (illumination > 0.0f);

	return float4(saturate(color * illumination + highlight), 1.0f);
}
