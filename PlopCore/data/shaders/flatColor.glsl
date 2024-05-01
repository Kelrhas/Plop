#type vertex

#version 330 core

layout (location=0) in vec3 pos;

uniform mat4 u_mViewProjection;
uniform mat4 u_mModel;

void main()
{
	gl_Position = u_mViewProjection * u_mModel * vec4(pos, 1.0);
}

#type fragment

#version 330 core

layout (location=0) out vec4 color;

uniform vec4 u_color;

void main()
{
	color = u_color;
}