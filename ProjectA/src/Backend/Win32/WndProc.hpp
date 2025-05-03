#pragma once

namespace ProjectA {
	static LRESULT CALLBACK DummyWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	static LRESULT CALLBACK TestWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		if (Msg > 49000) {
			// TODO: Handle client messages.
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		Win32WindowData* windowData = static_cast<Win32WindowData*>(GetPropW(hWnd, WINDOW_DATA_PROPERTY_STRING));

		if (windowData == nullptr) {
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		const WindowClass* windowClassData = static_cast<const WindowClass*>(GetPropW(hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING));

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

				//return 0;
				break;
			}
			case WM_KILLFOCUS:
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-killfocus.
				//return 0;
				break;
			case WM_PAINT:
				// Ignore message.
				break;
			case WM_CLOSE:
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-close.
				// TODO: Handle WM_CLOSE message.
				//BREAKPOINT();

				return 0;
			case WM_QUERYOPEN:
				// Ignore message.
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
			case WM_NCLBUTTONUP:
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
			case WM_MOUSEWHEEL:
				// TODO: Handle WM_MOUSEWHEEL message.
				break;
			case WM_MOUSEHWHEEL:
				// TODO: Handle WM_MOUSEHWHEEL message.
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

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(GetPropW(hWnd, WINDOW_DATA_PROPERTY_STRING));

		if (windowData == nullptr) {
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		const WindowClass* windowClassData = static_cast<const WindowClass*>(GetPropW(hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING));

		switch (Msg) {
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

				if (windowData->windowResizeEventListener) {
					WindowResizeEvent windowResizeEvent(windowData->sizeX, windowData->sizeY);

					windowData->windowResizeEventListener(windowResizeEvent);
				}

				ENGINE_DEBUG("wm_size");

				return 0;
			}
			case WM_SETFOCUS: {
				// TODO

				break;
			}
			case WM_KILLFOCUS: {
				// TODO

				break;
			}
			case WM_CLOSE: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-close.
				if (windowData->windowCloseEventListener) {
					WindowCloseEvent windowCloseEvent;

					windowData->windowCloseEventListener(windowCloseEvent);
				}

				return 0;
			}
			case WM_SETCURSOR: {
				// See https://learn.microsoft.com/en-us/windows/win32/menurc/wm-setcursor.
				if (LOWORD(lParam) == HTCLIENT) {
					SetCursor(windowClassData->hCursor);

					return TRUE;
				}

				break;
			}
			default:
				break;
		}

		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}
}