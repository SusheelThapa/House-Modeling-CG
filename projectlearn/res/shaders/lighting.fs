#version 330 core
// #extension GL_NV_shadow_samplers_cube : enable
out vec4 FragColor;

const int MAX_BULBS = 50;
const int MAX_POINT_BULBS = 50;

struct Material{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    bool hasTexture;
};

struct BaseLight {
    // vec3 Color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SunLight {
    vec3 position;
    BaseLight base;
    vec3 direction;
};

struct Attenuation{
    float constant;
    float linear;
    float exp;
};

struct PointLight{
    BaseLight base;
    vec3 position;
    Attenuation atten; 
};

struct SpotLight{
    PointLight base;
    vec3 direction;
    float cutoff;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform vec3 viewPos;
uniform samplerCube cubeMap;
uniform Material material;
uniform SunLight sunLight;
uniform SpotLight bulbs[MAX_BULBS];
uniform PointLight pointBulbs[MAX_POINT_BULBS];
uniform int numBulbs;
uniform int numpBulbs;
uniform bool isBulb;
uniform bool isGlass;
uniform bool isWater;

uniform sampler2D texture_diffuse1;

vec3 reflection(vec3 LightDir, vec3 normal)
{
    vec3 incident = normalize(LightDir);
    vec3 reflected = normalize(reflect(incident,normal));
    return reflected;
}

vec3 refraction(vec3 LightDir, vec3 normal)
{
    vec3 incident = normalize(LightDir);
    vec3 reflected = normalize(refract(incident,normal,1.1));
    return reflected;
}

vec4 CalcLightInternal(BaseLight light, vec3 LightDir, vec3 normal, bool bulb)
{
    // vec4 ambientColor = vec4(light.Color,1.0f) * light.ambient * material.ambient.rgba;
    vec4 ambientColor = vec4(light.ambient,1.0) * material.ambient.rgba;

    float diffuseFactor = dot(normal, LightDir);

    vec4 diffuseColor = vec4(0,0,0,0);
    vec4 specularColor = vec4(0,0,0,0);

    if (diffuseFactor > 0 )
    {
        // diffuseColor = vec4(light.Color, 1.0f) * light.diffuse * material.diffuse.rgba * diffuseFactor;
        diffuseColor = vec4(light.diffuse,1.0) * material.diffuse.rgba * diffuseFactor;

        vec3 viewDir = normalize(viewPos-FragPos);
        vec3 reflectDir = normalize(reflect(-LightDir, normal));
        float specularFactor = dot(viewDir,reflectDir);
        if(specularFactor>0)
        {
            float exp;
            if( bulb ) exp = 256.f;
            else exp = material.shininess;
            float spec = pow(specularFactor,exp);
            // specularColor = vec4(light.Color, 1.0f) * light.specular * material.specular.rgba * spec;
            specularColor = vec4(light.specular,1.0) * material.specular.rgba * spec;
        }
    }

    return (ambientColor+diffuseColor+specularColor);
}

vec4 CalcDirectionalLight( vec3 normal )
{
    vec3 dir = normalize(sunLight.position-FragPos);
    return CalcLightInternal(sunLight.base, dir, normal, false);
    // vec3 dir = normalize(sunLight.direction);
    // return CalcLightInternal(sunLight.base, dir, normal, false);
}

vec4 CalcPointLight(PointLight l, vec3 normal)
{
    vec3 LightDir = l.position - FragPos;
    float distance = length(LightDir);
    LightDir = normalize(LightDir);

    vec4 Color = CalcLightInternal(l.base, LightDir,normal, true);
    float attenuationFactor = l.atten.constant + (l.atten.linear * distance) + (l.atten.exp * distance * distance);

    return Color/attenuationFactor;
}

vec4 CalcSpotLight( SpotLight l, vec3 normal )
{
    vec3 LightDir = normalize(FragPos-l.base.position);
    float spotFactor = dot(LightDir, l.direction);

    if( spotFactor>l.cutoff ) 
    {
        vec4 Color = CalcPointLight(l.base, normal);
        float spotLightIntensity = 1.0-((1.0-spotFactor)/(1.0-l.cutoff));
        // return Color * spotLightIntensity;
        return Color;
    }
    else
    {
        return vec4(0,0,0,0);
    }

}

void main()
{

    vec3 normal = normalize(Normal);
    vec4 totalLight = CalcDirectionalLight(normal);

    bool night = sunLight.base.ambient == vec3(0.0) && sunLight.base.diffuse == vec3(0.0) && sunLight.base.specular == vec3(0.0);
    if( night )
    {
        for( int i=0; i<numBulbs; ++i )
        {
            // totalLight += CalcPointLight(i,normal);
            totalLight += CalcSpotLight(bulbs[i],normal);
        }
        for( int i=0; i<numpBulbs; ++i )
        {
            // totalLight += CalcPointLight(i,normal);
            totalLight += CalcPointLight(pointBulbs[i],normal);
        }
    
        if( isBulb )
        {
            totalLight = vec4(255,178,0,1);
            // totalLight = vec4(1.f);
        }
    }
    if( isGlass )
    {
        vec3 dir = normalize(FragPos-viewPos);
        vec3 reflected = reflection(dir,normal); 
        totalLight *= texture(cubeMap,reflected) * 0.7;
        // totalLight *= texture(texture_diffuse1,vec2(reflected.x,reflected.y));
        // reflected = refraction(dir,normal);
        // totalLight *= texture(cubeMap,reflected);
    }
    if( isWater )
    {
        vec3 dir = normalize(FragPos-viewPos);
        vec3 reflected = reflection(dir,normal); 
        totalLight *= texture(cubeMap,reflected);
        // totalLight *= texture(texture_diffuse1,vec2(reflected.x,reflected.y));
        // reflected = refraction(dir,normal);
        // totalLight *= texture(cubeMap,reflected);       
    }

    if ( !material.hasTexture ) FragColor = totalLight;
    else FragColor = texture(texture_diffuse1,TexCoords) * totalLight;
} 