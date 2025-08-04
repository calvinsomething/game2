struct PSIn
{
    float4 pos : SV_Position;
    float2 tex_coord : TEXCOORD;
};

Texture2D tex : register (t0);
SamplerState samp : register (s0);

float4 main(PSIn input) : SV_Target
{
	return tex.Sample(samp, input.tex_coord);
}
