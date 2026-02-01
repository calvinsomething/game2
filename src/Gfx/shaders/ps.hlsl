struct PSIn
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 light_direction : NORMAL1;
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
	float3 light_direction = normalize(input.light_direction);

	float illumination = (max(0, dot(normal, light_direction)) + light_position_w_ambient_amount.w);

	float3 color = (materials[material_index].albedo_color + float3(0.1f, 0.1f, 0.1f)) * illumination * 2.0f;

	return saturate(float4(color + materials[material_index].emissive_color, 1.0f));
}
