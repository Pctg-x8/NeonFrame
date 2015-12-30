#pragma once

#include <windows.h>
#include <dcomp.h>
#include <wrl.h>
#include <comdef.h>

class Comp final
{
	Microsoft::WRL::ComPtr<IDCompositionDesktopDevice> device;
	Microsoft::WRL::ComPtr<IDCompositionDevice3> effectFactory;
	Microsoft::WRL::ComPtr<IDCompositionTarget> target;
	Microsoft::WRL::ComPtr<IDCompositionVisual2> background_v, border_v, glowborder_v, glowborder2, caption_v, caption_glow[2];
	Microsoft::WRL::ComPtr<IDCompositionVirtualSurface> background_s, border_s, caption_s;
	Microsoft::WRL::ComPtr<IDCompositionBlendEffect> border_glow_add, border_glow2_add;
	Microsoft::WRL::ComPtr<IDCompositionGaussianBlurEffect> border_glow, border_glow2;

	D2D1_SIZE_U currentSize;
	void createVisual(Microsoft::WRL::ComPtr<IDCompositionVisual2>& visual);
	void createVisual(Microsoft::WRL::ComPtr<IDCompositionVisual2>& visual,
		const Microsoft::WRL::ComPtr<IDCompositionVirtualSurface>& content);
	void createSurface(Microsoft::WRL::ComPtr<IDCompositionVirtualSurface>& surface);
	void createVisualSurfacePair(Microsoft::WRL::ComPtr<IDCompositionVisual2>& visual,
		Microsoft::WRL::ComPtr<IDCompositionVirtualSurface>& surface);
	void createAdditiveBlendingEffect(Microsoft::WRL::ComPtr<IDCompositionBlendEffect>& eff);
	void createGaussianBlur(Microsoft::WRL::ComPtr<IDCompositionGaussianBlurEffect>& eff, float deviation);

	void updateSurfaces();
	void updateCaption();
public:
	Comp() = default;
	~Comp() = default;

	void init();
	void resize(LONG w, LONG h);
	LRESULT chromeHitTest(LONG mx, LONG my);
};
