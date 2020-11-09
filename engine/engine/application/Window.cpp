#include "Window.hpp"
#include "engine/platform/win.hpp"
////////////////////////////////////////////////////////////////
//////////////////////////// Define ////////////////////////////
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//////////////////////////// Static ////////////////////////////
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
/////////////////////// Standalone Function /////////////////////
////////////////////////////////////////////////////////////////

static LRESULT CALLBACK gameWndProc(HWND hwnd,
   UINT msg,
   WPARAM wparam,
   LPARAM lparam) {


   return ::DefWindowProc(hwnd, msg, wparam, lparam);
}



////////////////////////////////////////////////////////////////
///////////////////////// Member Function //////////////////////
////////////////////////////////////////////////////////////////

void Window::Init(uint2 pixelSize, std::string_view name)
{
   //////////////////////// Create Window ////////////////////////
   // Define a window style/class
   WNDCLASSEX windowClassDescription;
   memset(&windowClassDescription, 0, sizeof(windowClassDescription));
   windowClassDescription.cbSize = sizeof(windowClassDescription);
   windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
   windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(gameWndProc);
   // Register our Windows message-handling function
   windowClassDescription.hInstance = GetModuleHandle(NULL);
   windowClassDescription.hIcon = NULL;
   windowClassDescription.hCursor = NULL;
   windowClassDescription.lpszClassName = L"Simple Window Class";
   RegisterClassEx(&windowClassDescription);

   float clientAspect = (float)pixelSize.x / (float)pixelSize.y;

   const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
   const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

   // Get desktop rect, dimensions, aspect
   RECT desktopRect;
   HWND desktopWindowHandle = GetDesktopWindow();
   GetClientRect(desktopWindowHandle, &desktopRect);
   float desktopWidth = (float)(desktopRect.right - desktopRect.left);
   float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
   float desktopAspect = desktopWidth / desktopHeight;

   // Calculate maximum client size (as some % of desktop size)
   constexpr float maxClientFractionOfDesktop = 0.90f;
   float           clientWidth = desktopWidth * maxClientFractionOfDesktop;
   float           clientHeight = desktopHeight * maxClientFractionOfDesktop;
   if (clientAspect > desktopAspect) {
      // Client window has a wider aspect than desktop; shrink client height to match its width
      clientHeight = clientWidth / clientAspect;
   }
   else {
      // Client window has a taller aspect than desktop; shrink client width to match its height
      clientWidth = clientHeight * clientAspect;
   }

   // Calculate client rect bounds by centering the client area
   float clientMarginX = 0.5f * (desktopWidth - clientWidth);
   float clientMarginY = 0.5f * (desktopHeight - clientHeight);
   RECT  clientRect;
   clientRect.left = (int)clientMarginX;
   clientRect.right = clientRect.left + (int)clientWidth;
   clientRect.top = (int)clientMarginY;
   clientRect.bottom = clientRect.top + (int)clientHeight;

   // Calculate the outer dimensions of the physical window, including frame et. al.
   RECT windowRect = clientRect;
   AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

   mWindowSize = { uint32_t(windowRect.right - windowRect.left),
                   uint32_t(windowRect.bottom - windowRect.top) };

   WCHAR windowTitle[1024];
   MultiByteToWideChar(GetACP(), 0, name.data(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));

   HINSTANCE applicationInstanceHandle = GetModuleHandle(NULL);

   mHandle = CreateWindowEx(
      windowStyleExFlags,
      L"Simple Window Class",
      windowTitle,
      windowStyleFlags,
      windowRect.left,
      windowRect.top,
      mWindowSize.x,
      mWindowSize.y,
      NULL,
      NULL,
      applicationInstanceHandle,
      NULL);

   HWND wnd = (HWND)mHandle;
   ShowWindow(wnd, SW_SHOW);
   SetForegroundWindow(wnd);
   SetFocus(wnd);

   HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
   SetCursor(cursor);

   //////////////////////// Create Factory/SwapChain ////////////////////////
   mWindowData = new WindowData;
   uint dxgiFlags = 0;
#if defined(_DEBUG)
   // enable debug layer for debug mode.
   // have to do this step before create device or it will inavalidate the active device
   ID3D12Debug* debugLayer;
   if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
      debugLayer->EnableDebugLayer();

      dxgiFlags |= DXGI_CREATE_FACTORY_DEBUG;
   }
#endif

   assert_win(CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&mWindowData->dxgiFactory)));
   BOOL    allowTearing = FALSE;
   HRESULT hr = mWindowData->dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
      &allowTearing,
      sizeof(allowTearing));
   mAllowTearing = allowTearing && SUCCEEDED(hr);
}
