#include <windows.h>
#include <gdiplus.h>
#include "resource.h"

#define MAIN_WINDOW_CLASS L"MainWindow"

//Figure size error strings
#define FIGURE_SIZE_ERROR_TEXT L"The size of the figure was too large for the window!"
#define FIGURE_SIZE_ERROR_CAPTION L"Too large figure"

//Window styles
#define APP_NAME L"Figure Run"
#define WINDOW_UPPER_BORDER 200
#define WINDOW_HEIGHT 500
#define WINDOW_LEFT_BORDER 500
#define WINDOW_WIDTH 800
#define MAIN_WINDOW_STYLE (WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION)
#define WINDOW_ICON L"javalogo.bmp"

//Figure physics
#define FIGURE_DEFAULT_SPEED 10
#define FIGURE_MOUSE_SPEED 1
#define FIGURE_ACCELERATION 10
#define FIGURE_BORDER_JUMP 15
#define FIGURE_WIDTH_CORRECTION 15
#define FIGURE_HEIGHT_CORRECTION 35
#define FIGURE_SLEEP_AFTER_HIT 50

//Colors
#define BLACK_COLOR RGB(0,0,0)
#define DARK_GRAY_COLOR RGB(50,50,50)
#define RED_COLOR RGB(255,100,100)
#define BLUE_COLOR RGB(100,100,255)
#define YELLOW_COLOR RGB(208,224,83)
#define GREEN_COLOR RGB(83,224,125)
#define WHITE_COLOR RGB(255,255,255)

//Figure styles
#define FIGURE_WIDTH 200
#define FIGURE_HEIGHT 100
#define FIGURE_START_X (WINDOW_WIDTH / 2 - FIGURE_WIDTH / 2 - 15)
#define FIGURE_START_Y (WINDOW_HEIGHT / 2 - FIGURE_HEIGHT / 2 - 25)
#define BACKGROUND_COLOR DARK_GRAY_COLOR
#define FIGURE_SPRITE_COLOR YELLOW_COLOR
#define FIGURE_IMAGE_SRC L"dorik.bmp"
#define MAX_FIGURE_WIDTH (WINDOW_WIDTH - 50)
#define MAX_FIGURE_HEIGHT (WINDOW_HEIGHT - 50)
#define FIGURE_HIT_BORDER_WIDTH 2
#define FIGURE_HIT_BORDER_COLOR WHITE_COLOR

//Auto motion constants
#define INCREASE_COORDINATE 1
#define DECREASE_COORDINATE -1
#define ACCELERATION_VK 0x58
#define AUTO_MOTION_SLEEP 5

void ShowFigureJump(HWND hWnd, int windowWidth, int windowHeight);

struct {
	int x, y, width, height;
} FigureCoordinates;

int horizontalMotion = 0;
int verticalMotion = 0;

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
	RECT windowSize;
	GetWindowRect(hWnd, &windowSize);
	const int windowWidth = windowSize.right - windowSize.left;
	const int windowHeight = windowSize.bottom - windowSize.top;
	if (CheckDimension(FigureCoordinates.x + x, FigureCoordinates.width, windowWidth - FIGURE_WIDTH_CORRECTION) && CheckDimension(FigureCoordinates.y + y, FigureCoordinates.height, windowHeight - FIGURE_HEIGHT_CORRECTION))
	{
		FigureCoordinates.x += x;
		FigureCoordinates.y += y;
	}
	else
	{
		FigureCoordinates.x = CorrectDimension(FigureCoordinates.x + x, FigureCoordinates.width, windowWidth - FIGURE_WIDTH_CORRECTION);
		FigureCoordinates.y = CorrectDimension(FigureCoordinates.y + y, FigureCoordinates.height, windowHeight - FIGURE_HEIGHT_CORRECTION);
	}
	InvalidateRect(hWnd, NULL, TRUE);
	ShowFigureJump(hWnd, windowWidth, windowHeight);
}

void DrawFigure(HDC hDc, HWND hWnd)
{
	Gdiplus::Image figure(FIGURE_IMAGE_SRC);
	if (!figure.GetLastStatus())
	{
		Gdiplus::Graphics graphics(hDc);
		graphics.DrawImage(&figure, FigureCoordinates.x, FigureCoordinates.y, FigureCoordinates.width, FigureCoordinates.height);
	}
	else
	{
		HBRUSH figureBrush = CreateSolidBrush(FIGURE_SPRITE_COLOR);
		SelectObject(hDc, figureBrush);
		Rectangle(hDc, FigureCoordinates.x, FigureCoordinates.y, FigureCoordinates.x + FigureCoordinates.width, FigureCoordinates.y + FigureCoordinates.height);
		DeleteObject(figureBrush);
	}
}

void ShowFigureJump(HWND hWnd, int windowWidth, int windowHeight)
{
	if (!CheckBorder(FigureCoordinates.x, FigureCoordinates.width, windowWidth - FIGURE_WIDTH_CORRECTION))
	{
		HPEN borderPen = CreatePen(PS_SOLID, FIGURE_HIT_BORDER_WIDTH, FIGURE_HIT_BORDER_COLOR);
		HDC hDc = GetDC(hWnd);
		SelectObject(hDc, borderPen);
		DrawFigure(hDc, hWnd);
		DeleteObject(borderPen);
		ReleaseDC(hWnd, hDc);
		FigureCoordinates.x += FigureCoordinates.x == 0 ? FIGURE_BORDER_JUMP : -FIGURE_BORDER_JUMP;
		Sleep(FIGURE_SLEEP_AFTER_HIT);
		InvalidateRect(hWnd, NULL, TRUE);
		if (horizontalMotion != 0)
		{
			horizontalMotion = (horizontalMotion == INCREASE_COORDINATE ? DECREASE_COORDINATE : INCREASE_COORDINATE);
		}
	}
	if (!CheckBorder(FigureCoordinates.y, FigureCoordinates.height, windowHeight - FIGURE_HEIGHT_CORRECTION))
	{
		HPEN borderPen = CreatePen(PS_SOLID, FIGURE_HIT_BORDER_WIDTH, FIGURE_HIT_BORDER_COLOR);
		HDC hDc = GetDC(hWnd);
		SelectObject(hDc, borderPen);
		DrawFigure(hDc, hWnd);
		DeleteObject(borderPen);
		ReleaseDC(hWnd, hDc);
		FigureCoordinates.y += FigureCoordinates.y == 0 ? FIGURE_BORDER_JUMP : -FIGURE_BORDER_JUMP;
		Sleep(FIGURE_SLEEP_AFTER_HIT);
		InvalidateRect(hWnd, NULL, TRUE);
		if (verticalMotion)
		{
			verticalMotion = (verticalMotion == INCREASE_COORDINATE ? DECREASE_COORDINATE : INCREASE_COORDINATE);
		}
	}
}

void DrawMainWindow(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDc = BeginPaint(hWnd, &ps);

	HDC hdcBuffer = CreateCompatibleDC(hDc);
	HBITMAP hbmBuffer = CreateCompatibleBitmap(hDc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdcBuffer, hbmBuffer);

	HPEN clearBorder = CreatePen(PS_NULL, 0, BLACK_COLOR);
	SelectObject(hdcBuffer, clearBorder);
	HBRUSH brush = CreateSolidBrush(BACKGROUND_COLOR);
	SelectObject(hdcBuffer, brush);

	RECT windowSize;
	GetWindowRect(hWnd, &windowSize);

	Rectangle(hdcBuffer, 0, 0, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top);
	DrawFigure(hdcBuffer, hWnd);
	DeleteObject(clearBorder);
	DeleteObject(brush);

	BitBlt(hDc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);
	DeleteObject(hbmBuffer);
	DeleteDC(hdcBuffer);

	EndPaint(hWnd, &ps);
}

void StopAutoMotion() noexcept
{
	verticalMotion = 0;
	horizontalMotion = 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	int speed = ((GetAsyncKeyState(ACCELERATION_VK) >= 0) ? 0 : FIGURE_ACCELERATION);
	switch (uMsg) 
	{
	case WM_PAINT:
		DrawMainWindow(hWnd);
		break;
	case WM_COMMAND:
		speed += FIGURE_DEFAULT_SPEED;
		switch (LOWORD(wParam))
		{
		case ID_BTN_UP:
			ChangeFigureCoordinates(hWnd, 0, -speed);
			StopAutoMotion();
			break;
		case ID_BTN_LEFT:
			ChangeFigureCoordinates(hWnd, -speed, 0);
			StopAutoMotion();
			break;
		case ID_BTN_DOWN:
			ChangeFigureCoordinates(hWnd, 0, speed);
			StopAutoMotion();
			break;
		case ID_BTN_RIGHT:
			ChangeFigureCoordinates(hWnd, speed, 0);
			StopAutoMotion();
			break;
		case ID_BTN_W:
			verticalMotion = (verticalMotion != DECREASE_COORDINATE ? DECREASE_COORDINATE : 0);
			break;
		case ID_BTN_A:
			horizontalMotion = (horizontalMotion != DECREASE_COORDINATE ? DECREASE_COORDINATE : 0);
			break;
		case ID_BTN_S:
			verticalMotion = (verticalMotion != INCREASE_COORDINATE ? INCREASE_COORDINATE : 0);
			break;
		case ID_BTN_D:
			horizontalMotion = (horizontalMotion != INCREASE_COORDINATE ? INCREASE_COORDINATE : 0);
			break;
		default:
			StopAutoMotion();
			break;
		}
		break;
	case WM_MOUSEWHEEL:
	{
		StopAutoMotion();
		speed += FIGURE_MOUSE_SPEED;
		if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
		{
			ChangeFigureCoordinates(hWnd, 0, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? speed : -speed);
		}
		else
		{
			ChangeFigureCoordinates(hWnd, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -speed : speed, 0);
		}
	}
		break;
	case WM_MOUSEHWHEEL:
	{
		StopAutoMotion();
		speed += FIGURE_MOUSE_SPEED;
		if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
		{
			ChangeFigureCoordinates(hWnd, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -speed : speed, 0);
		}
		else
		{
			ChangeFigureCoordinates(hWnd, 0, GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? speed : -speed);
		}
	}
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	if (verticalMotion != 0)
	{
		speed += FIGURE_MOUSE_SPEED;
		ChangeFigureCoordinates(hWnd, 0, verticalMotion == INCREASE_COORDINATE ? speed : -speed);
		Sleep(AUTO_MOTION_SLEEP);
	}
	if (horizontalMotion != 0)
	{
		speed += FIGURE_MOUSE_SPEED;
		ChangeFigureCoordinates(hWnd, horizontalMotion == INCREASE_COORDINATE ? speed : -speed, 0);
		Sleep(AUTO_MOTION_SLEEP);
	}
	return 0;
}

void CorrectFigureParameters(HWND hWnd) noexcept
{
	if (FigureCoordinates.width > MAX_FIGURE_WIDTH)
	{
		FigureCoordinates.x = 1;
		FigureCoordinates.width = MAX_FIGURE_WIDTH;
	}
	else
	{
		FigureCoordinates.x = FIGURE_START_X;
		FigureCoordinates.width = FIGURE_WIDTH + 1;
	}
	if (FigureCoordinates.height > MAX_FIGURE_HEIGHT)
	{
		FigureCoordinates.y = 1;
		FigureCoordinates.height = MAX_FIGURE_HEIGHT;
	}
	else
	{
		FigureCoordinates.height = FIGURE_HEIGHT + 1;
		FigureCoordinates.y = FIGURE_START_Y;
	}
	if (FigureCoordinates.width > MAX_FIGURE_WIDTH || FigureCoordinates.height > MAX_FIGURE_HEIGHT)
	{
		MessageBox(hWnd, FIGURE_SIZE_ERROR_TEXT, FIGURE_SIZE_ERROR_CAPTION, MB_OK);
	}
}

void InitWindowClass(WNDPROC proc, HINSTANCE hInstance, HBRUSH hBrush, const wchar_t* className) noexcept
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = proc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconW(hInstance, WINDOW_ICON);
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
	hMainWindow = CreateWindowEx(0, MAIN_WINDOW_CLASS, APP_NAME, MAIN_WINDOW_STYLE, WINDOW_LEFT_BORDER, WINDOW_UPPER_BORDER, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInstance, NULL);

	FigureCoordinates.width = FIGURE_WIDTH;
	FigureCoordinates.height = FIGURE_HEIGHT;
	CorrectFigureParameters(hMainWindow);

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!TranslateAccelerator(hMainWindow, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}