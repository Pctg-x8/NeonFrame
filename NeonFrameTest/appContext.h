#pragma once

#include <windows.h>
#include "dx2.h"
#include "comp.h"
#include <memory>

class AppContext final
{
	AppContext() = default;
	~AppContext() = default;

	HWND hWnd;
	std::unique_ptr<Dx2> dx2;
	std::unique_ptr<Comp> comp;

	void init();
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
public:
	static auto instance() { static AppContext o; return &o; }

	int run();

	inline auto getHwnd() const noexcept { return this->hWnd; }
	inline auto getDx2() const noexcept { return this->dx2.get(); }
	inline auto getComp() const noexcept { return this->comp.get(); }
};
