#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wrl.h>
#include <comdef.h>

class Dx2 final
{
	Microsoft::WRL::ComPtr<ID3D11Device> device3;
	Microsoft::WRL::ComPtr<ID2D1Device> device2;
	Microsoft::WRL::ComPtr<IDWriteFactory> dwfactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> caption_f;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> caption_l;
public:
	Dx2() = default;
	~Dx2() = default;

	void init();

	inline auto getDevice2() const noexcept { return this->device2.Get(); }
	inline auto getCaptionLayout() const noexcept { return this->caption_l.Get(); }
};
