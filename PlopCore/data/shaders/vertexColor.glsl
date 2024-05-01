#type vertex

#version 330 core

layout (location=0) in vec3 pos;
layout (location=1) in vec4 color;

uniform mat4 u_mViewProjection;
uniform mat4 u_mModel;

out vec4 v_color;

void main()
{
	v_color = color;
	gl_Position = u_mViewProjection * u_mModel * vec4(pos, 1.0);
}

#type fragment

#version 330 core

layout (location=0) out vec4 color;

in vec4 v_color;

void main()
{
	color = v_color;
}