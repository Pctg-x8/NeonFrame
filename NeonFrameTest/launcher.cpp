#include "appContext.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return AppContext::instance()->run(); }
