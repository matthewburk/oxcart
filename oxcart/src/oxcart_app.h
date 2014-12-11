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

#ifndef OXCART_APP_H
#define OXCART_APP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OXCART_WINDOW_EVENT_CREATED            0x01
#define OXCART_WINDOW_EVENT_DESTROYED          0x02
#define OXCART_WINDOW_EVENT_ACTIVATED          0x03
#define OXCART_WINDOW_EVENT_DEACTIVATED        0x04
#define OXCART_WINDOW_EVENT_SHOWN              0x05
#define OXCART_WINDOW_EVENT_HIDDEN             0x06
#define OXCART_WINDOW_EVENT_MOVED              0x07
#define OXCART_WINDOW_EVENT_MAXIMIZED          0x08
#define OXCART_WINDOW_EVENT_MINIMIZED          0x09
#define OXCART_WINDOW_EVENT_RESIZED            0x0A
#define OXCART_WINDOW_STYLE_NOBORDER           0x01
#define OXCART_WINDOW_STYLE_BORDER             0x02
#define OXCART_COMMAND_CLOSE                   0x01
#define OXCART_GAMEPAD_BUTTON_NULL             0x00
#define OXCART_GAMEPAD_BUTTON_START            0x01
#define OXCART_GAMEPAD_BUTTON_BACK             0x02
#define OXCART_GAMEPAD_BUTTON_A                0x03
#define OXCART_GAMEPAD_BUTTON_B                0x04
#define OXCART_GAMEPAD_BUTTON_X                0x05
#define OXCART_GAMEPAD_BUTTON_Y                0x06
#define OXCART_GAMEPAD_BUTTON_DPAD_UP          0x07
#define OXCART_GAMEPAD_BUTTON_DPAD_DOWN        0x08
#define OXCART_GAMEPAD_BUTTON_DPAD_LEFT        0x09
#define OXCART_GAMEPAD_BUTTON_DPAD_RIGHT       0x0A
#define OXCART_GAMEPAD_BUTTON_SHOULDER_LEFT    0x0B
#define OXCART_GAMEPAD_BUTTON_SHOULDER_RIGHT   0x0C
#define OXCART_GAMEPAD_BUTTON_TRIGGER_LEFT     0x0D
#define OXCART_GAMEPAD_BUTTON_TRIGGER_RIGHT    0x0E
#define OXCART_GAMEPAD_BUTTON_THUMB_LEFT       0x0F
#define OXCART_GAMEPAD_BUTTON_LTHUMB_PRESS     0x10
#define OXCART_GAMEPAD_BUTTON_LTHUMB_UP        0x11
#define OXCART_GAMEPAD_BUTTON_LTHUMB_DOWN      0x12
#define OXCART_GAMEPAD_BUTTON_LTHUMB_LEFT      0x13
#define OXCART_GAMEPAD_BUTTON_LTHUMB_RIGHT     0x14
#define OXCART_GAMEPAD_BUTTON_LTHUMB_UPLEFT    0x15
#define OXCART_GAMEPAD_BUTTON_LTHUMB_UPRIGHT   0x16
#define OXCART_GAMEPAD_BUTTON_LTHUMB_DOWNLEFT  0x17
#define OXCART_GAMEPAD_BUTTON_LTHUMB_DOWNRIGHT 0x18
#define OXCART_GAMEPAD_BUTTON_THUMB_RIGHT      0x19
#define OXCART_GAMEPAD_BUTTON_RTHUMB_PRESS     0x1A
#define OXCART_GAMEPAD_BUTTON_RTHUMB_UP        0x1B
#define OXCART_GAMEPAD_BUTTON_RTHUMB_DOWN      0x1C
#define OXCART_GAMEPAD_BUTTON_RTHUMB_LEFT      0x1D
#define OXCART_GAMEPAD_BUTTON_RTHUMB_RIGHT     0x1E
#define OXCART_GAMEPAD_BUTTON_RTHUMB_UPLEFT    0x1F
#define OXCART_GAMEPAD_BUTTON_RTHUMB_UPRIGHT   0x20
#define OXCART_GAMEPAD_BUTTON_RTHUMB_DOWNLEFT  0x21
#define OXCART_GAMEPAD_BUTTON_RTHUMB_DOWNRIGHT 0x22
#define OXCART_GAMEPAD_DEADZONE_TRIGGER        30
#define OXCART_GAMEPAD_DEADZONE_LTHUMB         7849
#define OXCART_GAMEPAD_DEADZONE_RTHUMB         8689
#define OXCART_KEYBOARD_KEY_NULL               0x00
#define OXCART_KEYBOARD_KEY_BACKSPACE          0x01
#define OXCART_KEYBOARD_KEY_TAB                0x02
#define OXCART_KEYBOARD_KEY_ENTER              0x03
#define OXCART_KEYBOARD_KEY_SHIFT              0x04
#define OXCART_KEYBOARD_KEY_CTRL               0x05
#define OXCART_KEYBOARD_KEY_ALT                0x06
#define OXCART_KEYBOARD_KEY_PAUSE              0x07
#define OXCART_KEYBOARD_KEY_CAPSLOCK           0x08
#define OXCART_KEYBOARD_KEY_ESCAPE             0x09
#define OXCART_KEYBOARD_KEY_SPACE              0x0A
#define OXCART_KEYBOARD_KEY_PAGEUP             0x0B
#define OXCART_KEYBOARD_KEY_PAGEDOWN           0x0C
#define OXCART_KEYBOARD_KEY_END                0x0D
#define OXCART_KEYBOARD_KEY_HOME               0x0E
#define OXCART_KEYBOARD_KEY_LEFT               0x0F
#define OXCART_KEYBOARD_KEY_UP                 0x10
#define OXCART_KEYBOARD_KEY_RIGHT              0x11
#define OXCART_KEYBOARD_KEY_DOWN               0x12
#define OXCART_KEYBOARD_KEY_PRINTSCREEN        0x13
#define OXCART_KEYBOARD_KEY_INSERT             0x14
#define OXCART_KEYBOARD_KEY_DELETE             0x15
#define OXCART_KEYBOARD_KEY_0                  0x16
#define OXCART_KEYBOARD_KEY_1                  0x17
#define OXCART_KEYBOARD_KEY_2                  0x18
#define OXCART_KEYBOARD_KEY_3                  0x19
#define OXCART_KEYBOARD_KEY_4                  0x1A
#define OXCART_KEYBOARD_KEY_5                  0x1B
#define OXCART_KEYBOARD_KEY_6                  0x1C
#define OXCART_KEYBOARD_KEY_7                  0x1D
#define OXCART_KEYBOARD_KEY_8                  0x1E
#define OXCART_KEYBOARD_KEY_9                  0x1F
#define OXCART_KEYBOARD_KEY_A                  0x20
#define OXCART_KEYBOARD_KEY_B                  0x21
#define OXCART_KEYBOARD_KEY_C                  0x22
#define OXCART_KEYBOARD_KEY_D                  0x23
#define OXCART_KEYBOARD_KEY_E                  0x24
#define OXCART_KEYBOARD_KEY_F                  0x25
#define OXCART_KEYBOARD_KEY_G                  0x26
#define OXCART_KEYBOARD_KEY_H                  0x27
#define OXCART_KEYBOARD_KEY_I                  0x28
#define OXCART_KEYBOARD_KEY_J                  0x29
#define OXCART_KEYBOARD_KEY_K                  0x2A
#define OXCART_KEYBOARD_KEY_L                  0x2B
#define OXCART_KEYBOARD_KEY_M                  0x2C
#define OXCART_KEYBOARD_KEY_N                  0x2D
#define OXCART_KEYBOARD_KEY_O                  0x2E
#define OXCART_KEYBOARD_KEY_P                  0x2F
#define OXCART_KEYBOARD_KEY_Q                  0x30
#define OXCART_KEYBOARD_KEY_R                  0x31
#define OXCART_KEYBOARD_KEY_S                  0x32
#define OXCART_KEYBOARD_KEY_T                  0x33
#define OXCART_KEYBOARD_KEY_U                  0x34
#define OXCART_KEYBOARD_KEY_V                  0x35
#define OXCART_KEYBOARD_KEY_W                  0x36
#define OXCART_KEYBOARD_KEY_X                  0x37
#define OXCART_KEYBOARD_KEY_Y                  0x38
#define OXCART_KEYBOARD_KEY_Z                  0x39
#define OXCART_KEYBOARD_KEY_NUMPAD0            0x3A
#define OXCART_KEYBOARD_KEY_NUMPAD1            0x3B
#define OXCART_KEYBOARD_KEY_NUMPAD2            0x3C
#define OXCART_KEYBOARD_KEY_NUMPAD3            0x3D
#define OXCART_KEYBOARD_KEY_NUMPAD4            0x3E
#define OXCART_KEYBOARD_KEY_NUMPAD5            0x3F
#define OXCART_KEYBOARD_KEY_NUMPAD6            0x40
#define OXCART_KEYBOARD_KEY_NUMPAD7            0x41
#define OXCART_KEYBOARD_KEY_NUMPAD8            0x42
#define OXCART_KEYBOARD_KEY_NUMPAD9            0x43
#define OXCART_KEYBOARD_KEY_MULTIPLY           0x44
#define OXCART_KEYBOARD_KEY_ADD                0x45
#define OXCART_KEYBOARD_KEY_SUBTRACT           0x46
#define OXCART_KEYBOARD_KEY_DECIMAL            0x47
#define OXCART_KEYBOARD_KEY_DIVIDE             0x48
#define OXCART_KEYBOARD_KEY_F1                 0x49
#define OXCART_KEYBOARD_KEY_F2                 0x4A
#define OXCART_KEYBOARD_KEY_F3                 0x4B
#define OXCART_KEYBOARD_KEY_F4                 0x4C
#define OXCART_KEYBOARD_KEY_F5                 0x4D
#define OXCART_KEYBOARD_KEY_F6                 0x4E
#define OXCART_KEYBOARD_KEY_F7                 0x4F
#define OXCART_KEYBOARD_KEY_F8                 0x50
#define OXCART_KEYBOARD_KEY_F9                 0x51
#define OXCART_KEYBOARD_KEY_F10                0x52
#define OXCART_KEYBOARD_KEY_F11                0x53
#define OXCART_KEYBOARD_KEY_F12                0x54
#define OXCART_KEYBOARD_KEY_NUMLOCK            0x55
#define OXCART_KEYBOARD_KEY_SCROLLLOCK         0x56
#define OXCART_KEYBOARD_KEY_EQUAL              0x57
#define OXCART_KEYBOARD_KEY_COMMA              0x58
#define OXCART_KEYBOARD_KEY_MINUS              0x59
#define OXCART_KEYBOARD_KEY_PERIOD             0x5A
#define OXCART_KEYBOARD_KEY_SEMICOLON          0x5B
#define OXCART_KEYBOARD_KEY_SLASH              0x5C
#define OXCART_KEYBOARD_KEY_GRAVEACCENT        0x5D
#define OXCART_KEYBOARD_KEY_OPENBRACKET        0x5E
#define OXCART_KEYBOARD_KEY_BACKSLASH          0x5F
#define OXCART_KEYBOARD_KEY_CLOSEBRACKET       0x60
#define OXCART_KEYBOARD_KEY_APOSTROPHE         0x61
#define OXCART_MOUSE_BUTTON_LEFT               0x01
#define OXCART_MOUSE_BUTTON_MIDDLE             0x02
#define OXCART_MOUSE_BUTTON_RIGHT              0x04

void oxcart_window_close();
void oxcart_window_swap();
int oxcart_window_visible();
void oxcart_window_setvisible(int visible);
int oxcart_window_style();
void oxcart_window_setstyle(int style, int x, int y, int w, int h);
void oxcart_window_rect(int* x, int* y, int* w, int* h);
void oxcart_window_setrect(int x, int y, int w, int h);
int oxcart_window_vsync();
void oxcart_window_setvsync(int vsync);

int oxcart_monitor_count();
void oxcart_monitor_rect(int index, int* x, int* y, int* w, int* h);
void oxcart_monitor_dpi(int* x, int* y);

size_t oxcart_time_tick();
void oxcart_time_local(int* h, int* m, int* s, int* ms);
void oxcart_time_system(int* h, int* m, int* s, int* ms);

#ifdef __cplusplus
}
#endif

#endif
