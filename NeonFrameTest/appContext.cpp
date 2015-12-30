#include "appContext.h"
#include <stdexcept>

void AppContext::init()
{
	WNDCLASSEX wce = { 0 };

	wce.cbSize = sizeof wce;
	wce.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wce.hInstance = GetModuleHandle(nullptr);
	wce.lpszClassName = L"com.cterm2.neonFrameTest.MainFrame";
	wce.lpfnWndProc = &AppContext::WndProc;
	if (!RegisterClassEx(&wce)) throw std::runtime_error("RegisterClassEx");

	this->hWnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP, wce.lpszClassName, L"NeonFrameTest",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, wce.hInstance, nullptr);
	if (this->hWnd == nullptr) throw std::runtime_error("CreateWindowEx");
}

int AppContext::run()
{
	this->init();

	this->dx2 = std::make_unique<Dx2>();
	this->dx2->init();
	this->comp = std::make_unique<Comp>();
	this->comp->init();

	ShowWindow(this->hWnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) DispatchMessage(&msg);
	return msg.wParam;
}

LRESULT CALLBACK AppContext::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_SIZE:
		AppContext::instance()->comp->resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_NCCALCSIZE: return 0;
	case WM_NCHITTEST:
		return AppContext::instance()->comp->chromeHitTest(LOWORD(lParam), HIWORD(lParam));
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
