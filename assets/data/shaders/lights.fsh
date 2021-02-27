#ifdef GL_ES
precision mediump float;
#endif

varying vec2 v_TexCoord;

uniform mat4 u_Matrix;
uniform vec4 u_LightPositions[12];
uniform vec4 u_LightColors[12];
uniform vec4 u_AmbientColor;
uniform vec4 u_Falloff;
uniform ivec4 u_NumLights;

uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;

void main()
{
    vec4 DiffuseColor = texture2D(u_Texture, v_TexCoord);
    vec3 NormalMap = texture2D(u_NormalMap, v_TexCoord).rgb;
    vec3 Ambient = u_AmbientColor.rgb * u_AmbientColor.a;
    
    vec3 Sum = vec3(0.0);
    for (int i = 0; i < u_NumLights.x; ++i)
    {
        vec4 lightPosIn = u_LightPositions[i];
        vec4 light = vec4(lightPosIn.x, lightPosIn.y, 0, 1);
        vec4 lightNorm = u_Matrix * light;
        vec2 c = vec2(lightNorm.x, lightNorm.y);
        vec2 lightPos = (c + 1.0) / 2.0;
        vec3 LightDir = vec3(lightPos.xy - v_TexCoord, lightPosIn.z);
        float D = length(LightDir);
        vec3 N = normalize(NormalMap * 2.0 - 1.0);
        vec3 L = normalize(LightDir);
        vec3 Diffuse = (u_LightColors[i].rgb * u_LightColors[i].a) * max(dot(N, L), 0.0);
        float Attenuation = 1.0 / (u_Falloff.x + (u_Falloff.y*D) + (u_Falloff.z*D*D));
        vec3 Intensity = Diffuse * Attenuation;
        Sum += Intensity;
    }
    
    Sum += Ambient;
    gl_FragColor = vec4(Sum * DiffuseColor.rgb, DiffuseColor.a);
}
