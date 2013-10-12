varying vec3 normal;
varying vec3 position;
varying vec4 color;

void main()
{	
	gl_Position = ftransform();
  normal = gl_NormalMatrix * gl_Normal;
  position = vec3(gl_ModelViewMatrix * gl_Vertex);
  color = gl_Color;
}

