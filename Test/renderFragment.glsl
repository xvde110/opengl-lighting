#version 330 core
in  vec2 texCoord;
out vec4 color;
uniform sampler2D win0;
uniform sampler2D win1;
uniform sampler2D win2;
uniform sampler2D win3;
void main()
{

	if(texCoord.x >= 0.0 && texCoord.x < 1.0 && texCoord.y >= 0.0 && texCoord.y < 1.0)
		color=texture(win0,texCoord);
	if(texCoord.x >= -1.0 && texCoord.x < 0.0 && texCoord.y >= -1.0 && texCoord.y < 0.0)
		color=texture(win3,texCoord+vec2(1,1));
	if(texCoord.x >= 0.0 && texCoord.x < 1.0 && texCoord.y >= -1.0 && texCoord.y < 0.0)
		color=texture(win2,texCoord+vec2(0,1));
	if(texCoord.x >= -1.0 && texCoord.x < 0.0 && texCoord.y >= 0.0 && texCoord.y < 1.0)
		color=texture(win1,texCoord+vec2(1,0));
}