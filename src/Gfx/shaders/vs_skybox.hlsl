cbuffer CameraBuffer : register(b0)
{
	vector camera_position;
	matrix view_proj;
};

struct VSOut
{
	float3 world_position : POSITION;
	float4 ndc_position : SV_Position;
};

VSOut main(float3 position : POSITION)
{
	VSOut output;

	output.world_position = position;

	output.ndc_position = mul(position, view_proj);
	output.ndc_position.z = output.ndc_position.w;

	return output;
}
