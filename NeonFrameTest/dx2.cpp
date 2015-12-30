#include "dx2.h"
#include <stdexcept>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#define ThrowOnError(v) if(FAILED(v)) throw _com_error(v)

void Dx2::init()
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<IDXGIDevice> devicex;

	// Init DirectX
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &this->device3,
		nullptr, nullptr);
	ThrowOnError(hr);
	hr = this->device3.As(&devicex);
	ThrowOnError(hr);
	hr = D2D1CreateDevice(devicex.Get(),
		D2D1::CreationProperties(D2D1_THREADING_MODE_SINGLE_THREADED, D2D1_DEBUG_LEVEL_ERROR, D2D1_DEVICE_CONTEXT_OPTIONS_NONE),
		&this->device2);
	ThrowOnError(hr);

	// Init DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &this->dwfactory);
	ThrowOnError(hr);
	hr = this->dwfactory->CreateTextFormat(L"Yu Gothic UI", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		12.0f, L"ja-JP", &this->caption_f);
	ThrowOnError(hr);
	hr = this->dwfactory->CreateTextLayout(L"NeonFrameTest", 13, this->caption_f.Get(), _FMAX, _FMAX, &this->caption_l);
	ThrowOnError(hr);
}
