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

#ifndef OXCART_DELEGATE_H
#define OXCART_DELEGATE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void oxcart_delegate_update(size_t elapsed);
void oxcart_delegate_windowevent(int event);
int oxcart_delegate_command(int command);
void oxcart_delegate_char(char c);
void oxcart_delegate_keydown(int key, int repeat, int alt, int ctrl, int shift);
void oxcart_delegate_keyup(int key, int alt, int ctrl, int shift);
void oxcart_delegate_mouseenter();
void oxcart_delegate_mouseexit();
void oxcart_delegate_mousemove(int x, int y, int alt, int ctrl, int shift);
void oxcart_delegate_mousebuttondblclick(int button, int x, int y, int alt, int ctrl, int shift);
void oxcart_delegate_mousebuttondown(int button, int x, int y, int alt, int ctrl, int shift);
void oxcart_delegate_mousebuttonup(int button, int x, int y, int alt, int ctrl, int shift);
void oxcart_delegate_mousewheelscroll(int delta, int step, int x, int y, int alt, int ctrl, int shift);
void oxcart_delegate_gamepadbuttondown(int button, int repeat);
void oxcart_delegate_gamepadbuttonup(int button);
void oxcart_delegate_gamepadtrigger(int button, int z);
void oxcart_delegate_gamepadthumb(int button, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
