varying vec3 normal;
varying vec3 position;
varying vec4 color;

void main()
{
  //0 - color, 1 - normal, 2 - position

  gl_FragData[0] = color;
  gl_FragData[1] = vec4(normal, 1);
  gl_FragData[2] = vec4(position, 1);
}

