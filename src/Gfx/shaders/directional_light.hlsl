cbuffer LightingBuffer : register(b1)
{
	float4 light_position_w_ambient_amount;
	matrix light_view_proj;
};

// Shadow Map
//
Texture2D shadow_map : register(t6);
SamplerComparisonState shadow_samp : register(s6);
//


float shadow(float3 world_position, float min)
{
	float4 position = mul(float4(world_position, 1.0f), light_view_proj);

	float2 uv = {position.x * 0.5f + 0.5f, position.y * -0.5f + 0.5f};

	float width, height;
	shadow_map.GetDimensions(width, height);

	float texel_size = float2(1.0f / width, 1.0f / height);

	float result = 0.0f;

	[unroll]
	for (int x = -2; x <= 2; x++)
	{
		[unroll]
        for (int y = -2; y <= 2; y++)
		{
            result += shadow_map.SampleCmpLevelZero(shadow_samp, uv + float2(x, y) * texel_size, position.z);
		}
	}

	return max(min, result / 25.0f);
}

