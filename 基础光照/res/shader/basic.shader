//$Vertex
#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform float additionY;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;

void main()
{
    mat4 transform = proj * view * model;
    gl_Position = transform * vec4(pos.x, pos.y + additionY, pos.z, 1.0f);
    fragPos = vec3(model * vec4(pos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;
    texCoord = aTexCoord;
}

//$Fragment
#version 330 core
out vec4 fragColor;
in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec4 vertexColor;

void main()
{
    //fragColor = texture(u_Texture, texCoord) * vertexColor;
    float ambientStrength = 0.1f;
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 objectColor = vec3(1.0f, 0.5f, 0.0f);
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    fragColor = vec4(result, 1.0f);
}