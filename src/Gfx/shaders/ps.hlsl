struct PSIn
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct Material
{
	float4 color;
	int diffuse_map_index;
	int normal_map_index;
	float roughness;
};

StructuredBuffer<Material> materials : register(t0);

cbuffer MeshBuffer : register(b0)
{
	uint material_index;
};

float4 main(PSIn input) : SV_Target
{
    // return input.color;
	float4 color = materials[material_index].color;
	color.x += 0.05f;
	color.y += 0.05f;
	color.z += 0.05f;

	float4 light = float4(2.4f, 2.4f, 2.4f, 1.0f);
	
	return saturate(color * light);
}
