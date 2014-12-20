@rem ********************************************************************************
@rem oxcart is licensed under the terms of the MIT license reproduced below.
@rem
@rem Copyright © 2014 Thomas J. Schaefer
@rem
@rem Permission is hereby granted, free of charge, to any person obtaining a copy of
@rem this software and associated documentation files (the "Software"), to deal in
@rem the Software without restriction, including without limitation the rights to
@rem use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
@rem the Software, and to permit persons to whom the Software is furnished to do so,
@rem subject to the following conditions:
@rem
@rem The above copyright notice and this permission notice shall be included in all
@rem copies or substantial portions of the Software.
@rem
@rem THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@rem IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
@rem FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
@rem COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
@rem IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
@rem CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
@rem ********************************************************************************

@rem oxcart archiver

@echo OFF
@setlocal ENABLEDELAYEDEXPANSION

@set MYDEL=del
@set MYZIP=..\tools\7-zip\7z.exe a -tzip
@set MYBIN=..\bin
@set MYCNF=
@set MYOUT=oxcart.zip
@set MYDAT=..\dat\*

@if "%1" equ "usage" goto :OK_USAGE
@if "%1" equ "debug" goto :OK_DEBUG
@if "%1" equ "release" goto :OK_RELEASE
@goto :OK_DEFAULT

:OK_USAGE
@echo.
@echo Usage: oxcart_ar.bat [usage^|debug^|release]
@echo   usage    prints these usage details
@echo   debug    debug archive configuration; default
@echo   release  release archive configuration
@goto :END

:OK_DEBUG
@shift
@goto :OK_DEFAULT

:OK_RELEASE
@shift
@set MYCNF=release
@goto :OK_BUILD

:OK_DEFAULT
@set MYCNF=debug
@goto :OK_BUILD

:OK_BUILD
%MYDEL% %MYBIN%\%MYCNF%\%MYOUT%
@if errorlevel 1 goto :FAILED
%MYZIP% %MYBIN%\%MYCNF%\%MYOUT% %MYDAT%
@if errorlevel 1 goto :FAILED
@goto :SUCCESS

:SUCCESS
@echo.
@echo === ARCHIVE SUCCESSFUL ===
@goto :END

:FAILED
@echo.
@echo *** ARCHIVE FAILED ***
@goto :END

:END
