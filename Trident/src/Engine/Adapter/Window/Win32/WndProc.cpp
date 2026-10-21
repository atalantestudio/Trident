// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/WndProc.hpp"

namespace Trident {
	LRESULT CALLBACK DummyWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(GetProp(hWnd, WINDOW_DATA_PROPERTY_STRING));

		if (windowData == nullptr) {
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}

		switch (Msg) {
			case WM_MOVE: {
				windowData->positionX = GET_X_LPARAM(lParam);
				windowData->positionY = GET_Y_LPARAM(lParam);

				return 0;
			}
			case WM_SIZE: {
				const vec2<sint32> size = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				const Extent2D extent = Extent2D(size);

				if (windowData->extent != extent) {
					WindowResizeEvent windowResizeEvent(size);

					windowData->windowResizeEventListener(windowResizeEvent);
				}

				windowData->extent = extent;
				windowData->minimized = wParam == SIZE_MINIMIZED;
				windowData->maximized = wParam == SIZE_MAXIMIZED;

				return 0;
			}
			case WM_CLOSE: {
				WindowCloseEvent windowCloseEvent;

				windowData->windowCloseEventListener(windowCloseEvent);

				return 0;
			}
			case WM_SETCURSOR: {
				if (LOWORD(lParam) == HTCLIENT) {
					const WindowClass* windowClassData = static_cast<const WindowClass*>(GetProp(hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING));

					SetCursor(windowClassData->hCursor);

					return TRUE;
				}

				break;
			}
			case WM_KEYDOWN: {
				const UINT vkCode = static_cast<UINT>(wParam);

				windowData->keyMap[vkCode] = true;

				KeyDownEvent keyDownEvent(static_cast<KeyCode>(vkCode));

				windowData->keyDownEventListener(keyDownEvent);

				return 0;
			}
			case WM_KEYUP: {
				const UINT vkCode = static_cast<UINT>(wParam);

				windowData->keyMap[vkCode] = false;

				KeyUpEvent keyUpEvent(static_cast<KeyCode>(vkCode));

				windowData->keyUpEventListener(keyUpEvent);

				return 0;
			}
			case WM_CHAR: {
				const UINT wideCharStr = static_cast<UINT>(wParam);

				// See https://blog.keyman.com/2008/06/robust-key-mess.
				if (wideCharStr < 32 || wideCharStr == 127) {
					break;
				}

				UINT multiByteStr = 0;
				INT cbMultiByte = WideCharToMultiByte(CP_UTF8, 0, pointer_cast<LPCWCH>(&wideCharStr), -1, NULL, 0, NULL, NULL);

				WIN32_ASSERT(cbMultiByte >= 2 && cbMultiByte <= 5);

				cbMultiByte = WideCharToMultiByte(CP_UTF8, 0, pointer_cast<LPCWCH>(&wideCharStr), -1, pointer_cast<LPSTR>(&multiByteStr), cbMultiByte, NULL, NULL);

				WIN32_ASSERT(cbMultiByte >= 2 && cbMultiByte <= 5);

				TextInputEvent textInputEvent(multiByteStr, cbMultiByte - 1);

				windowData->textInputEventListener(textInputEvent);

				return 0;
			}
			case WM_MOUSEMOVE: {
				const sint32 x = GET_X_LPARAM(lParam);
				const sint32 y = GET_Y_LPARAM(lParam);

				sint32 eventX;
				sint32 eventY;

				if (windowData->pointerLocked) {
					const sint32 deltaX = x - windowData->cursor.x;
					const sint32 deltaY = y - windowData->cursor.y;

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

				windowData->cursor.x = x;
				windowData->cursor.y = y;

				return 0;
			}
			case WM_LBUTTONDOWN: {
				MouseDownEvent mouseDownEvent(MouseButton::LEFT);

				windowData->mouseDownEventListener(mouseDownEvent);

				return 0;
			}
			case WM_LBUTTONUP: {
				MouseUpEvent mouseUpEvent(MouseButton::LEFT);

				windowData->mouseUpEventListener(mouseUpEvent);

				return 0;
			}
			case WM_RBUTTONDOWN: {
				MouseDownEvent mouseDownEvent(MouseButton::RIGHT);

				windowData->mouseDownEventListener(mouseDownEvent);

				return 0;
			}
			case WM_RBUTTONUP: {
				MouseUpEvent mouseUpEvent(MouseButton::RIGHT);

				windowData->mouseUpEventListener(mouseUpEvent);

				return 0;
			}
			case WM_MBUTTONDOWN: {
				MouseDownEvent mouseDownEvent(MouseButton::MIDDLE);

				windowData->mouseDownEventListener(mouseDownEvent);

				return 0;
			}
			case WM_MBUTTONUP: {
				MouseUpEvent mouseUpEvent(MouseButton::MIDDLE);

				windowData->mouseUpEventListener(mouseUpEvent);

				return 0;
			}
			case WM_MOUSEWHEEL: {
				const sint16 delta = GET_WHEEL_DELTA_WPARAM(wParam);

				MouseWheelEvent mouseWheelEvent(delta);

				windowData->mouseWheelEventListener(mouseWheelEvent);

				return 0;
			}
		}

		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}