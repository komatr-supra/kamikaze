#version 330

uniform sampler2D texture0;
uniform vec2 resolution;

in vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    vec2 texOffset = 1.0 / (resolution * 3);

    vec4 color = texture(texture0, fragTexCoord);
    color += texture(texture0, fragTexCoord);
    color += texture(texture0, fragTexCoord + vec2(texOffset.x, 0.0));
    color += texture(texture0, fragTexCoord + vec2(-texOffset.x, 0.0));
    color += texture(texture0, fragTexCoord + vec2(0.0, texOffset.y));
    color += texture(texture0, fragTexCoord + vec2(0.0, texOffset.y));

    
    fragColor = color / 6.0;
}