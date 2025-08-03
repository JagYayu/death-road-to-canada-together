#version 450 core

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(set = 0, binding = 1) uniform sampler2D uTexture;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(uTexture, fragTexCoord);
    outColor = fragColor * texColor;
}
