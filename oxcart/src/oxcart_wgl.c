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

#include "oxcart_wgl.h"

PROC oxcart_proc_address(const char* name);

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;

/**
 * 
 */
int oxcart_wgl_extensions()
{
  int error = 0;

  error |= !(wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)oxcart_proc_address("wglCreateContextAttribsARB"));
  error |= !(wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)oxcart_proc_address("wglGetExtensionsStringARB"));
  error |= !(wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)oxcart_proc_address("wglMakeContextCurrentARB"));
  error |= !(wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)oxcart_proc_address("wglGetCurrentReadDCARB"));
  error |= !(wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)oxcart_proc_address("wglGetPixelFormatAttribivARB"));
  error |= !(wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)oxcart_proc_address("wglGetPixelFormatAttribfvARB"));
  error |= !(wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)oxcart_proc_address("wglChoosePixelFormatARB"));
  error |= !(wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)oxcart_proc_address("wglSwapIntervalEXT"));
  error |= !(wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)oxcart_proc_address("wglGetSwapIntervalEXT"));

  return(!error);
}
