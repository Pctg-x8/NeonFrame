#include "comp.h"
#include "appContext.h"

#pragma comment(lib, "dcomp")

#define ThrowOnError(h)		if(FAILED(h)) throw _com_error(h)
constexpr auto DefaultFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
constexpr auto DefaultAlpha = DXGI_ALPHA_MODE_PREMULTIPLIED;
constexpr UINT Margin = 8u;
constexpr UINT CaptionHeight = 20u;

// Factory Methods(ThrowOnErrorを毎回書くのが面倒)
void Comp::createVisual(Microsoft::WRL::ComPtr<IDCompositionVisual2>& visual)
{
	HRESULT hr = this->device->CreateVisual(&visual);
	ThrowOnError(hr);
}
void Comp::createVisual(Microsoft::WRL::ComPtr<IDCompositionVisual2>& visual,
	const Microsoft::WRL::ComPtr<IDCompositionVirtualSurface>& content)
{
	this->createVisual(visual);
	HRESULT hr = visual->SetContent(content.Get());
	ThrowOnError(hr);
}
void Comp::createSurface(Microsoft::WRL::ComPtr<IDCompositionVirtualSurface>& surface)
{
	HRESULT hr = this->device->CreateVirtualSurface(this->currentSize.width, this->currentSize.height,
		DefaultFormat, DefaultAlpha, &surface);
	ThrowOnError(hr);
}
void Comp::createVisualSurfacePair(Microsoft::WRL::ComPtr<IDCompositionVisual2>& visual,
	Microsoft::WRL::ComPtr<IDCompositionVirtualSurface>& surface)
{
	this->createVisual(visual);
	this->createSurface(surface);
	HRESULT hr = visual->SetContent(surface.Get());
	ThrowOnError(hr);
}
void Comp::createGaussianBlur(Microsoft::WRL::ComPtr<IDCompositionGaussianBlurEffect>& eff, float deviation)
{
	HRESULT hr = this->effectFactory->CreateGaussianBlurEffect(&eff);
	ThrowOnError(hr);
	hr = eff->SetStandardDeviation(deviation);
	ThrowOnError(hr);
}
void Comp::createAdditiveBlendingEffect(Microsoft::WRL::ComPtr<IDCompositionBlendEffect>& eff)
{
	HRESULT hr = this->effectFactory->CreateBlendEffect(&eff);
	ThrowOnError(hr);
	hr = eff->SetMode(D2D1_BLEND_MODE_LINEAR_BURN);
	ThrowOnError(hr);
}
inline void makeRelation(const Microsoft::WRL::ComPtr<IDCompositionVisual2>& parent,
	const Microsoft::WRL::ComPtr<IDCompositionVisual2>& child)
{
	HRESULT hr = parent->AddVisual(child.Get(), true, nullptr);
	ThrowOnError(hr);
}
inline void setEffect(const Microsoft::WRL::ComPtr<IDCompositionVisual2>& target,
	const Microsoft::WRL::ComPtr<IDCompositionEffect>& effect)
{
	HRESULT hr = target->SetEffect(effect.Get());
	ThrowOnError(hr);
}
// Initialize //
void Comp::init()
{
	HRESULT hr;

	this->currentSize = D2D1::SizeU(128, 128);

	// Init Device/Target
	hr = DCompositionCreateDevice3(AppContext::instance()->getDx2()->getDevice2(), __uuidof(IDCompositionDesktopDevice), &this->device);
	ThrowOnError(hr);
	hr = this->device.As(&this->effectFactory);
	ThrowOnError(hr);
	hr = this->device->CreateTargetForHwnd(AppContext::instance()->getHwnd(), true, &this->target);
	ThrowOnError(hr);

	// Create Visuals/Surfaces
	this->createVisualSurfacePair(this->background_v, this->background_s);
	this->createVisualSurfacePair(this->border_v, this->border_s);
	this->createVisualSurfacePair(this->caption_v, this->caption_s);
	this->createVisual(this->glowborder_v, this->border_s);
	this->createVisual(this->glowborder2, this->border_s);
	this->createVisual(this->caption_glow[0], this->caption_s);
	this->createVisual(this->caption_glow[1], this->caption_s);
	// AddVisual
	makeRelation(this->background_v, this->border_v);
	makeRelation(this->background_v, this->glowborder2);
	makeRelation(this->background_v, this->glowborder_v);
	makeRelation(this->background_v, this->caption_v);
	makeRelation(this->background_v, this->caption_glow[1]);
	makeRelation(this->background_v, this->caption_glow[0]);
	hr = this->target->SetRoot(this->background_v.Get());
	ThrowOnError(hr);

	this->background_v->SetOffsetX(Margin);
	this->background_v->SetOffsetY(Margin);
	for (auto& v : { this->caption_v, this->caption_glow[0], this->caption_glow[1] })
	{
		v->SetOffsetX(12.0f);
		v->SetOffsetY(6.0f);
	}
	DWRITE_TEXT_METRICS metrics;
	hr = AppContext::instance()->getDx2()->getCaptionLayout()->GetMetrics(&metrics);
	ThrowOnError(hr);
	this->caption_s->Resize(metrics.width, metrics.height);
	this->updateCaption();

	// Init Effects(2.0f, 4.0fで発光具合を調整する)
	this->createGaussianBlur(this->border_glow, 2.0f);
	this->createGaussianBlur(this->border_glow2, 4.0f);
	this->createAdditiveBlendingEffect(this->border_glow_add);
	this->createAdditiveBlendingEffect(this->border_glow2_add);
	hr = this->border_glow_add->SetInput(1, this->border_glow.Get(), 0);
	ThrowOnError(hr);
	hr = this->border_glow2_add->SetInput(1, this->border_glow2.Get(), 0);
	ThrowOnError(hr);
	setEffect(this->glowborder_v, this->border_glow_add);
	setEffect(this->glowborder2, this->border_glow2_add);
	setEffect(this->caption_glow[0], this->border_glow_add);
	setEffect(this->caption_glow[1], this->border_glow2_add);
}

// 背景と枠の再描画
void Comp::updateSurfaces()
{
	POINT offset;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> context;
	HRESULT hr;

	hr = this->background_s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), &context, &offset);
	ThrowOnError(hr);
	context->Clear(D2D1::ColorF(0x40bfff, 0.09375f));
	hr = this->background_s->EndDraw();
	ThrowOnError(hr);
	hr = this->border_s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), &context, &offset);
	ThrowOnError(hr);
	context->Clear(D2D1::ColorF(0, 0.0f));
	{
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		hr = context->CreateSolidColorBrush(D2D1::ColorF(0x209fff), &brush);
		ThrowOnError(hr);
		context->DrawRectangle(D2D1::RectF(offset.x, offset.y, offset.x + this->currentSize.width, offset.y + this->currentSize.height),
			brush.Get(), 3.0f);
	}
	hr = this->border_s->EndDraw();
	ThrowOnError(hr);
}
// タイトルの再描画
void Comp::updateCaption()
{
	POINT offset;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> context;
	HRESULT hr;

	hr = this->caption_s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), &context, &offset);
	ThrowOnError(hr);
	context->Clear(D2D1::ColorF(0, 0.0f));
	{
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		hr = context->CreateSolidColorBrush(D2D1::ColorF(0x80dfff), &brush);
		ThrowOnError(hr);
		context->DrawTextLayout(D2D1::Point2F(), AppContext::instance()->getDx2()->getCaptionLayout(), brush.Get());
	}
	hr = this->caption_s->EndDraw();
	ThrowOnError(hr);
}
// リサイズ
void Comp::resize(LONG w, LONG h)
{
	this->currentSize = D2D1::SizeU(w - Margin * 2, h - Margin * 2);
	HRESULT hr;

	hr = this->background_s->Resize(this->currentSize.width, this->currentSize.height);
	ThrowOnError(hr);
	hr = this->border_s->Resize(this->currentSize.width, this->currentSize.height);
	ThrowOnError(hr);
	this->updateSurfaces();

	this->device->Commit();
}
// WM_NCHITTESTから呼ばれる
LRESULT Comp::chromeHitTest(LONG mx, LONG my)
{
	POINT local;
	local.x = mx;
	local.y = my;
	MapWindowPoints(nullptr, AppContext::instance()->getHwnd(), &local, 1);

	if (Margin <= local.x && local.x <= Margin + this->currentSize.width &&
		Margin <= local.y && local.y <= Margin + this->currentSize.height)
	{
		return HTCAPTION;
	}
	else
	{
		const auto onLeft = local.x <= Margin;
		const auto onRight = Margin + this->currentSize.width <= local.x;
		const auto onTop = local.y <= Margin;
		const auto onBottom = Margin + this->currentSize.height <= local.y;

		if (onLeft)
		{
			return onTop ? HTTOPLEFT : onBottom ? HTBOTTOMLEFT : HTLEFT;
		}
		else if (onRight)
		{
			return onTop ? HTTOPRIGHT : onBottom ? HTBOTTOMRIGHT : HTRIGHT;
		}
		else return onTop ? HTTOP : onBottom ? HTBOTTOM : HTERROR;
	}
}
