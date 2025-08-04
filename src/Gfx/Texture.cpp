#include "Texture.h"

#include <vector>

#include "../Error.h"

Texture::Texture(Gfx &gfx) : GfxAccess(gfx)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 100;
	desc.Height = 100;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// test texture
	std::vector<DirectX::XMFLOAT4> tester(10000);

	for (auto &f : tester)
	{
		f = DirectX::XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f};
	}

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = tester.data();
	data.SysMemPitch = static_cast<UINT>(desc.Width * sizeof(tester[0]));
	data.SysMemSlicePitch = static_cast<UINT>(tester.size() * sizeof(tester[0]));

	HANDLE_GFX_ERR(device->CreateTexture2D(&desc, &data, texture.GetAddressOf()));

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HANDLE_GFX_ERR(device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
	view_desc.Format = desc.Format;
	view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texture.Get(), &view_desc, view.GetAddressOf());
}

ID3D11SamplerState *Texture::get_sampler()
{
	return sampler_state.Get();
}

ID3D11ShaderResourceView *Texture::get_view()
{
	return view.Get();
}
