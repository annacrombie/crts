#version 330 core

out vec4 clr;

flat in vec4 inclr;

void main()
{
	clr = inclr;
}
