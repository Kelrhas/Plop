#type vertex

#version 330 core

layout (location=0) in vec3 pos;
layout (location=1) in vec2 uv;

uniform mat4 u_mViewProjection;
uniform mat4 u_mModel;

out vec2 v_uv;

void main()
{
	v_uv = uv;
	gl_Position = u_mViewProjection * u_mModel * vec4(pos, 1.0);
}

#type fragment

#version 330 core

layout (location=0) out vec4 color;

uniform sampler2D u_tDiffuse;
uniform vec4 u_color;

in vec2 v_uv;

void main()
{
	color = texture(u_tDiffuse, v_uv) * u_color;
}