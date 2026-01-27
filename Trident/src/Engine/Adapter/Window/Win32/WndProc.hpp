// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	static LRESULT CALLBACK DummyWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(GetPropW(hWnd, WINDOW_DATA_PROPERTY_STRING));

		if (windowData == nullptr) {
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		switch (Msg) {
			case WM_MOVE: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-move.

				windowData->positionX = GET_X_LPARAM(lParam);
				windowData->positionY = GET_Y_LPARAM(lParam);

				return 0;
			}
			case WM_SIZE: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-size.

				const sint32 width = GET_X_LPARAM(lParam);
				const sint32 height = GET_Y_LPARAM(lParam);

				if (windowData->sizeX != width || windowData->sizeY != height) {
					WindowResizeEvent windowResizeEvent(width, height);

					windowData->windowResizeEventListener(windowResizeEvent);
				}

				windowData->sizeX = width;
				windowData->sizeY = height;
				windowData->minimized = wParam == SIZE_MINIMIZED;
				windowData->maximized = wParam == SIZE_MAXIMIZED;

				return 0;
			}
			case WM_CLOSE: {
				// See https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-close.

				WindowCloseEvent windowCloseEvent;

				windowData->windowCloseEventListener(windowCloseEvent);

				return 0;
			}
			case WM_SETCURSOR: {
				// See https://learn.microsoft.com/en-us/windows/win32/menurc/wm-setcursor.

				if (LOWORD(lParam) == HTCLIENT) {
					const WindowClass* windowClassData = static_cast<const WindowClass*>(GetPropW(hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING));

					SetCursor(windowClassData->hCursor);

					return TRUE;
				}

				break;
			}
			case WM_KEYDOWN: {
				// TODO: Extended key and scan code.
				// const uint16 lParamHi = HIWORD(lParam);

				// const bool extendedKey = HIWORD(lParam) & KF_EXTENDED;
				// ENGINE_DEBUG("Extended key = []", extendedKey);

				const uint32 code = static_cast<uint32>(wParam);
				// const uint32 scanCode = MapVirtualKeyW(code, MAPVK_VK_TO_VSC);

				windowData->keyMap[code] = true;

				KeyDownEvent keyDownEvent(static_cast<KeyCode>(code));

				windowData->keyDownEventListener(keyDownEvent);

				return 0;
			}
			case WM_KEYUP: {
				// TODO: Extended key and scan code.
				// const uint16 lParamHi = HIWORD(lParam);

				const uint32 code = static_cast<uint32>(wParam);

				windowData->keyMap[code] = false;

				KeyUpEvent keyUpEvent(static_cast<KeyCode>(code));

				windowData->keyUpEventListener(keyUpEvent);

				return 0;
			}
			case WM_MOUSEMOVE: {
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousemove.

				const sint32 x = GET_X_LPARAM(lParam);
				const sint32 y = GET_Y_LPARAM(lParam);

				sint32 eventX;
				sint32 eventY;

				if (windowData->pointerLocked) {
					const sint32 deltaX = x - windowData->cursorX;
					const sint32 deltaY = y - windowData->cursorY;

					eventX = windowData->virtualCursorPositionX + deltaX;
					eventY = windowData->virtualCursorPositionY + deltaY;

					windowData->virtualCursorPositionX = deltaX;
					windowData->virtualCursorPositionY = deltaY;
				}
				else {
					eventX = x;
					eventY = y;

					windowData->virtualCursorPositionX = x;
					windowData->virtualCursorPositionY = y;
				}

				MouseMoveEvent mouseMoveEvent(eventX, eventY);

				windowData->mouseMoveEventListener(mouseMoveEvent);

				windowData->cursorX = x;
				windowData->cursorY = y;

				return 0;
			}
			case WM_LBUTTONDOWN: {
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttondown.

				MouseDownEvent mouseDownEvent(MouseButton::LEFT);

				windowData->mouseDownEventListener(mouseDownEvent);

				return 0;
			}
			case WM_LBUTTONUP: {
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-lbuttonup.

				MouseUpEvent mouseUpEvent(MouseButton::LEFT);

				windowData->mouseUpEventListener(mouseUpEvent);

				return 0;
			}
			case WM_MOUSEWHEEL: {
				// See https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel.

				const sint16 wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

				MouseWheelEvent mouseWheelEvent(wheelDelta);

				windowData->mouseWheelEventListener(mouseWheelEvent);

				return 0;
			}
		}

		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}
}