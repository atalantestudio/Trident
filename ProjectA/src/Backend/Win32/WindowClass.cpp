#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/API/Event/MouseEvent.hpp"
#include "Engine/API/Window/WindowClass.hpp"

namespace ProjectA {
	static LRESULT CALLBACK Wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		if (Msg > 49000) {
			// TODO: Handle client messages.
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		Win32WindowData* windowData = static_cast<Win32WindowData*>(GetPropW(hWnd, WINDOW_DATA_PROPERTY_STRING));

		if (windowData == nullptr) {
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		const Win32WindowClassData* windowClassData = static_cast<const Win32WindowClassData*>(GetPropW(hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING));

		switch (Msg) {
			case WM_CREATE:
				// Ignore message (already handled through WM_NCCREATE).
				break;
			case WM_DESTROY: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-destroy.
				PostQuitMessage(0);

				return 0;
			}
			case WM_MOVE: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-move.
				windowData->positionX = GET_X_LPARAM(lParam);
				windowData->positionY = GET_Y_LPARAM(lParam);

				return 0;
			}
			case WM_SIZE: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-size.
				windowData->sizeX = GET_X_LPARAM(lParam);
				windowData->sizeY = GET_Y_LPARAM(lParam);
				windowData->minimized = wParam == SIZE_MINIMIZED;
				windowData->maximized = wParam == SIZE_MAXIMIZED;

				return 0;
			}
			case WM_ACTIVATE:
				// Ignore message.
				break;
			case WM_SETFOCUS: {
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-setfocus.

				/*RECT rect;

				GetClientRect(hWnd, &rect);

				POINT topLeftPoint{};
				topLeftPoint.x = rect.left;
				topLeftPoint.y = rect.top;

				POINT bottomRightPoint{};
				bottomRightPoint.x = rect.right;
				bottomRightPoint.y = rect.bottom;

				ClientToScreen(hWnd, (POINT*) &rect.left);
				ClientToScreen(hWnd, (POINT*) &rect.right);

				ClipCursor(&rect);*/

				return 0;
			}
			case WM_KILLFOCUS:
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-killfocus.
				return 0;
			case WM_PAINT: {
				// See https://learn.microsoft.com/en-us/windows/win32/gdi/wm-paint.
				/*PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				// TODO: Handle WM_PAINT message.

				FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
				EndPaint(hWnd, &ps);

				return 0;*/
				break;
			}
			case WM_CLOSE:
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-close.
				// TODO: Handle WM_CLOSE message.
				BREAKPOINT();

				break;
			case WM_ERASEBKGND:
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-erasebkgnd.
				// TODO: Handle WM_ERASEBKGND message.
				//return TRUE;
				break;
			case WM_SHOWWINDOW:
				// Ignore message.
				break;
			case WM_ACTIVATEAPP:
				// Ignore message.
				break;
			case WM_SETCURSOR: {
				// See https://learn.microsoft.com/en-us/windows/win32/menurc/wm-setcursor.
				if (LOWORD(lParam) == HTCLIENT) {
					SetCursor(windowClassData->hCursor);

					return TRUE;
				}

				break;
			}
			case WM_MOUSEACTIVATE:
				// Ignore message.
				break;
			case WM_GETMINMAXINFO: {
				// TODO: Update MINMAXINFO struct.
				//const MINMAXINFO* mmi = (const MINMAXINFO*) lParam;

				//return 0;
				break;
			}
			case WM_WINDOWPOSCHANGING:
				// Ignore message.
				break;
			case WM_WINDOWPOSCHANGED:
				// Ignore message.
				break;
			case WM_GETICON:
				// Ignore message.
				break;
			case WM_NCCALCSIZE:
				// Ignore message.
				break;
			case WM_NCHITTEST:
				// Ignore message.
				break;
			case WM_NCPAINT:
				// See https://learn.microsoft.com/en-us/windows/win32/gdi/wm-ncpaint.
				// TODO: Handle WM_NCPAINT message.
				break;
			case WM_NCACTIVATE:
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-ncactivate.
				return wParam == FALSE;
			case WM_SYNCPAINT:
				// Ignore message.
				break;
			case WM_NCMOUSEMOVE:
				// Ignore message.
				break;
			case WM_NCLBUTTONDOWN:
				// Ignore message.
				break;
			case WM_NCLBUTTONDBLCLK:
				// Ignore message.
				break;
			case WM_KEYDOWN:
				// TODO: Handle WM_KEYDOWN message.
				BREAKPOINT();

				break;
			case WM_SYSKEYDOWN:
				// TODO: Handle WM_SYSKEYDOWN message.
				BREAKPOINT();

				break;
			case WM_SYSCOMMAND:
				// See https://learn.microsoft.com/en-us/windows/win32/menurc/wm-syscommand.

				break;
			case WM_MOUSEMOVE:
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove.

				windowData->cursorX = GET_X_LPARAM(lParam);
				windowData->cursorY = GET_Y_LPARAM(lParam);

				return 0;
			case WM_LBUTTONDOWN:
				// TODO: Handle WM_LBUTTONDOWN message.
				break;
			case WM_LBUTTONUP:
				// TODO: Handle WM_LBUTTONUP message.
				break;
			case WM_ENTERMENULOOP:
				// TODO: Handle WM_ENTERMENULOOP message.
				break;
			case WM_EXITMENULOOP:
				// TODO: Handle WM_EXITMENULOOP message.
				break;
			case WM_SIZING:
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-sizing.
				// TODO: Update aspect ratio.

				break;
			case WM_CAPTURECHANGED:
				// TODO: Handle WM_CAPTURECHANGED message.
				break;
			case WM_MOVING:
				// Ignore message.
				break;
			case WM_ENTERSIZEMOVE:
				// TODO: Handle WM_ENTERSIZEMOVE message.
				break;
			case WM_EXITSIZEMOVE:
				// TODO: Handle WM_EXITSIZEMOVE message.
				break;
			case WM_IME_SETCONTEXT:
				// Ignore message.
				break;
			case WM_IME_NOTIFY:
				// Ignore message.
				break;
			case WM_IME_REQUEST:
				// Ignore message.
				break;
			case WM_NCMOUSELEAVE:
				// Ignore message.
				break;
			case WM_DWMNCRENDERINGCHANGED:
				// Ignore message.
				break;
			default:
				BREAKPOINT();

				break;
		}

		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	WindowClass::WindowClass(const WindowClassDescriptor& descriptor) {
		const HINSTANCE hInstance = GetInstance();

		WNDCLASSEXW wndClass{};
		wndClass.cbSize = sizeof(WNDCLASSEX);

		// See https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles.
		wndClass.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;

		wndClass.lpfnWndProc = Wndproc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = nullptr;
		//wndClass.hIcon = LoadIconW(hInstance, IDI_APPLICATION);
		wndClass.hCursor = nullptr;
		wndClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = ConvertUTF8ToUTF16(descriptor.name);
		wndClass.hIconSm = nullptr;

		const ATOM result = RegisterClassExW(&wndClass);

		ENGINE_ASSERT(result != 0, "Failed to register Win32 window class.");

		Win32WindowClassData* windowClassData = new Win32WindowClassData();
		windowClassData->className = wndClass.lpszClassName;
		windowClassData->hCursor = LoadCursorW(nullptr, IDC_ARROW);

		data = windowClassData;
	}

	WindowClass::~WindowClass() {
		const HINSTANCE hInstance = GetInstance();
		const Win32WindowClassData* windowClassData = static_cast<const Win32WindowClassData*>(data);

		const BOOL result = UnregisterClassW(windowClassData->className, hInstance);

		ENGINE_ASSERT(result, "Failed to unregister Win32 window class.");

		delete windowClassData;

		data = nullptr;
	}
}