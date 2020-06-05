//$Vertex
#version 450 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 aTexCoord;

uniform int option;
out vec2 texCoord;

void main()
{
    vec2 rPos = (pos + vec2(1.0, 1.0)) * 0.5;
    if (option == 0) rPos = pos;
    else if (option == 1) rPos.x -= 1.0;
    else if (option == 2) rPos = rPos;
    else if (option == 3) rPos += vec2(-1.0, -1.0);
    else if (option == 4) rPos.y -= 1.0;
    gl_Position = vec4(rPos, 0.0f, 1.0f);
    texCoord = aTexCoord;
}

//$Fragment
#version 450 core

in vec2 texCoord;
layout(location = 0) out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, texCoord);
}