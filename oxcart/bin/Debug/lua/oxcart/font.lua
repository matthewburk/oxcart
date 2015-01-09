require 'oxcart.atlas'
local ffi = require 'ffi'
local C = ffi.C
local math = math

ffi.cdef[[
typedef struct oxcart_font_t oxcart_font_t;
typedef struct oxcart_glyph_t oxcart_glyph_t;

oxcart_font_t* oxcart_font_create(const char* filename, int size, float line, oxcart_atlas_t* atlas);
void oxcart_font_destroy(oxcart_font_t* font);
const char* oxcart_font_properties(oxcart_font_t* font, int* size, float* line);
oxcart_atlas_t* oxcart_font_atlas(oxcart_font_t* font);
void oxcart_font_metrics(oxcart_font_t* font, float* height, float* ascent, float* descent, float* advance);
oxcart_glyph_t* oxcart_font_glyph(oxcart_font_t* font, char code);

void oxcart_glyph_metrics(oxcart_glyph_t* glyph, float* w, float* h, float* bearing_x, float* bearing_y, float* advance);
float oxcart_glyph_kerning(oxcart_glyph_t* glyph, char code);
void oxcart_glyph_texcoords(oxcart_glyph_t* glyph, float* s0, float* t0, float* s1, float* t1);
]]

local M = {}

local mt = {}
mt.__index = mt

do
  local math_min = math.min
  local math_max = math.max

  local aw = ffi.new('float[1]')
  local ah = ffi.new('float[1]')
  local abearingx = ffi.new('float[1]')
  local abearingy = ffi.new('float[1]')
  local aadvance = ffi.new('float[1]')

  local function getglyphmetrics(glyph)
    C.oxcart_glyph_metrics(glyph, aw, ah, abearingx, abearingy, aadvance)
    return aw[0], ah[0], abearingx[0], abearingy[0], aadvance[0]
  end

  function mt:getglyphmetrics(byte)
    local glyph = C.oxcart_font_glyph(self.cdata, byte)
    return getglyphmetrics(glyph)
  end

  --origin is basline y is positive down
  --returns advance, xmin(ink), xmax(ink), ymin(ink), ymax(ink)
  function mt:measure(text)
    local xmin, xmax, ymin, ymax = 32000, -32000, 32000, -32000
    local penx = 0

    local font_cdata = self.cdata
    for i = 1, #text do
      local glyph = C.oxcart_font_glyph(font_cdata, string.byte(text, i))
      local w, h, bearingx, bearingy, advance = getglyphmetrics(glyph)

      local glyph_xmin = penx + bearingx
      local glyph_xmax = penx + bearingx + w

      xmin = math_min(xmin, glyph_xmin)
      xmax = math_max(xmax, glyph_xmax)
      ymin = math_min(ymin, bearingy)
      ymax = math_max(ymax, bearingy + h)

      penx = penx + advance
    end

    if xmin > xmax then
      return penx, 0, 0, 0, 0, 0
    end

    return penx, -xmin, xmax, ymin, ymax
  end
end

do
  local as0 = ffi.new('float[1]')
  local at0 = ffi.new('float[1]')
  local as1 = ffi.new('float[1]')
  local at1 = ffi.new('float[1]')
  function mt:getglyphtexcoords(byte)
    assert(self.cdata)
    local glyph = C.oxcart_font_glyph(self.cdata, byte)
    C.oxcart_glyph_texcoords(glyph, as0, at0, as1, at1)
    return as0[0], at0[0], as1[0], at1[0]
  end
end

M.config = {
  firasans = {
    regular = "/font/firasans-regular.otf",
    bold = "/font/firasans-bold.otf",
    italic = "/font/firasans-italic.otf",
    bolditalic = "/font/firasans-bold-italic.otf"
  },

  firamono = {
    regular = "/font/firamono-regular.otf",
    bold = "/font/firamono-bold.otf",
    italic ="/font/firamono-regular.otf",
    bolditalic = "/font/firamono-bold.otf"
  }
}

local loadedfonts = {}

local function getfontmetrics(font, t)
  local lineheight = ffi.new('float[1]')
  local ascent = ffi.new('float[1]')
  local descent = ffi.new('float[1]')
  local maxadvance = ffi.new('float[1]')

  C.oxcart_font_metrics(font, lineheight, ascent, descent, maxadvance)
  t.lineheight = lineheight[0]
  t.ascent = ascent[0]
  t.descent = descent[0]
  t.maxadvance = maxadvance[0]
end

--TODO what happens when atlas is full
M.atlas = oxcart.atlas.new() --default atlas 


--load new font into atlas
--oxcart.font.atlas is used if atlas is nil
function M.load(name, style, size, line, atlas)
  line = line or 0

  local fontfile = M.config[name][style]

  for i, t in ipairs(loadedfonts) do
    if t.fontfile == fontfile 
    and t.font.size == size 
    and t.font.atlas == atlas
    and t.line == math.floor(line*1000+.5) then
      return t.font
    end
  end

  local font = {
    cdata = false,
    atlas = atlas, 
    size = size,
    metrics = {
      lineheight = 0,
      ascent = 0,
      descent = 0,
      maxadvance = 0,
    }
  }

  atlas = atlas or M.atlas

  font.cdata = ffi.gc(C.oxcart_font_create(fontfile, size, line, atlas), C.oxcart_font_destroy)
  getfontmetrics(font.cdata, font.metrics)

  loadedfonts[#loadedfonts+1] = {
    font=font,
    fontfile=fontfile,
    line=math.floor(line*1000+.5),
  }

  return setmetatable(font, mt)
end

oxcart.font = M

return M
