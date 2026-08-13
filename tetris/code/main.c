#include <windows.h>

LRESULT Wndproc(
		HWND WindowHandle,
		UINT Message,
		WPARAM WParam,
		LPARAM LParam
		)
{
	LRESULT Result;
    switch (Message)
    {
		case WM_CREATE:
		{
			OutputDebugStringA("WM_CREATE\n");
		} break;
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
			if (MessageBoxA(WindowHandle, "Really quit?", "My application", MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(WindowHandle);
			}
			// Else: User canceled. Do nothing.
			return 0;

		} break;
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT PaintStructure;
			HDC DeviceContext = BeginPaint(WindowHandle, &PaintStructure);
			// Painting

			// FillRect(DeviceContext, &PaintStructure.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
			int X = PaintStructure.rcPaint.left;
			int Y = PaintStructure.rcPaint.right;
			int Width = PaintStructure.rcPaint.right - PaintStructure.rcPaint.left;
			int Height = PaintStructure.rcPaint.top - PaintStructure.rcPaint.bottom;
			static DWORD Operation = WHITENESS;
			PatBlt(DeviceContext, X, Y, Width, Height, Operation);
			if(Operation == WHITENESS)
			{
				Operation = BLACKNESS;
			}
			else
			{
				Operation = WHITENESS;
			}
			EndPaint(WindowHandle, &PaintStructure);
		} break;
		default:
		{
			Result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
		} break;
    }

	return Result;
}

int WINAPI wWinMain(
		HINSTANCE Instance,
		HINSTANCE PrevInstance,
		PWSTR CommandLine,
		int CmdShow)
{
	// Define Window Class
    WNDCLASSA WindowClass = {0};

    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = &Wndproc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon;
    WindowClass.hCursor = 0;
    WindowClass.hbrBackground = 0;
    WindowClass.lpszMenuName = 0;
    WindowClass.lpszClassName = "TetrisWindowClass";

    // Register Window Class
	if(RegisterClassA(&WindowClass))
	{
		// Create a Window Handle
		HWND WindowHandle = CreateWindowExA(
							0,
							WindowClass.lpszClassName,
							"Tetris",
							WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							0,
							0,
							WindowClass.hInstance,
							0);

		if(WindowHandle)
		{
			for(;;)
			{
				// Get Messages from the Message Queue
				MSG Message;
				BOOL MessageResult = GetMessage(&Message, WindowHandle, 0, 0);
				if(MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
				}
			}

		}
		else
		{
			//TODO: Logging info
		}
	}
	else
	{
		//TODO: Logging info
	}

    return (0);
}
