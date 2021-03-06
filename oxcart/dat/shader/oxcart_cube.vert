/**
 * oxcart is licensed under the terms of the MIT license reproduced below.
 * 
 * Copyright � 2014 Thomas J. Schaefer
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

#version 400 core
layout (std140) uniform;

uniform camera {
  mat4 projection;
  mat4 view;
};

uniform mat4 model;

in vec3 vertex;
in vec3 normal;

out vec3 vs_vertex;
out vec3 vs_normal;

/**
 * 
 */
void main()
{
  vec4 v = model * vec4(vertex, 1.0);
  vec4 n = model * vec4(normal, 0.0);

  vs_vertex = v.xyz;
  vs_normal = n.xyz;
  gl_Position = projection * view * v;
}
