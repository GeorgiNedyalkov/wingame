#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <Xinput.h>

#define local_persist   static
#define global_variable static
#define internal        static

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
} win32_offscreen_buffer;

typedef struct win32_window_dimension
{
	int Width;
	int Height;
} win32_window_dimension;

// NOTE: Some Voodoo Magic Here
#define X_INPUT_GET_STATE(name) WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE (x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return(0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;

#define X_INPUT_SET_STATE(name) WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE (x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return (0);
}
global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;

#define XInputSetState XInputSetState_
#define XInputGetState XInputGetState_

internal void
Win32LoadXInput()
{
	HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");
	if(XInputLibrary)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");;
	}
}


// TODO: This is a global for now
// global variables with static are initialized to 0
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right  - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return (Result);
};

internal void
RenderWeirdGradient(win32_offscreen_buffer* Buffer, int XOffset, int YOffset)
{
	// TODO: Let's see what the optimizer does
	uint8 *Row = (uint8*)Buffer->Memory;

	for (int Y = 0; Y < Buffer->Height; ++Y)
	{
		uint32* Pixel = (uint32*)Row;
		for (int X = 0; X < Buffer->Width; ++X)
		{
			/*
			 * Pixel in memory: 00 00 00 00
			 * */
			uint8 Blue  = (X + XOffset);
			uint8 Green = (Y + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row += Buffer->Pitch;
	}
}


internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	// TODO: Bulletproof this.
	// TODO: Free our DIBSection

	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width  = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;

	Buffer->Info.bmiHeader.biSize        = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth       = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight 	 = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes      = 1;
	Buffer->Info.bmiHeader.biBitCount    = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	Buffer->Pitch = Width*Buffer->BytesPerPixel;
}

internal void
Win32DisplayBufferInWindow(
		HDC DeviceContext,
		win32_offscreen_buffer *Buffer,
		int WindowWidth, int WindowHeight,
		int X, int Y, int Width, int Height)
{
	// TODO: Aspect ratio correction
	StretchDIBits(
			DeviceContext,
			0, 0, WindowWidth, WindowHeight,
			0, 0, Buffer->Width, Buffer->Height,
			Buffer->Memory,
			&Buffer->Info,
			DIB_RGB_COLORS, SRCCOPY);
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
		} break;

		case WM_CLOSE:
		{
			// TODO:: Handle this with a message to the user?
			GlobalRunning = false;
		} break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;

		case WM_DESTROY:
		{
			// TODO:: Handle this with an error
			GlobalRunning = false;
		} break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32 VKCode = WParam;
			bool WasDown = ((LParam & (1 << 30)) != 0);
			bool IsDown = ((LParam & (1 << 31)) == 0);

			if(WasDown != IsDown)
			{
				if(VKCode == 'W')
				{
				}
				else if(VKCode == 'A')
				{
				}
				else if(VKCode == 'S')
				{
				}
				else if(VKCode == 'D')
				{
				}
				else if(VKCode == VK_UP)
				{
				}
				else if(VKCode == VK_DOWN)
				{
				}
				else if(VKCode == VK_LEFT)
				{
				}
				else if(VKCode == VK_RIGHT)
				{
				}
				else if(VKCode == VK_SPACE)
				{
				}
				else if(VKCode == VK_ESCAPE)
				{
					OutputDebugStringA("ESCAPE: ");
					if(IsDown)
					{
						OutputDebugStringA("IsDown ");
					}
					if(WasDown)
					{
						OutputDebugStringA("WasDown");
					}
					OutputDebugStringA("\n");
				}
				else if(VKCode == 'Q')
				{
					OutputDebugStringA("Q\n");
				}
				else if(VKCode == 'E')
				{
					OutputDebugStringA("E\n");
				}
			}
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT PaintStructure;
			HDC DeviceContext = BeginPaint(Window, &PaintStructure);
			int X = PaintStructure.rcPaint.left;
			int Y = PaintStructure.rcPaint.top;
			int Width = PaintStructure.rcPaint.right - PaintStructure.rcPaint.left;
			int Height = PaintStructure.rcPaint.bottom - PaintStructure.rcPaint.top;

			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(DeviceContext, &GlobalBackbuffer,
					Dimension.Width, Dimension.Height,
					X, Y, Width, Height);
			EndPaint(Window, &PaintStructure);
		} break;
		default:
		{
			Result = DefWindowProcA(Window, Message, WParam, LParam);
		} break;
    }

	return Result;
}

int WINAPI
wWinMain(
		HINSTANCE Instance,
		HINSTANCE PrevInstance,
		PWSTR CommandLine,
		int CmdShow)
{
	Win32LoadXInput();

	// Define Window Class
    WNDCLASSA WindowClass = {0};
	Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 728);

    WindowClass.style       = CS_HREDRAW|CS_VREDRAW;
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
			GlobalRunning = true;
			int XOffset = 0;
			int YOffset = 0;

			while(GlobalRunning)
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				// NOTE: Controller Input:
				// Should we poll this more frequently
				// 2 methods. "Interupt" based input scheme and "Polling".
				for(DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
				{
					XINPUT_STATE ControllerState;
					if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
					{
						// NOTE: Controller is plugged in
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
						bool Up    = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down  = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left  = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back  = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder  = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;

						if(AButton)
						{
							YOffset += 2;
						}

					}
					else
					{
						// NOTE: Controller is not available
						//TODO: Logging info
					}
				}

				RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);
				HDC DeviceContext = GetDC(Window);

				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(
						DeviceContext, &GlobalBackbuffer, Dimension.Width, Dimension.Height,
						0, 0, Dimension.Width, Dimension.Height);

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
