#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
//uniform sampler2D cameraDepthMap;

//uniform mat4 lightSpaceMatrix;

//uniform vec3 camFwd;
//uniform vec3 camRight;
//uniform vec3 camUp;

//uniform vec3 camPos;

//uniform float time;

#define USE_ACES 1
#define USE_VIGNETTE 1

#define USE_VOLUMETRIC_LIGHT 0
#define VL_STEP_COUNT 64.0
#define VL_DISTANCE 75.0

#define USE_SATURATION 1
#define SATURATION 1.3

float lineariseDepth(float depth){
	float near = 0.1;
	float far = 100.0;

	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float random(vec2 st){
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453);
}

// https://en.wikipedia.org/wiki/Academy_Color_Encoding_System
vec3 ACES(vec3 colorInput){
    mat3x3 rgb2aces = mat3x3(
        1.0498110175, 0.0000000000, -0.0000974845,
        -0.4959030231, 1.3733130458, 0.0982400361,
        0.0000000000, 0.0000000000, 0.9912520182
    );

    return colorInput * rgb2aces;
}

vec3 Saturation(vec3 color){
    float luma = dot(color, vec3(0.2126729, 0.7151522, 0.0721750));
    return vec3(luma) + vec3(SATURATION) * (color - vec3(luma));
}

vec3 Vignette(vec3 colorInput, vec2 uv){
    float vignette = 0.5;
    float vignetteRadius = 0.5;
    float vignetteSoftness = 0.5;

    vec2 vignetteCenter = vec2(0.5, 0.5);
    vec2 vignetteDelta = uv - vignetteCenter;
    float vignetteDistance = length(vignetteDelta);
    float vignetteIntensity = smoothstep(vignetteRadius, vignetteRadius + vignetteSoftness, vignetteDistance);
    vignetteIntensity = 1.0 - vignetteIntensity * vignette;

    return colorInput * vignetteIntensity;
}

void main(){
	vec3 albedo = texture(screenTexture, TexCoord).rgb;

	vec2 ScreenPos = TexCoord * 2.0 - 1.0;

	vec3 color = albedo;

	#if USE_ACES
			color = ACES(color);
	#endif

	#if USE_SATURATION
		color = Saturation(color);
	#endif

	#if USE_VIGNETTE
		color = Vignette(color, TexCoord);
	#endif

	FragColor = vec4(color.r, color.g, color.b, 1.0);
}