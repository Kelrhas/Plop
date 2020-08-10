#type vertex

#version 330 core

layout (location=0) in vec3 in_pos;
layout (location=1) in vec4 in_color;
layout (location=2) in vec2 in_uv;
layout (location=3) in float in_texUnit;

uniform mat4 u_mViewProjection;

out vec4 v_color;
out vec2 v_uv;
out float v_texUnit;

void main()
{
	v_color = in_color;
	v_uv = in_uv;
	v_texUnit = in_texUnit;
	gl_Position = u_mViewProjection * vec4(in_pos, 1.0);
}

#type fragment

#version 330 core

layout (location=0) out vec4 out_color;

uniform sampler2D u_tDiffuse;

in vec4 v_color;
in vec2 v_uv;
in float v_texUnit;

uniform sampler2D u_textures[32];

void main()
{
	out_color = texture(u_textures[int(v_texUnit)], v_uv) * v_color;
}