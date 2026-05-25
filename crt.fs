#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;
uniform sampler2D texture0;

const vec2 curvature = vec2(4.5, 4.5);

void main() {
    vec2 uv = fragTexCoord;
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / curvature;
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 texelColor = texture(texture0, uv);
    float scanline = sin(uv.y * 1000.0) * 0.03;
    texelColor.rgb -= scanline;

    float vignette = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    vignette = clamp(pow(16.0 * vignette, 0.2), 0.0, 1.0);
    texelColor.rgb *= vignette;

    finalColor = texelColor;
}