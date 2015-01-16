/**
 * oxcart is licensed under the terms of the MIT license reproduced below.
 * 
 * Copyright © 2014 Thomas J. Schaefer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <xinput.h>
#include <physfs.h>
#include "oxcart_assert.h"
#include "oxcart_app.h"
#include "oxcart_delegate.h"
#include "oxcart_gl.h"
#include "oxcart_lua.h"
#include "oxcart_vector.h"
#include "oxcart_wgl.h"

#define OXCART_DEVICENAME_LENGTH 32
#define OXCART_WINDOW_CLASSNAME "oxcart_app_t"
#define OXCART_WM_CREATE (WM_APP + 1)

typedef struct oxcart_config_t oxcart_config_t;
typedef struct oxcart_monitor_t oxcart_monitor_t;
typedef struct oxcart_window_t oxcart_window_t;
typedef struct oxcart_module_t oxcart_module_t;

struct oxcart_config_t
{
  int w, h;
  int fullscreen;
  int vsync;
  int multisample;
  int nvpr;
};

struct oxcart_monitor_t
{
  char name[OXCART_DEVICENAME_LENGTH];
  int freq;
  int bits;
  int x, y, w, h;
};

struct oxcart_window_t
{
  HWND hwnd;
  HDC hdc;
  HGLRC hrc;
  HCURSOR hcursor;
  int tracking;
  int x, y, w, h;
};

struct oxcart_module_t
{
  DWORD start;
  DWORD pktnum;
  HMODULE hgl;
  oxcart_config_t config;
  oxcart_vector_t* monitors;
  oxcart_window_t window;
  XINPUT_GAMEPAD gamepad;
};

int oxcart_gl_extensions(int* major, int* minor);
int oxcart_wgl_extensions();
int oxcart_nvpr_extensions();

static void _module_loadconfig();
static void _monitor_initialize();
static void _monitor_terminate();
static int _monitor_add(const char* name, int primary);
static void _gamepad_poll();
static int _gamepad_translate(WORD button);
static void _window_initialize(const char* title, int style, int x, int y, int w, int h);
static void _window_terminate();
static int _window_poll();
static int _window_wait();
static void _window_dummy(const char* title, int* major, int* minor);
static LRESULT CALLBACK _window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static int _keyboard_translate(WPARAM key);

static oxcart_module_t _m = {0};

lua_State* L = 0;
int oxcartref = 0;

//1 - dir
//2 - args
static int open_oxcart_L(lua_State* L) {
	lua_settop(L, 2);

	//oxcart
	lua_newtable(L);
	lua_setglobal(L, "oxcart");

	lua_getglobal(L, "oxcart");
	oxcartref = luaL_ref(L, LUA_REGISTRYINDEX);

	//set oxcart.path
	lua_getglobal(L, "oxcart");
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "path");

	//oxcart.args
	lua_pushvalue(L, 2);
	lua_setfield(L, -2, "args");

	lua_settop(L, 0);

	lua_getglobal(L, "require");
	lua_pushliteral(L, "oxcart");
	lua_call(L, 1, 1);

	lua_call(L, 0, 0); //call function returned from require 'oxcart'

	return 0;
}

/**
 * 
 */
int main(int argc, char** argv)
{
  int x, y, w, h;
  TIMECAPS tcaps;
  size_t past;
  size_t now;
  size_t elapsed;

  timeGetDevCaps(&tcaps, sizeof(TIMECAPS));
  timeBeginPeriod(tcaps.wPeriodMin);
  _m.start = timeGetTime();

  if (!(_m.hgl = GetModuleHandle("opengl32.dll"))) {
    OXCART_ASSERT(!"GetModuleHandle() failed");
  }

  if (!PHYSFS_init(argv[0])) {
    OXCART_ASSERT(!"PHYSFS_init() failed");
  }

  if (!PHYSFS_mount("oxcart.zip", 0, 0)) {
    OXCART_ASSERT(!"PHYSFS_mount() failed");
  }

  _module_loadconfig();

  _monitor_initialize();
  oxcart_monitor_rect(0, &x, &y, &w, &h);

  if (_m.config.fullscreen) {
    _window_initialize("oxcart", OXCART_WINDOW_STYLE_NOBORDER, 0, 0, w, h);
  }
  else {
    _window_initialize("oxcart", OXCART_WINDOW_STYLE_BORDER, (w - _m.config.w) / 2, (h - _m.config.h) / 2, _m.config.w, _m.config.h);
  }

  L = oxcart_lua_newstate();

  {
	  int i;
	  static char oxcartname[MAX_PATH] = { 0 };
	  static char oxcartdir[MAX_PATH] = { 0 };

	  GetModuleFileName(0, oxcartname, sizeof(oxcartname));
	  strncpy(oxcartdir, oxcartname, strrchr(oxcartname, '\\') - oxcartname);

	  lua_pushcfunction(L, open_oxcart_L);
	  lua_pushstring(L, oxcartdir); //1 	  
	  lua_newtable(L);//2 args

	  for (i = 2; i < argc; i++) {
		  lua_pushstring(L, argv[i]);
		  lua_rawseti(L, -2, i - 1);
	  }

	  if (!oxcart_lua_pcall(L, 2, 0)) {
		  //ok
	  }
  }

  past = oxcart_time_tick();

  while (_window_poll()) {
    now = oxcart_time_tick();
    elapsed = now - past;
    past = now;

    _gamepad_poll();

    oxcart_delegate_update(elapsed);
  }

  _window_terminate();
  _monitor_terminate();
  PHYSFS_deinit();
  timeEndPeriod(tcaps.wPeriodMin);

  return(EXIT_SUCCESS);
}

/**
 * 
 */
void oxcart_window_close()
{
  PostQuitMessage(0);
}

/**
 * 
 */
void oxcart_window_swap()
{
  SwapBuffers(_m.window.hdc);
}

/**
 * 
 */
int oxcart_window_visible()
{
  return(IsWindowVisible(_m.window.hwnd));
}

/**
 * 
 */
void oxcart_window_setvisible(int visible)
{
  visible ? ShowWindow(_m.window.hwnd, SW_SHOW) : ShowWindow(_m.window.hwnd, SW_HIDE);
}

/**
 * 
 */
int oxcart_window_style()
{
  DWORD wndstyle;

  wndstyle = GetWindowLongPtr(_m.window.hwnd, GWL_STYLE);

  if (WS_BORDER & wndstyle) {
    return(OXCART_WINDOW_STYLE_BORDER);
  }

  return(OXCART_WINDOW_STYLE_NOBORDER);
}

/**
 * 
 */
void oxcart_window_setstyle(int style, int x, int y, int w, int h)
{
  int visible;
  DWORD wndstyle;
  RECT rect;

  OXCART_ASSERT(w > 0);
  OXCART_ASSERT(h > 0);

  visible = IsWindowVisible(_m.window.hwnd);

  if (visible) {
    ShowWindow(_m.window.hwnd, SW_HIDE);
  }

  switch (style)
  {
    case OXCART_WINDOW_STYLE_NOBORDER:
      wndstyle = WS_POPUP;
      break;

    case OXCART_WINDOW_STYLE_BORDER:
      wndstyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
      break;

    default:
      OXCART_ASSERT(!"Invalid window style");
      return;
  }

  SetRect(&rect, x, y, x + w, y + h);
  AdjustWindowRect(&rect, wndstyle, 0);
  SetWindowLongPtr(_m.window.hwnd, GWL_STYLE, wndstyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

  SetWindowPos(_m.window.hwnd, HWND_TOP,
               rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
               SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_FRAMECHANGED);

  if (visible) {
    ShowWindow(_m.window.hwnd, SW_SHOW);
  }
}

/**
 * 
 */
void oxcart_window_rect(int* x, int* y, int* w, int* h)
{
  OXCART_ASSERT(x);
  OXCART_ASSERT(y);
  OXCART_ASSERT(w);
  OXCART_ASSERT(h);

  *x = _m.window.x;
  *y = _m.window.y;
  *w = _m.window.w;
  *h = _m.window.h;
}

/**
 * 
 */
void oxcart_window_setrect(int x, int y, int w, int h)
{
  RECT rect;

  OXCART_ASSERT(w > 0);
  OXCART_ASSERT(h > 0);

  SetRect(&rect, x, y, x + w, y + h);
  AdjustWindowRect(&rect, GetWindowLongPtr(_m.window.hwnd, GWL_STYLE), 0);

  SetWindowPos(_m.window.hwnd, HWND_TOP,
               rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
               SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOCOPYBITS);
}

/**
* TODO remove once working as before
*/
typedef struct rect {
	int x;
	int y;
	int w;
	int h;
} rect_t;
/**
* TODO remove once working as before
*/
OXCART_API rect_t oxcart_get_window_rect()
{
	rect_t r;

	r.x = _m.window.x;
	r.y = _m.window.y;
	r.w = _m.window.w;
	r.h = _m.window.h;

	return r;
}

/**
 * 
 */
int oxcart_window_vsync()
{
  return(!!wglGetSwapIntervalEXT());
}

/**
 * 
 */
void oxcart_window_setvsync(int vsync)
{
  wglSwapIntervalEXT(!!vsync);
}

/**
 * 
 */
int oxcart_monitor_count()
{
  return(oxcart_vector_size(_m.monitors));
}

/**
 * 
 */
void oxcart_monitor_rect(int index, int* x, int* y, int* w, int* h)
{
  oxcart_monitor_t* monitor;

  OXCART_ASSERT(x);
  OXCART_ASSERT(y);
  OXCART_ASSERT(w);
  OXCART_ASSERT(h);

  monitor = (oxcart_monitor_t*)oxcart_vector_item(_m.monitors, index);

  *x = monitor->x;
  *y = monitor->y;
  *w = monitor->w;
  *h = monitor->h;
}

/**
 * 
 */
void oxcart_monitor_dpi(int* x, int* y)
{
  OXCART_ASSERT(x);
  OXCART_ASSERT(y);

  *x = GetDeviceCaps(_m.window.hdc, LOGPIXELSX);
  *y = GetDeviceCaps(_m.window.hdc, LOGPIXELSY);
}

/**
 * 
 */
size_t oxcart_time_tick()
{
  return(timeGetTime() - _m.start);
}

/**
 * 
 */
void oxcart_time_local(int* h, int* m, int* s, int* ms)
{
  SYSTEMTIME time;

  OXCART_ASSERT(h);
  OXCART_ASSERT(m);
  OXCART_ASSERT(s);
  OXCART_ASSERT(ms);

  GetLocalTime(&time);

  *h = time.wHour;
  *m = time.wMinute;
  *s = time.wSecond;
  *ms = time.wMilliseconds;
}

/**
 * 
 */
void oxcart_time_system(int* h, int* m, int* s, int* ms)
{
  SYSTEMTIME time;

  OXCART_ASSERT(h);
  OXCART_ASSERT(m);
  OXCART_ASSERT(s);
  OXCART_ASSERT(ms);

  GetSystemTime(&time);

  *h = time.wHour;
  *m = time.wMinute;
  *s = time.wSecond;
  *ms = time.wMilliseconds;
}

/**
 * 
 */
PROC oxcart_proc_address(const char* name)
{
  PROC address;

  OXCART_ASSERT(name);

  if (!(address = wglGetProcAddress(name))) {
    address = GetProcAddress(_m.hgl, name);
  }

  return(address);
}

/**
 * 
 */
static void _module_loadconfig()
{
  lua_State* L;

  L = oxcart_lua_newstate();

  if (OXCART_LUA_DOFILE(L, "/config/oxcart_app.lua")) {
    OXCART_ASSERT(!"OXCART_LUA_DOFILE() failed");
  }

  _m.config.w = oxcart_lua_tointeger(L, "oxcart_app.window.width");
  _m.config.h = oxcart_lua_tointeger(L, "oxcart_app.window.height");
  _m.config.fullscreen = oxcart_lua_toboolean(L, "oxcart_app.window.fullscreen");
  _m.config.vsync = oxcart_lua_toboolean(L, "oxcart_app.window.vsync");
  _m.config.multisample = oxcart_lua_tointeger(L, "oxcart_app.format.multisample");
  _m.config.nvpr = oxcart_lua_toboolean(L, "oxcart_app.extensions.nvpr");

  lua_close(L);
}

/**
 * 
 */
static void _monitor_initialize()
{
  int count;
  int adapter;
  int monitor;
  int primary;
  DISPLAY_DEVICE device;
  char name[OXCART_DEVICENAME_LENGTH];

  _m.monitors = oxcart_vector_create(sizeof(oxcart_monitor_t), 2);

  device.cb = sizeof(DISPLAY_DEVICE);

  for (adapter = 0; EnumDisplayDevices(0, adapter, &device, 0); adapter++) {
    if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
      continue;
    }

    memset(name, 0, sizeof(name));
    strncpy_s(name, OXCART_DEVICENAME_LENGTH, device.DeviceName, _TRUNCATE);
    primary = device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;

    count = 0;
    for (monitor = 0; EnumDisplayDevices(name, monitor, &device, 0); monitor++) {
      if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
        continue;
      }

      if (_monitor_add(device.DeviceName, device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
        count++;
      }
    }

    if (!count) {
      _monitor_add(name, primary);
    }
  }
}

/**
 * 
 */
static void _monitor_terminate()
{
  oxcart_vector_destroy(_m.monitors);
}

/**
 * 
 */
static int _monitor_add(const char* name, int primary)
{
  DEVMODE devmode;
  oxcart_monitor_t monitor;

  OXCART_ASSERT(name);

  devmode.dmSize = sizeof(DEVMODE);

  if (!EnumDisplaySettings(name, ENUM_CURRENT_SETTINGS, &devmode)) {
    return(0);
  }

  strncpy_s(monitor.name, OXCART_DEVICENAME_LENGTH, name, _TRUNCATE);
  monitor.freq = devmode.dmDisplayFrequency;
  monitor.bits = devmode.dmBitsPerPel;
  monitor.x = devmode.dmPosition.x;
  monitor.y = devmode.dmPosition.y;
  monitor.w = devmode.dmPelsWidth;
  monitor.h = devmode.dmPelsHeight;

  if (primary) {
    oxcart_vector_insert(_m.monitors, 0, &monitor);
  }
  else {
    oxcart_vector_pushback(_m.monitors, &monitor);
  }

  return(1);
}

/**
 * 
 */
static void _gamepad_poll()
{
  XINPUT_KEYSTROKE keystroke;
  XINPUT_STATE state;

  if (ERROR_SUCCESS == XInputGetKeystroke(0, 0, &keystroke)) {
    if (keystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN) {
      oxcart_delegate_gamepadbuttondown(_gamepad_translate(keystroke.VirtualKey), !!(keystroke.Flags & XINPUT_KEYSTROKE_REPEAT));
    }

    if (keystroke.Flags & XINPUT_KEYSTROKE_KEYUP) {
      oxcart_delegate_gamepadbuttonup(_gamepad_translate(keystroke.VirtualKey));
    }
  }

  if (ERROR_SUCCESS == XInputGetState(0, &state)) {
    if (_m.pktnum < state.dwPacketNumber) {
      if (_m.gamepad.bLeftTrigger != state.Gamepad.bLeftTrigger) {
        oxcart_delegate_gamepadtrigger(OXCART_GAMEPAD_BUTTON_TRIGGER_LEFT, state.Gamepad.bLeftTrigger);
      }

      if (_m.gamepad.bRightTrigger != state.Gamepad.bRightTrigger) {
        oxcart_delegate_gamepadtrigger(OXCART_GAMEPAD_BUTTON_TRIGGER_RIGHT, state.Gamepad.bRightTrigger);
      }

      if ((_m.gamepad.sThumbLX != state.Gamepad.sThumbLX) || (_m.gamepad.sThumbLY != state.Gamepad.sThumbLY)){
        oxcart_delegate_gamepadthumb(OXCART_GAMEPAD_BUTTON_THUMB_LEFT, state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
      }

      if ((_m.gamepad.sThumbRX != state.Gamepad.sThumbRX) || (_m.gamepad.sThumbRY != state.Gamepad.sThumbRY)){
        oxcart_delegate_gamepadthumb(OXCART_GAMEPAD_BUTTON_THUMB_RIGHT, state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
      }

      _m.pktnum = state.dwPacketNumber;
      _m.gamepad = state.Gamepad;
    }
  }
}

/**
 * 
 */
static int _gamepad_translate(WORD button)
{
  switch (button)
  {
    case VK_PAD_START:            return(OXCART_GAMEPAD_BUTTON_START);
    case VK_PAD_BACK:             return(OXCART_GAMEPAD_BUTTON_BACK);
    case VK_PAD_A:                return(OXCART_GAMEPAD_BUTTON_A);
    case VK_PAD_B:                return(OXCART_GAMEPAD_BUTTON_B);
    case VK_PAD_X:                return(OXCART_GAMEPAD_BUTTON_X);
    case VK_PAD_Y:                return(OXCART_GAMEPAD_BUTTON_Y);
    case VK_PAD_DPAD_UP:          return(OXCART_GAMEPAD_BUTTON_DPAD_UP);
    case VK_PAD_DPAD_DOWN:        return(OXCART_GAMEPAD_BUTTON_DPAD_DOWN);
    case VK_PAD_DPAD_LEFT:        return(OXCART_GAMEPAD_BUTTON_DPAD_LEFT);
    case VK_PAD_DPAD_RIGHT:       return(OXCART_GAMEPAD_BUTTON_DPAD_RIGHT);
    case VK_PAD_LSHOULDER:        return(OXCART_GAMEPAD_BUTTON_SHOULDER_LEFT);
    case VK_PAD_RSHOULDER:        return(OXCART_GAMEPAD_BUTTON_SHOULDER_RIGHT);
    case VK_PAD_LTRIGGER:         return(OXCART_GAMEPAD_BUTTON_TRIGGER_LEFT);
    case VK_PAD_RTRIGGER:         return(OXCART_GAMEPAD_BUTTON_TRIGGER_RIGHT);
    case VK_PAD_LTHUMB_PRESS:     return(OXCART_GAMEPAD_BUTTON_LTHUMB_PRESS);
    case VK_PAD_LTHUMB_UP:        return(OXCART_GAMEPAD_BUTTON_LTHUMB_UP);
    case VK_PAD_LTHUMB_DOWN:      return(OXCART_GAMEPAD_BUTTON_LTHUMB_DOWN);
    case VK_PAD_LTHUMB_LEFT:      return(OXCART_GAMEPAD_BUTTON_LTHUMB_LEFT);
    case VK_PAD_LTHUMB_RIGHT:     return(OXCART_GAMEPAD_BUTTON_LTHUMB_RIGHT);
    case VK_PAD_LTHUMB_UPLEFT:    return(OXCART_GAMEPAD_BUTTON_LTHUMB_UPLEFT);
    case VK_PAD_LTHUMB_UPRIGHT:   return(OXCART_GAMEPAD_BUTTON_LTHUMB_UPRIGHT);
    case VK_PAD_LTHUMB_DOWNLEFT:  return(OXCART_GAMEPAD_BUTTON_LTHUMB_DOWNLEFT);
    case VK_PAD_LTHUMB_DOWNRIGHT: return(OXCART_GAMEPAD_BUTTON_LTHUMB_DOWNRIGHT);
    case VK_PAD_RTHUMB_PRESS:     return(OXCART_GAMEPAD_BUTTON_RTHUMB_PRESS);
    case VK_PAD_RTHUMB_UP:        return(OXCART_GAMEPAD_BUTTON_RTHUMB_UP);
    case VK_PAD_RTHUMB_DOWN:      return(OXCART_GAMEPAD_BUTTON_RTHUMB_DOWN);
    case VK_PAD_RTHUMB_LEFT:      return(OXCART_GAMEPAD_BUTTON_RTHUMB_LEFT);
    case VK_PAD_RTHUMB_RIGHT:     return(OXCART_GAMEPAD_BUTTON_RTHUMB_RIGHT);
    case VK_PAD_RTHUMB_UPLEFT:    return(OXCART_GAMEPAD_BUTTON_RTHUMB_UPLEFT);
    case VK_PAD_RTHUMB_UPRIGHT:   return(OXCART_GAMEPAD_BUTTON_RTHUMB_UPRIGHT);
    case VK_PAD_RTHUMB_DOWNLEFT:  return(OXCART_GAMEPAD_BUTTON_RTHUMB_DOWNLEFT);
    case VK_PAD_RTHUMB_DOWNRIGHT: return(OXCART_GAMEPAD_BUTTON_RTHUMB_DOWNRIGHT);
  }

  return(OXCART_GAMEPAD_BUTTON_NULL);
}

/**
 * 
 */
static void _window_initialize(const char* title, int style, int x, int y, int w, int h)
{
  int i;
  int attribs[23];
  int format;
  int major, minor;
  unsigned int count;
  HWND hwnd;
  HDC hdc;
  HGLRC hrc;
  WNDCLASS wndclass = {0};
  DWORD wndstyle;
  RECT rect;
  RAWINPUTDEVICE rid;
  PIXELFORMATDESCRIPTOR pfd;

  OXCART_ASSERT(title);
  OXCART_ASSERT(w > 0);
  OXCART_ASSERT(h > 0);

  if (!GetClassInfo(GetModuleHandle(0), OXCART_WINDOW_CLASSNAME, &wndclass)) {
    wndclass.style = CS_OWNDC | CS_DBLCLKS;
    wndclass.lpfnWndProc = _window_proc;
    wndclass.hInstance = GetModuleHandle(0);
    wndclass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wndclass.hbrBackground = (HBRUSH)0;
    wndclass.lpszClassName = OXCART_WINDOW_CLASSNAME;

    if (!RegisterClass(&wndclass)) {
      OXCART_ASSERT(!"RegisterClass() failed");
    }
  }

  _window_dummy(title, &major, &minor);

  switch (style)
  {
    case OXCART_WINDOW_STYLE_NOBORDER:
      wndstyle = WS_POPUP;
      break;

    case OXCART_WINDOW_STYLE_BORDER:
      wndstyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
      break;

    default:
      OXCART_ASSERT(!"Invalid window style");
      return;
  }

  SetRect(&rect, x, y, x + w, y + h);
  AdjustWindowRect(&rect, wndstyle, 0);

  if (!(hwnd = CreateWindow(OXCART_WINDOW_CLASSNAME, title,
                            wndstyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                            0, 0, GetModuleHandle(0), 0))) {
    OXCART_ASSERT(!"CreateWindow() failed");
  }

  rid.usUsagePage = 0x01;
  rid.usUsage = 0x02; /* 0x02:mouse, 0x06:keyboard */
  rid.dwFlags = 0;
  rid.hwndTarget = hwnd;

  if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
    OXCART_ASSERT(!"RegisterRawInputDevices() failed");
  }

  if (!(hdc = GetDC(hwnd))) {
    OXCART_ASSERT(!"GetDC() failed");
  }

  i = 0;
  attribs[i++] = WGL_SUPPORT_OPENGL_ARB;
  attribs[i++] = 1;
  attribs[i++] = WGL_DRAW_TO_WINDOW_ARB;
  attribs[i++] = 1;
  attribs[i++] = WGL_DOUBLE_BUFFER_ARB;
  attribs[i++] = 1;
  attribs[i++] = WGL_SAMPLE_BUFFERS_ARB;
  attribs[i++] = 1;
  attribs[i++] = WGL_SAMPLES_ARB;
  attribs[i++] = _m.config.multisample;
  attribs[i++] = WGL_SWAP_METHOD_ARB;
  attribs[i++] = WGL_SWAP_EXCHANGE_ARB;
  attribs[i++] = WGL_ACCELERATION_ARB;
  attribs[i++] = WGL_FULL_ACCELERATION_ARB;
  attribs[i++] = WGL_COLOR_BITS_ARB;
  attribs[i++] = 32;
  attribs[i++] = WGL_DEPTH_BITS_ARB;
  attribs[i++] = 24;
  attribs[i++] = WGL_STENCIL_BITS_ARB;
  attribs[i++] = 8;
  attribs[i++] = WGL_PIXEL_TYPE_ARB;
  attribs[i++] = WGL_TYPE_RGBA_ARB;
  attribs[i++] = 0;

  if (!(wglChoosePixelFormatARB(hdc, attribs, 0, 1, &format, &count))) {
    OXCART_ASSERT(!"wglChoosePixelFormatARB() failed");
  }

  if (!(SetPixelFormat(hdc, format, &pfd))) {
    OXCART_ASSERT(!"SetPixelFormat() failed");
  }

  i = 0;
  attribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
  attribs[i++] = major;
  attribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
  attribs[i++] = minor;
  attribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
  attribs[i++] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
  attribs[i++] = 0;

  if (!(hrc = wglCreateContextAttribsARB(hdc, 0, attribs))) {
    OXCART_ASSERT(!"wglCreateContextAttribsARB() failed");
  }

  wglMakeCurrent(hdc, hrc);

  _m.window.hwnd = hwnd;
  _m.window.hdc = hdc;
  _m.window.hrc = hrc;
  _m.window.hcursor = LoadCursor(0, IDC_ARROW);
  _m.window.x = x;
  _m.window.y = y;
  _m.window.w = w;
  _m.window.h = h;

  SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&_m.window);
  SendMessage(hwnd, OXCART_WM_CREATE, 0, 0);
}

/**
 * 
 */
static void _window_terminate()
{
  wglMakeCurrent(0, 0);
  wglDeleteContext(_m.window.hrc);
  DestroyWindow(_m.window.hwnd);
}

/**
 * 
 */
static int _window_poll()
{
  MSG msg;

  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    if (WM_QUIT == msg.message) {
      return(0);
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return(1);
}

/**
 * 
 */
static int _window_wait()
{
  MSG msg;

  while (0 < GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    return(_window_poll());
  }

  return(0);
}

/**
 * 
 */
static void _window_dummy(const char* title, int* major, int* minor)
{
  int format;
  HWND hwnd;
  HDC hdc;
  HGLRC hrc;
  PIXELFORMATDESCRIPTOR pfd;

  OXCART_ASSERT(title);
  OXCART_ASSERT(major);
  OXCART_ASSERT(minor);

  if (!(hwnd = CreateWindow(OXCART_WINDOW_CLASSNAME, title,
                            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                            0, 0, GetModuleHandle(0), 0))) {
    OXCART_ASSERT(!"CreateWindow() failed");
  }

  if (!(hdc = GetDC(hwnd))) {
    OXCART_ASSERT(!"GetDC() failed");
  }

  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cRedBits = 8;
  pfd.cGreenBits = 8;
  pfd.cBlueBits = 8;
  pfd.cAlphaBits = 8;
  pfd.cDepthBits = 24;
  pfd.iLayerType = PFD_MAIN_PLANE;

  if (!(format = ChoosePixelFormat(hdc, &pfd))) {
    OXCART_ASSERT(!"ChoosePixelFormat() failed");
  }

  if (!SetPixelFormat(hdc, format, &pfd)) {
    OXCART_ASSERT(!"SetPixelFormat() failed");
  }

  if (!(hrc = wglCreateContext(hdc))) {
    OXCART_ASSERT(!"wglCreateContext() failed");
  }

  wglMakeCurrent(hdc, hrc);

  if (OXCART_GL_VERSION_42 > oxcart_gl_extensions(major, minor)) {
    OXCART_ASSERT(!"OpenGL 4.2+ required");
  }

  if (!oxcart_wgl_extensions()) {
    OXCART_ASSERT(!"Failed to load wgl extensions");
  }

  if (_m.config.nvpr) {
    if (!oxcart_nvpr_extensions()) {
      OXCART_ASSERT(!"Failed to load nvpr extensions");
    }
  }

  wglMakeCurrent(0, 0);
  wglDeleteContext(hrc);

  DestroyWindow(hwnd);
}

/**
 * 
 */
static LRESULT CALLBACK _window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  oxcart_window_t* window;

  if (!(window = (oxcart_window_t*)((LONG_PTR)GetWindowLongPtr(hwnd, GWLP_USERDATA)))) {
    return(DefWindowProc(hwnd, msg, wparam, lparam));
  }

  switch (msg)
  {
    case OXCART_WM_CREATE:
    {
      oxcart_window_setvsync(_m.config.vsync);
      oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_CREATED);
      return(0);
    }

    case WM_DESTROY:
    {
      oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_DESTROYED);
      return(0);
    }

    case WM_ACTIVATEAPP:
    {
      if (wparam) {
        SetForegroundWindow(window->hwnd);
        LockSetForegroundWindow(LSFW_LOCK);
        XInputEnable(1);
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_ACTIVATED);
      }
      else {
        XInputEnable(0);
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_DEACTIVATED);
      }

      return(0);
    }

    case WM_CLOSE:
    {
      if (!oxcart_delegate_command(OXCART_COMMAND_CLOSE)) {
        oxcart_window_close();
      }

      return(0);
    }

    case WM_SHOWWINDOW:
    {
      if (wparam) {
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_SHOWN);
      }
      else {
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_HIDDEN);
      }

      return(0);
    }

    case WM_MOVE:
    {
      if (!IsIconic(window->hwnd)) {
        window->x = (int)(short)LOWORD(lparam);
        window->y = (int)(short)HIWORD(lparam);
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_MOVED);
      }

      return(0);
    }

    case WM_SIZE:
    {
      window->w = LOWORD(lparam);
      window->h = HIWORD(lparam);

      if (SIZE_MAXIMIZED == wparam) {
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_MAXIMIZED);
      }
      else if (SIZE_MINIMIZED == wparam) {
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_MINIMIZED);
      }
      else {
        oxcart_delegate_windowevent(OXCART_WINDOW_EVENT_RESIZED);
      }

      return(0);
    }

    case WM_PAINT:
    {
      if (GetUpdateRect(window->hwnd, 0, 0)) {
        ValidateRect(window->hwnd, 0);
      }

      return(0);
    }

    case WM_ERASEBKGND:
    {
      return(1);
    }

    case WM_GETMINMAXINFO:
    {
      ((MINMAXINFO*)lparam)->ptMinTrackSize.x = 100;
      ((MINMAXINFO*)lparam)->ptMinTrackSize.y = 100;
      return(0);
    }

    case WM_CHAR:
    {
      oxcart_delegate_char((char)wparam);
      return(0);
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
      oxcart_delegate_keydown(_keyboard_translate(wparam),
                              (KF_REPEAT & HIWORD(lparam)) ? 1 : 0,
                              GetKeyState(VK_MENU) & 0x8000,
                              GetKeyState(VK_CONTROL) & 0x8000,
                              GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
      oxcart_delegate_keyup(_keyboard_translate(wparam),
                            GetKeyState(VK_MENU) & 0x8000,
                            GetKeyState(VK_CONTROL) & 0x8000,
                            GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_INPUT:
    {
      RAWINPUT raw;
      UINT size = sizeof(RAWINPUT);
      GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

      oxcart_delegate_mousemoveraw(raw.data.mouse.lLastX, raw.data.mouse.lLastY,
                                   GetKeyState(VK_MENU) & 0x8000,
                                   GetKeyState(VK_CONTROL) & 0x8000,
                                   GetKeyState(VK_SHIFT) & 0x8000);

      return(0);
    }

    case WM_MOUSEMOVE:
    {
      SetCursor(window->hcursor);

      if (!window->tracking) {
        TRACKMOUSEEVENT tme = {0};
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = window->hwnd;
        tme.dwHoverTime = HOVER_DEFAULT;
        TrackMouseEvent(&tme);

        window->tracking = 1;
        oxcart_delegate_mouseenter();
      }

      oxcart_delegate_mousemove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                GetKeyState(VK_MENU) & 0x8000,
                                GetKeyState(VK_CONTROL) & 0x8000,
                                GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_MOUSELEAVE:
    {
      window->tracking = 0;
      oxcart_delegate_mouseexit();
      return(0);
    }

    case WM_LBUTTONDBLCLK:
    {
      oxcart_delegate_mousebuttondblclick(OXCART_MOUSE_BUTTON_LEFT,
                                          GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                          GetKeyState(VK_MENU) & 0x8000,
                                          GetKeyState(VK_CONTROL) & 0x8000,
                                          GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_LBUTTONDOWN:
    {
      SetCapture(window->hwnd);

      oxcart_delegate_mousebuttondown(OXCART_MOUSE_BUTTON_LEFT,
                                      GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                      GetKeyState(VK_MENU) & 0x8000,
                                      GetKeyState(VK_CONTROL) & 0x8000,
                                      GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_LBUTTONUP:
    {
      ReleaseCapture();

      oxcart_delegate_mousebuttonup(OXCART_MOUSE_BUTTON_LEFT,
                                    GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                    GetKeyState(VK_MENU) & 0x8000,
                                    GetKeyState(VK_CONTROL) & 0x8000,
                                    GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_MBUTTONDBLCLK:
    {
      oxcart_delegate_mousebuttondblclick(OXCART_MOUSE_BUTTON_MIDDLE,
                                          GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                          GetKeyState(VK_MENU) & 0x8000,
                                          GetKeyState(VK_CONTROL) & 0x8000,
                                          GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_MBUTTONDOWN:
    {
      SetCapture(window->hwnd);

      oxcart_delegate_mousebuttondown(OXCART_MOUSE_BUTTON_MIDDLE,
                                      GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                      GetKeyState(VK_MENU) & 0x8000,
                                      GetKeyState(VK_CONTROL) & 0x8000,
                                      GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_MBUTTONUP:
    {
      ReleaseCapture();

      oxcart_delegate_mousebuttonup(OXCART_MOUSE_BUTTON_MIDDLE,
                                    GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                    GetKeyState(VK_MENU) & 0x8000,
                                    GetKeyState(VK_CONTROL) & 0x8000,
                                    GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_RBUTTONDBLCLK:
    {
      oxcart_delegate_mousebuttondblclick(OXCART_MOUSE_BUTTON_RIGHT,
                                          GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                          GetKeyState(VK_MENU) & 0x8000,
                                          GetKeyState(VK_CONTROL) & 0x8000,
                                          GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_RBUTTONDOWN:
    {
      SetCapture(window->hwnd);

      oxcart_delegate_mousebuttondown(OXCART_MOUSE_BUTTON_RIGHT,
                                      GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                      GetKeyState(VK_MENU) & 0x8000,
                                      GetKeyState(VK_CONTROL) & 0x8000,
                                      GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_RBUTTONUP:
    {
      ReleaseCapture();

      oxcart_delegate_mousebuttonup(OXCART_MOUSE_BUTTON_RIGHT,
                                    GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam),
                                    GetKeyState(VK_MENU) & 0x8000,
                                    GetKeyState(VK_CONTROL) & 0x8000,
                                    GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }

    case WM_MOUSEWHEEL:
    {
      POINT point;
      UINT step;

      point.x = GET_X_LPARAM(lparam);
      point.y = GET_Y_LPARAM(lparam);
      ScreenToClient(window->hwnd, &point);
      SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &step, 0);

      oxcart_delegate_mousewheelscroll(GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA,
                                       step,
                                       point.x, point.y,
                                       GetKeyState(VK_MENU) & 0x8000,
                                       GetKeyState(VK_CONTROL) & 0x8000,
                                       GetKeyState(VK_SHIFT) & 0x8000);
      return(0);
    }
  }

  return(DefWindowProc(hwnd, msg, wparam, lparam));
}

/**
 * 
 */
static int _keyboard_translate(WPARAM key)
{
  switch (key)
  {
    case VK_BACK:       return(OXCART_KEYBOARD_KEY_BACKSPACE);
    case VK_TAB:        return(OXCART_KEYBOARD_KEY_TAB);
    case VK_RETURN:     return(OXCART_KEYBOARD_KEY_ENTER);
    case VK_SHIFT:      return(OXCART_KEYBOARD_KEY_SHIFT);
    case VK_CONTROL:    return(OXCART_KEYBOARD_KEY_CTRL);
    case VK_MENU:       return(OXCART_KEYBOARD_KEY_ALT);
    case VK_PAUSE:      return(OXCART_KEYBOARD_KEY_PAUSE);
    case VK_CAPITAL:    return(OXCART_KEYBOARD_KEY_CAPSLOCK);
    case VK_ESCAPE:     return(OXCART_KEYBOARD_KEY_ESCAPE);
    case VK_SPACE:      return(OXCART_KEYBOARD_KEY_SPACE);
    case VK_PRIOR:      return(OXCART_KEYBOARD_KEY_PAGEUP);
    case VK_NEXT:       return(OXCART_KEYBOARD_KEY_PAGEDOWN);
    case VK_END:        return(OXCART_KEYBOARD_KEY_END);
    case VK_HOME:       return(OXCART_KEYBOARD_KEY_HOME);
    case VK_LEFT:       return(OXCART_KEYBOARD_KEY_LEFT);
    case VK_UP:         return(OXCART_KEYBOARD_KEY_UP);
    case VK_RIGHT:      return(OXCART_KEYBOARD_KEY_RIGHT);
    case VK_DOWN:       return(OXCART_KEYBOARD_KEY_DOWN);
    case VK_SNAPSHOT:   return(OXCART_KEYBOARD_KEY_PRINTSCREEN);
    case VK_INSERT:     return(OXCART_KEYBOARD_KEY_INSERT);
    case VK_DELETE:     return(OXCART_KEYBOARD_KEY_DELETE);
    case 0x30:          return(OXCART_KEYBOARD_KEY_0);
    case 0x31:          return(OXCART_KEYBOARD_KEY_1);
    case 0x32:          return(OXCART_KEYBOARD_KEY_2);
    case 0x33:          return(OXCART_KEYBOARD_KEY_3);
    case 0x34:          return(OXCART_KEYBOARD_KEY_4);
    case 0x35:          return(OXCART_KEYBOARD_KEY_5);
    case 0x36:          return(OXCART_KEYBOARD_KEY_6);
    case 0x37:          return(OXCART_KEYBOARD_KEY_7);
    case 0x38:          return(OXCART_KEYBOARD_KEY_8);
    case 0x39:          return(OXCART_KEYBOARD_KEY_9);
    case 0x41:          return(OXCART_KEYBOARD_KEY_A);
    case 0x42:          return(OXCART_KEYBOARD_KEY_B);
    case 0x43:          return(OXCART_KEYBOARD_KEY_C);
    case 0x44:          return(OXCART_KEYBOARD_KEY_D);
    case 0x45:          return(OXCART_KEYBOARD_KEY_E);
    case 0x46:          return(OXCART_KEYBOARD_KEY_F);
    case 0x47:          return(OXCART_KEYBOARD_KEY_G);
    case 0x48:          return(OXCART_KEYBOARD_KEY_H);
    case 0x49:          return(OXCART_KEYBOARD_KEY_I);
    case 0x4A:          return(OXCART_KEYBOARD_KEY_J);
    case 0x4B:          return(OXCART_KEYBOARD_KEY_K);
    case 0x4C:          return(OXCART_KEYBOARD_KEY_L);
    case 0x4D:          return(OXCART_KEYBOARD_KEY_M);
    case 0x4E:          return(OXCART_KEYBOARD_KEY_N);
    case 0x4F:          return(OXCART_KEYBOARD_KEY_O);
    case 0x50:          return(OXCART_KEYBOARD_KEY_P);
    case 0x51:          return(OXCART_KEYBOARD_KEY_Q);
    case 0x52:          return(OXCART_KEYBOARD_KEY_R);
    case 0x53:          return(OXCART_KEYBOARD_KEY_S);
    case 0x54:          return(OXCART_KEYBOARD_KEY_T);
    case 0x55:          return(OXCART_KEYBOARD_KEY_U);
    case 0x56:          return(OXCART_KEYBOARD_KEY_V);
    case 0x57:          return(OXCART_KEYBOARD_KEY_W);
    case 0x58:          return(OXCART_KEYBOARD_KEY_X);
    case 0x59:          return(OXCART_KEYBOARD_KEY_Y);
    case 0x5A:          return(OXCART_KEYBOARD_KEY_Z);
    case VK_NUMPAD0:    return(OXCART_KEYBOARD_KEY_NUMPAD0);
    case VK_NUMPAD1:    return(OXCART_KEYBOARD_KEY_NUMPAD1);
    case VK_NUMPAD2:    return(OXCART_KEYBOARD_KEY_NUMPAD2);
    case VK_NUMPAD3:    return(OXCART_KEYBOARD_KEY_NUMPAD3);
    case VK_NUMPAD4:    return(OXCART_KEYBOARD_KEY_NUMPAD4);
    case VK_NUMPAD5:    return(OXCART_KEYBOARD_KEY_NUMPAD5);
    case VK_NUMPAD6:    return(OXCART_KEYBOARD_KEY_NUMPAD6);
    case VK_NUMPAD7:    return(OXCART_KEYBOARD_KEY_NUMPAD7);
    case VK_NUMPAD8:    return(OXCART_KEYBOARD_KEY_NUMPAD8);
    case VK_NUMPAD9:    return(OXCART_KEYBOARD_KEY_NUMPAD9);
    case VK_MULTIPLY:   return(OXCART_KEYBOARD_KEY_MULTIPLY);
    case VK_ADD:        return(OXCART_KEYBOARD_KEY_ADD);
    case VK_SUBTRACT:   return(OXCART_KEYBOARD_KEY_SUBTRACT);
    case VK_DECIMAL:    return(OXCART_KEYBOARD_KEY_DECIMAL);
    case VK_DIVIDE:     return(OXCART_KEYBOARD_KEY_DIVIDE);
    case VK_F1:         return(OXCART_KEYBOARD_KEY_F1);
    case VK_F2:         return(OXCART_KEYBOARD_KEY_F2);
    case VK_F3:         return(OXCART_KEYBOARD_KEY_F3);
    case VK_F4:         return(OXCART_KEYBOARD_KEY_F4);
    case VK_F5:         return(OXCART_KEYBOARD_KEY_F5);
    case VK_F6:         return(OXCART_KEYBOARD_KEY_F6);
    case VK_F7:         return(OXCART_KEYBOARD_KEY_F7);
    case VK_F8:         return(OXCART_KEYBOARD_KEY_F8);
    case VK_F9:         return(OXCART_KEYBOARD_KEY_F9);
    case VK_F10:        return(OXCART_KEYBOARD_KEY_F10);
    case VK_F11:        return(OXCART_KEYBOARD_KEY_F11);
    case VK_F12:        return(OXCART_KEYBOARD_KEY_F12);
    case VK_NUMLOCK:    return(OXCART_KEYBOARD_KEY_NUMLOCK);
    case VK_SCROLL:     return(OXCART_KEYBOARD_KEY_SCROLLLOCK);
    case VK_OEM_PLUS:   return(OXCART_KEYBOARD_KEY_EQUAL);
    case VK_OEM_COMMA:  return(OXCART_KEYBOARD_KEY_COMMA);
    case VK_OEM_MINUS:  return(OXCART_KEYBOARD_KEY_MINUS);
    case VK_OEM_PERIOD: return(OXCART_KEYBOARD_KEY_PERIOD);
    case VK_OEM_1:      return(OXCART_KEYBOARD_KEY_SEMICOLON);
    case VK_OEM_2:      return(OXCART_KEYBOARD_KEY_SLASH);
    case VK_OEM_3:      return(OXCART_KEYBOARD_KEY_GRAVEACCENT);
    case VK_OEM_4:      return(OXCART_KEYBOARD_KEY_OPENBRACKET);
    case VK_OEM_5:      return(OXCART_KEYBOARD_KEY_BACKSLASH);
    case VK_OEM_6:      return(OXCART_KEYBOARD_KEY_CLOSEBRACKET);
    case VK_OEM_7:      return(OXCART_KEYBOARD_KEY_APOSTROPHE);
  }

  return(OXCART_KEYBOARD_KEY_NULL);
}

