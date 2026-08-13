#include <windows.h>
#include <stdint.h>
#include <stdbool.h>

#define local_persist   static
#define global_variable static
#define internal        static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// TODO: This is a global for now
// global variables with static are initialized to 0
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void* BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
	int Width = BitmapWidth;
	int Height = BitmapHeight;

	int Pitch = Width*BytesPerPixel;
	uint8 *Row = (uint8*)BitmapMemory;
	for (int Y = 0; Y < BitmapHeight; ++Y)
	{
		uint32*Pixel = (uint32*)Row;
		for (int X = 0; X < BitmapWidth; ++X)
		{
			/*
			 * Pixel in memory: 00 00 00 00
			 * */
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row += Pitch;
	}
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
	// TODO: Bulletproof this.
	// TODO: Free our DIBSection
	if(BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapWidth = Width;
	BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = ClientRect->right - ClientRect->left;
	int WindowHeight = ClientRect->bottom - ClientRect->top;
	StretchDIBits(
			DeviceContext,
			/*
			X, Y, Width, Height,
			X, Y, Width, Height,
			 * */
			0, 0, BitmapWidth, BitmapHeight,
			0, 0, WindowWidth, WindowHeight,
			BitmapMemory,
			&BitmapInfo,
			DIB_RGB_COLORS, SRCCOPY
	);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
						UINT Message,
						WPARAM WParam,
						LPARAM LParam)
{
	LRESULT Result = 0;

    switch (Message)
    {
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width  = ClientRect.right  - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;
			Win32ResizeDIBSection(Width, Height);
		} break;

		case WM_CLOSE:
		{
			// TODO:: Handle this with a message to the user?
			Running = false;
		} break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;

		case WM_DESTROY:
		{
			// TODO:: Handle this with an error
			Running = false;
		} break;

		case WM_PAINT:
		{
			PAINTSTRUCT PaintStructure;
			HDC DeviceContext = BeginPaint(Window, &PaintStructure);
			int X = PaintStructure.rcPaint.left;
			int Y = PaintStructure.rcPaint.top;
			int Width = PaintStructure.rcPaint.right - PaintStructure.rcPaint.left;
			int Height = PaintStructure.rcPaint.bottom - PaintStructure.rcPaint.top;
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			EndPaint(Window, &PaintStructure);
		} break;
		default:
		{
			Result = DefWindowProcA(Window, Message, WParam, LParam);
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
    WindowClass.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = &Win32MainWindowCallback;
    WindowClass.cbClsExtra  = 0;
    WindowClass.cbWndExtra  = 0;
    WindowClass.hInstance   = Instance;
    // WindowClass.hIcon;
    WindowClass.hCursor       = 0;
    WindowClass.hbrBackground = 0;
    WindowClass.lpszMenuName  = 0;
    WindowClass.lpszClassName = "TetrisWindowClass";

    // Register Window Class
	if(RegisterClassA(&WindowClass))
	{
		// Create a Window Handle
		HWND Window = CreateWindowExA(
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

		if(Window)
		{
			Running = true;
			int XOffset = 0;
			int YOffset = 0;

			while(Running)
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						Running = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				RenderWeirdGradient(XOffset, YOffset);

				HDC DeviceContext = GetDC(Window);
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int WindowWidth = ClientRect.right - ClientRect.left;
				int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
				ReleaseDC(Window, DeviceContext);

				++XOffset;
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
