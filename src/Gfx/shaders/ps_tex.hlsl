struct PSIn
{
    float4 pos : SV_Position;
    // float3 normal : NORMAL;
    float2 diffuse_map_coordinates : TEXCOORD;
    float2 normal_map_coordinates : TEXCOORD;
};

cbuffer MeshBuffer : register(b0)
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

SamplerState samp : register (s0);

float4 main(PSIn input) : SV_Target
{
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
		default:
			color = materials[material_index].color;
			break;
	}

	// float3 normal;
	// switch (materials[material_index].normal_map_index)
	// {
	// 	case 0:
	// 		normal = tex_0.Sample(samp, input.normal_map_coordinates);
	// 		break;
	// 	case 1:
	// 		normal = tex_1.Sample(samp, input.normal_map_coordinates);
	// 		break;
	// 	case 2:
	// 		normal = tex_2.Sample(samp, input.normal_map_coordinates);
	// 		break;
	// 	default:
	// 		normal = materials[material_index].color;
	// 		break;
	// }

	return color;
}
