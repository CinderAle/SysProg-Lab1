#include <windows.h>
#include <gdiplus.h>

#define MAIN_WINDOW_CLASS L"MainWindow"

//Window styles
#define APP_NAME L"Figure Run"
#define WINDOW_UPPER_BORDER 200
#define WINDOW_HEIGHT 500
#define WINDOW_LEFT_BORDER 500
#define WINDOW_WIDTH 800
#define MAIN_WINDOW_STYLE (WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION)

//Figure physics
#define FIGURE_DEFAULT_SPEED 10
#define FIGURE_ACCELERATION 10
#define FIGURE_BORDER_JUMP 15
#define FIGURE_WIDTH_CORRECTION 15
#define FIGURE_HEIGHT_CORRECTION 35

//Figure styles
#define FIGURE_START_X 100
#define FIGURE_START_Y 100
#define FIGURE_WIDTH 128
#define FIGURE_HEIGHT 116
#define BLACK_COLOR RGB(0,0,0)
#define BACKGROUND_COLOR RGB(50,50,50)
#define DARK_GRAY_COLOR RGB(50,50,50)
#define FIGURE_SPRITE_COLOR RGB(100,100,255)

void ShowFigureJump(HWND hWnd);

struct {
	int x, y, width, height;
} FigureCoordinates;

int constexpr CheckDimension(int dimension, int space, int max) noexcept
{
	return dimension < 0 || dimension + space > max ? 0 : 1;
}

int constexpr CheckBorder(int dimension, int space, int max) noexcept
{
	return dimension == 0 || dimension + space == max ? 0 : 1;
}

int CorrectDimension(int dimension, int space, int max) noexcept
{
	if (dimension + space > max) 
	{
		return max - space;
	}
	else if (dimension < 0)
	{
		return 0;
	}
	else 
	{
		return dimension;
	}
}

void ChangeFigureCoordinates(HWND hWnd, int x, int y)
{
	if (CheckDimension(FigureCoordinates.x + x, FigureCoordinates.width, WINDOW_WIDTH - FIGURE_WIDTH_CORRECTION) && CheckDimension(FigureCoordinates.y + y, FigureCoordinates.height, WINDOW_HEIGHT - FIGURE_HEIGHT_CORRECTION))
	{
		FigureCoordinates.x += x;
		FigureCoordinates.y += y;
	}
	else
	{
		FigureCoordinates.x = CorrectDimension(FigureCoordinates.x + x, FigureCoordinates.width, WINDOW_WIDTH - FIGURE_WIDTH_CORRECTION);
		FigureCoordinates.y = CorrectDimension(FigureCoordinates.y + y, FigureCoordinates.height, WINDOW_HEIGHT - FIGURE_HEIGHT_CORRECTION);
	}
	InvalidateRect(hWnd, NULL, TRUE);
	ShowFigureJump(hWnd);
}

void DrawFigure(HDC hDc, HWND hWnd)
{
	Gdiplus::Image figure(L"dorik.bmp");
	if (!figure.GetLastStatus())
	{
		Gdiplus::Graphics graphics(hDc);
		graphics.DrawImage(&figure, FigureCoordinates.x, FigureCoordinates.y, FigureCoordinates.width, FigureCoordinates.height);
	}
	else
	{
		HBRUSH brushR = CreateSolidBrush(FIGURE_SPRITE_COLOR);
		SelectObject(hDc, brushR);
		Rectangle(hDc, FigureCoordinates.x, FigureCoordinates.y, FigureCoordinates.x + FigureCoordinates.width, FigureCoordinates.y + FigureCoordinates.height);
		DeleteObject(brushR);
	}
}

void ShowFigureJump(HWND hWnd)
{
	if (!CheckBorder(FigureCoordinates.x, FigureCoordinates.width, WINDOW_WIDTH - FIGURE_WIDTH_CORRECTION))
	{
		DrawFigure(GetDC(hWnd), hWnd);
		FigureCoordinates.x += FigureCoordinates.x == 0 ? FIGURE_BORDER_JUMP : -FIGURE_BORDER_JUMP;
		Sleep(100);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	if (!CheckBorder(FigureCoordinates.y, FigureCoordinates.height, WINDOW_HEIGHT - FIGURE_HEIGHT_CORRECTION))
	{
		DrawFigure(GetDC(hWnd), hWnd);
		FigureCoordinates.y += FigureCoordinates.y == 0 ? FIGURE_BORDER_JUMP : -FIGURE_BORDER_JUMP;
		Sleep(100);
		InvalidateRect(hWnd, NULL, TRUE);
	}
}

void DrawMainWindow(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDc = BeginPaint(hWnd, &ps);
	HPEN clearBorder = CreatePen(PS_NULL, 1, BLACK_COLOR);
	SelectObject(hDc, clearBorder);
	HBRUSH brush = CreateSolidBrush(BACKGROUND_COLOR);
	SelectObject(hDc, brush);
	Rectangle(hDc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	DrawFigure(hDc, hWnd);
	DeleteObject(clearBorder);
	DeleteObject(brush);
	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
	case WM_PAINT:
		DrawMainWindow(hWnd);
		break;
	case WM_KEYDOWN: 
	{
		int speed;
		if (GetAsyncKeyState(VK_SPACE) < 0)
		{
			speed = FIGURE_DEFAULT_SPEED + FIGURE_ACCELERATION;
		}
		else
		{
			speed = FIGURE_DEFAULT_SPEED;
		}
		if(GetAsyncKeyState(VK_UP) < 0)
			ChangeFigureCoordinates(hWnd, 0, -speed);
		else if (GetAsyncKeyState(VK_DOWN) < 0)
			ChangeFigureCoordinates(hWnd, 0, speed);
		else if (GetAsyncKeyState(VK_LEFT) < 0)
			ChangeFigureCoordinates(hWnd, -speed, 0);
		else if (GetAsyncKeyState(VK_RIGHT) < 0)
			ChangeFigureCoordinates(hWnd, speed, 0);
		
	}
		break;
	case WM_MOUSEWHEEL:
	{
		int speed;
		if (GetAsyncKeyState(VK_SPACE) < 0)
		{
			speed = FIGURE_DEFAULT_SPEED + FIGURE_ACCELERATION;
		}
		else
		{
			speed = FIGURE_DEFAULT_SPEED;
		}
		if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
		{
			ChangeFigureCoordinates(hWnd, 0, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? speed : -speed);
		}
		else
		{
			ChangeFigureCoordinates(hWnd, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -speed : speed, 0);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		ShowFigureJump(hWnd);
	}
		break;
	case WM_MOUSEHWHEEL:
	{
		int speed;
		if (GetAsyncKeyState(VK_SPACE) < 0)
		{
			speed = FIGURE_DEFAULT_SPEED + FIGURE_ACCELERATION;
		}
		else
		{
			speed = FIGURE_DEFAULT_SPEED;
		}
		if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
		{
			ChangeFigureCoordinates(hWnd, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -speed : speed, 0);
		}
		else
		{
			ChangeFigureCoordinates(hWnd, 0, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? speed : -speed);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		ShowFigureJump(hWnd);
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void InitWindowClass(WNDPROC proc, HINSTANCE hInstance, HBRUSH hBrush, const wchar_t* className) noexcept
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = proc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = hBrush;
	wcex.lpszClassName = className;
	RegisterClassEx(&wcex);
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) 
{
	HWND hMainWindow;
	MSG msg;
	InitWindowClass(WndProc, hInstance, (HBRUSH)(0), MAIN_WINDOW_CLASS);
	FigureCoordinates.width = FIGURE_WIDTH;
	FigureCoordinates.height = FIGURE_HEIGHT;
	FigureCoordinates.x = FIGURE_START_X;
	FigureCoordinates.y = FIGURE_START_Y;
	hMainWindow = CreateWindowExW(0, MAIN_WINDOW_CLASS, APP_NAME, MAIN_WINDOW_STYLE, WINDOW_LEFT_BORDER, WINDOW_UPPER_BORDER, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInstance, NULL);
	
	const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}