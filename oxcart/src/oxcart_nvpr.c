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

#include "oxcart_nvpr.h"

PROC oxcart_proc_address(const char* name);

PFNGLGENPATHSNVPROC glGenPathsNV;
PFNGLDELETEPATHSNVPROC glDeletePathsNV;
PFNGLISPATHNVPROC glIsPathNV;
PFNGLPATHCOMMANDSNVPROC glPathCommandsNV;
PFNGLPATHCOORDSNVPROC glPathCoordsNV;
PFNGLPATHSUBCOMMANDSNVPROC glPathSubCommandsNV;
PFNGLPATHSUBCOORDSNVPROC glPathSubCoordsNV;
PFNGLPATHSTRINGNVPROC glPathStringNV;
PFNGLPATHGLYPHSNVPROC glPathGlyphsNV;
PFNGLPATHGLYPHRANGENVPROC glPathGlyphRangeNV;
PFNGLWEIGHTPATHSNVPROC glWeightPathsNV;
PFNGLCOPYPATHNVPROC glCopyPathNV;
PFNGLINTERPOLATEPATHSNVPROC glInterpolatePathsNV;
PFNGLTRANSFORMPATHNVPROC glTransformPathNV;
PFNGLPATHPARAMETERIVNVPROC glPathParameterivNV;
PFNGLPATHPARAMETERINVPROC glPathParameteriNV;
PFNGLPATHPARAMETERFVNVPROC glPathParameterfvNV;
PFNGLPATHPARAMETERFNVPROC glPathParameterfNV;
PFNGLPATHDASHARRAYNVPROC glPathDashArrayNV;
PFNGLPATHSTENCILFUNCNVPROC glPathStencilFuncNV;
PFNGLPATHSTENCILDEPTHOFFSETNVPROC glPathStencilDepthOffsetNV;
PFNGLSTENCILFILLPATHNVPROC glStencilFillPathNV;
PFNGLSTENCILSTROKEPATHNVPROC glStencilStrokePathNV;
PFNGLSTENCILFILLPATHINSTANCEDNVPROC glStencilFillPathInstancedNV;
PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC glStencilStrokePathInstancedNV;
PFNGLPATHCOVERDEPTHFUNCNVPROC glPathCoverDepthFuncNV;
PFNGLPATHCOLORGENNVPROC glPathColorGenNV;
PFNGLPATHTEXGENNVPROC glPathTexGenNV;
PFNGLPATHFOGGENNVPROC glPathFogGenNV;
PFNGLCOVERFILLPATHNVPROC glCoverFillPathNV;
PFNGLCOVERSTROKEPATHNVPROC glCoverStrokePathNV;
PFNGLCOVERFILLPATHINSTANCEDNVPROC glCoverFillPathInstancedNV;
PFNGLCOVERSTROKEPATHINSTANCEDNVPROC glCoverStrokePathInstancedNV;
PFNGLGETPATHPARAMETERIVNVPROC glGetPathParameterivNV;
PFNGLGETPATHPARAMETERFVNVPROC glGetPathParameterfvNV;
PFNGLGETPATHCOMMANDSNVPROC glGetPathCommandsNV;
PFNGLGETPATHCOORDSNVPROC glGetPathCoordsNV;
PFNGLGETPATHDASHARRAYNVPROC glGetPathDashArrayNV;
PFNGLGETPATHMETRICSNVPROC glGetPathMetricsNV;
PFNGLGETPATHMETRICRANGENVPROC glGetPathMetricRangeNV;
PFNGLGETPATHSPACINGNVPROC glGetPathSpacingNV;
PFNGLGETPATHCOLORGENIVNVPROC glGetPathColorGenivNV;
PFNGLGETPATHCOLORGENFVNVPROC glGetPathColorGenfvNV;
PFNGLGETPATHTEXGENIVNVPROC glGetPathTexGenivNV;
PFNGLGETPATHTEXGENFVNVPROC glGetPathTexGenfvNV;
PFNGLISPOINTINFILLPATHNVPROC glIsPointInFillPathNV;
PFNGLISPOINTINSTROKEPATHNVPROC glIsPointInStrokePathNV;
PFNGLGETPATHLENGTHNVPROC glGetPathLengthNV;
PFNGLPOINTALONGPATHNVPROC glPointAlongPathNV;

/**
 * 
 */
int oxcart_nvpr_extensions()
{
  int error = 0;

  error |= !(glGenPathsNV = (PFNGLGENPATHSNVPROC)oxcart_proc_address("glGenPathsNV"));
  error |= !(glDeletePathsNV = (PFNGLDELETEPATHSNVPROC)oxcart_proc_address("glDeletePathsNV"));
  error |= !(glIsPathNV = (PFNGLISPATHNVPROC)oxcart_proc_address("glIsPathNV"));
  error |= !(glPathCommandsNV = (PFNGLPATHCOMMANDSNVPROC)oxcart_proc_address("glPathCommandsNV"));
  error |= !(glPathCoordsNV = (PFNGLPATHCOORDSNVPROC)oxcart_proc_address("glPathCoordsNV"));
  error |= !(glPathSubCommandsNV = (PFNGLPATHSUBCOMMANDSNVPROC)oxcart_proc_address("glPathSubCommandsNV"));
  error |= !(glPathSubCoordsNV = (PFNGLPATHSUBCOORDSNVPROC)oxcart_proc_address("glPathSubCoordsNV"));
  error |= !(glPathStringNV = (PFNGLPATHSTRINGNVPROC)oxcart_proc_address("glPathStringNV"));
  error |= !(glPathGlyphsNV = (PFNGLPATHGLYPHSNVPROC)oxcart_proc_address("glPathGlyphsNV"));
  error |= !(glPathGlyphRangeNV = (PFNGLPATHGLYPHRANGENVPROC)oxcart_proc_address("glPathGlyphRangeNV"));
  error |= !(glWeightPathsNV = (PFNGLWEIGHTPATHSNVPROC)oxcart_proc_address("glWeightPathsNV"));
  error |= !(glCopyPathNV = (PFNGLCOPYPATHNVPROC)oxcart_proc_address("glCopyPathNV"));
  error |= !(glInterpolatePathsNV = (PFNGLINTERPOLATEPATHSNVPROC)oxcart_proc_address("glInterpolatePathsNV"));
  error |= !(glTransformPathNV = (PFNGLTRANSFORMPATHNVPROC)oxcart_proc_address("glTransformPathNV"));
  error |= !(glPathParameterivNV = (PFNGLPATHPARAMETERIVNVPROC)oxcart_proc_address("glPathParameterivNV"));
  error |= !(glPathParameteriNV = (PFNGLPATHPARAMETERINVPROC)oxcart_proc_address("glPathParameteriNV"));
  error |= !(glPathParameterfvNV = (PFNGLPATHPARAMETERFVNVPROC)oxcart_proc_address("glPathParameterfvNV"));
  error |= !(glPathParameterfNV = (PFNGLPATHPARAMETERFNVPROC)oxcart_proc_address("glPathParameterfNV"));
  error |= !(glPathDashArrayNV = (PFNGLPATHDASHARRAYNVPROC)oxcart_proc_address("glPathDashArrayNV"));
  error |= !(glPathStencilFuncNV = (PFNGLPATHSTENCILFUNCNVPROC)oxcart_proc_address("glPathStencilFuncNV"));
  error |= !(glPathStencilDepthOffsetNV = (PFNGLPATHSTENCILDEPTHOFFSETNVPROC)oxcart_proc_address("glPathStencilDepthOffsetNV"));
  error |= !(glStencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC)oxcart_proc_address("glStencilFillPathNV"));
  error |= !(glStencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC)oxcart_proc_address("glStencilStrokePathNV"));
  error |= !(glStencilFillPathInstancedNV = (PFNGLSTENCILFILLPATHINSTANCEDNVPROC)oxcart_proc_address("glStencilFillPathInstancedNV"));
  error |= !(glStencilStrokePathInstancedNV = (PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC)oxcart_proc_address("glStencilStrokePathInstancedNV"));
  error |= !(glPathCoverDepthFuncNV = (PFNGLPATHCOVERDEPTHFUNCNVPROC)oxcart_proc_address("glPathCoverDepthFuncNV"));
  error |= !(glPathColorGenNV = (PFNGLPATHCOLORGENNVPROC)oxcart_proc_address("glPathColorGenNV"));
  error |= !(glPathTexGenNV = (PFNGLPATHTEXGENNVPROC)oxcart_proc_address("glPathTexGenNV"));
  error |= !(glPathFogGenNV = (PFNGLPATHFOGGENNVPROC)oxcart_proc_address("glPathFogGenNV"));
  error |= !(glCoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC)oxcart_proc_address("glCoverFillPathNV"));
  error |= !(glCoverStrokePathNV = (PFNGLCOVERSTROKEPATHNVPROC)oxcart_proc_address("glCoverStrokePathNV"));
  error |= !(glCoverFillPathInstancedNV = (PFNGLCOVERFILLPATHINSTANCEDNVPROC)oxcart_proc_address("glCoverFillPathInstancedNV"));
  error |= !(glCoverStrokePathInstancedNV = (PFNGLCOVERSTROKEPATHINSTANCEDNVPROC)oxcart_proc_address("glCoverStrokePathInstancedNV"));
  error |= !(glGetPathParameterivNV = (PFNGLGETPATHPARAMETERIVNVPROC)oxcart_proc_address("glGetPathParameterivNV"));
  error |= !(glGetPathParameterfvNV = (PFNGLGETPATHPARAMETERFVNVPROC)oxcart_proc_address("glGetPathParameterfvNV"));
  error |= !(glGetPathCommandsNV = (PFNGLGETPATHCOMMANDSNVPROC)oxcart_proc_address("glGetPathCommandsNV"));
  error |= !(glGetPathCoordsNV = (PFNGLGETPATHCOORDSNVPROC)oxcart_proc_address("glGetPathCoordsNV"));
  error |= !(glGetPathDashArrayNV = (PFNGLGETPATHDASHARRAYNVPROC)oxcart_proc_address("glGetPathDashArrayNV"));
  error |= !(glGetPathMetricsNV = (PFNGLGETPATHMETRICSNVPROC)oxcart_proc_address("glGetPathMetricsNV"));
  error |= !(glGetPathMetricRangeNV = (PFNGLGETPATHMETRICRANGENVPROC)oxcart_proc_address("glGetPathMetricRangeNV"));
  error |= !(glGetPathSpacingNV = (PFNGLGETPATHSPACINGNVPROC)oxcart_proc_address("glGetPathSpacingNV"));
  error |= !(glGetPathColorGenivNV = (PFNGLGETPATHCOLORGENIVNVPROC)oxcart_proc_address("glGetPathColorGenivNV"));
  error |= !(glGetPathColorGenfvNV = (PFNGLGETPATHCOLORGENFVNVPROC)oxcart_proc_address("glGetPathColorGenfvNV"));
  error |= !(glGetPathTexGenivNV = (PFNGLGETPATHTEXGENIVNVPROC)oxcart_proc_address("glGetPathTexGenivNV"));
  error |= !(glGetPathTexGenfvNV = (PFNGLGETPATHTEXGENFVNVPROC)oxcart_proc_address("glGetPathTexGenfvNV"));
  error |= !(glIsPointInFillPathNV = (PFNGLISPOINTINFILLPATHNVPROC)oxcart_proc_address("glIsPointInFillPathNV"));
  error |= !(glIsPointInStrokePathNV = (PFNGLISPOINTINSTROKEPATHNVPROC)oxcart_proc_address("glIsPointInStrokePathNV"));
  error |= !(glGetPathLengthNV = (PFNGLGETPATHLENGTHNVPROC)oxcart_proc_address("glGetPathLengthNV"));
  error |= !(glPointAlongPathNV = (PFNGLPOINTALONGPATHNVPROC)oxcart_proc_address("glPointAlongPathNV"));

  return(!error);
}
