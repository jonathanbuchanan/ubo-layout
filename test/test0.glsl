...
...
Other Shader Stuff
...
...

layout (std140) uniform TestBlock {
	float a;
	vec3 b;
	mat4 c;
	float d[3];
	bool e;
	int f;
	mat2x3 g;
};

layout (std140) uniform TestBlock1 {
	mat2x3 g;
	int f;
	bool e;
	float d[3];
	mat4 c;
	vec3 b;
	float a;
};

...
...
Other Shader Stuff
...
...
