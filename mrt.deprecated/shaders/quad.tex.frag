#version 330 core

in vec2 UV;
out vec3 color;

uniform sampler2D in_tex;

void main(){
	color = texture(in_tex, UV).xyz ;
}

