local cel = require 'cel'

local M = {
  red=cel.color.rgb(1, 0, 0),
  yellow=cel.color.rgb(1, 1, 0),
  green=cel.color.rgb(0, 1, 0),
  blue=cel.color.rgb(0, 0, 1),

  blue5=cel.color.setsaturation(cel.color.rgb(0, 0, 1), .5),

  cyan=cel.color.rgb(0, 1, 1),
  cyan5=cel.color.setsaturation(cel.color.rgb(0, 1, 1), .5),
  cyan8=cel.color.setsaturation(cel.color.rgb(0, 1, 1), .8),

  black=cel.color.rgb(0, 0, 0),
  gray1=cel.color.rgb(.1, .1, .1),
  gray2=cel.color.rgb(.2, .2, .2),
  gray3=cel.color.rgb(.3, .3, .3),
  gray4=cel.color.rgb(.4, .4, .4),
  gray5=cel.color.rgb(.5, .5, .5),
  gray6=cel.color.rgb(.6, .6, .6),
  gray7=cel.color.rgb(.7, .7, .7),
  gray8=cel.color.rgb(.8, .8, .8),
  gray9=cel.color.rgb(.9, .9, .9),
  white=cel.color.rgb(1, 1, 1),

  
  oceanblue=cel.color.rgb8(0, 136, 204),
}

M.themecolor = M.oceanblue
M.themebordercolor = M.white
M.themetextcolor = M.white
M.themecomplement = cel.color.getcomplement(M.themecolor, nil, .6, 1)
M.themecomplementlight = cel.color.getcomplement(M.themecolor, nil, .8, 1)

return M
