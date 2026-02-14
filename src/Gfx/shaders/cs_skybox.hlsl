Buffer<float4> equirectangle : register(t0);

RWTexture2DArray<float4> cube : register(u0);

static const uint FACE_X_POS = 0,
	FACE_X_NEG = 1,
	FACE_Y_POS = 2,
	FACE_Y_NEG = 3,
	FACE_Z_POS = 4,
	FACE_Z_NEG = 5;

static const float PI_INV = 1.0f / 3.14159265f;

static const float inv_cube_span_div_2 = 1.0f / 512.0f;
static const uint buffer_width = 4096;
static const uint buffer_height = 2048;

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{

	float2 src = float2(
		(id.x + 0.5f) * inv_cube_span_div_2 - 1.0f,
		(id.y + 0.5f) * -inv_cube_span_div_2 + 1.0f);

	float3 direction;

	[branch] switch (id.z)
	{
	case FACE_X_POS:
		direction = float3(1.0f, src.y, -src.x);
		break;
	case FACE_X_NEG:
		direction = float3(-1.0f, src.y, src.x);
		break;
	case FACE_Y_POS:
		direction = float3(src.x, 1.0f, -src.y);
		break;
	case FACE_Y_NEG:
		direction = float3(src.x, -1.0f, src.y);
		break;
	case FACE_Z_POS:
		direction = float3(src.x, src.y, 1.0f);
		break;
	default:
		direction = float3(-src.x, src.y, -1.0f);
		break;
	}

	float longitude = atan2(direction.x, direction.z);
	float latitude = atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));

	float radians_per_texel = buffer_height * PI_INV;

	uint u = uint(buffer_height + longitude * radians_per_texel + buffer_width) % buffer_width;
	uint v = uint(buffer_height * 0.5f - latitude * radians_per_texel + buffer_height) % buffer_height;

	uint i = v * buffer_width + u;

	cube[id] = equirectangle[i];
}
