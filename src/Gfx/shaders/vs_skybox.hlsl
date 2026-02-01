cbuffer CameraBuffer : register(b0)
{
	vector camera_position;
	matrix view_proj;
};

struct VSOut
{
	float3 vertex_position : POSITION;
	float4 pixel_position : SV_Position;
};

VSOut main(float3 position : POSITION)
{
	VSOut output;

	output.vertex_position = position;

	output.pixel_position = mul(position, view_proj);
	output.pixel_position.z = output.pixel_position.w;

	return output;
}
