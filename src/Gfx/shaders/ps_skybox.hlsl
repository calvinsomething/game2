TextureCube texture_cube : register(t0);

SamplerState samp : register(s0);

struct PSIn
{
	float3 vertex_position : POSITION;
};

float4 main(PSIn input) : SV_Target
{
	return texture_cube.Sample(samp, input.vertex_position);
}
