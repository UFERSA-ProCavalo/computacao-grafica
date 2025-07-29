/**********************************************************************************
// Pixel (Arquivo de Sombreamento)
//
// Criação:     22 Jul 2020
// Atualização: 08 Jul 2025
// Compilador:  Direct3D Shader Compiler (FXC)
//
// Descrição:   Implementa iluminação ambiente, difusa e especular
//
**********************************************************************************/

struct PixelIn
{
    float4 PosH : SV_POSITION;      // coordenadas do dispositivo
    float4 PosW : POSITION;         // coordenadas do mundo
    float4 Color : COLOR;           // cor do pixel
    float3 NormalW : NORMAL;        // vetor normal
    float2 TexC : TEXCOORD;         // textura
};

struct Light
{
    float3 Direction;               // vetor na direção da luz
    float  FalloffStart;            // inicio da atenuação
    float3 Strength;                // potência da luz
    float  FalloffEnd;              // limite da atenuação
    float3 Position;                // posição da luz
    float  SpotPower;               // potência do holofote
};

// ------------------------------------------------------------------------------

Texture2D Texture : register(t0);

SamplerState Anisotropic : register(s0);

cbuffer Scene : register(b0)
{
    float4x4 ViewProj;              // matriz de visualização e projeção
    bool     Amb;                   // luz ambiente
    float3   Ambient;               // cor da luz ambiente
    Light    Lights;                // luzes da cena
    float3   Eye;                   // vetor na direção do observador
    bool     Dif;                   // luz difusa
    bool     Spe;                   // luz especular
    bool     Lam;                   // ângulo de incidência
    bool     Mat;                   // material dos objetos
    bool     Tex;                   // textura dos objetos
    float    Elapsed;               // tempo transcorrido 
}

cbuffer Constants : register(b1)
{
    float4x4 World;                 // matriz de mundo  
    int ObjIndex;                   // índice do objeto
}

cbuffer Material : register(b2)
{
    float4 Albedo;                  // cor do objeto
    float3 Fresnel;                 // refletividade 
    float Roughness;                // rugosidade
}

// ------------------------------------------------------------------------------

float4 Specular(float3 direction, float3 light, float3 normal, float3 toEye)
{
    // rugosidade da superfície
    const float m = (1 - Roughness) * 256.0f;    
    
    // half vector
    float3 h = normalize(toEye + direction);
    
    // cosseno do ângulo entre o vetor normal e o vetor da fonte de luz
    float cosTheta = saturate(dot(normal, direction));
    
    // calcula componente especular
    float3 fresnel = Fresnel + (1.0f - Fresnel) * pow(1.0f - cosTheta, 5);
    float roughness = (m + 8.0f) * pow(max(dot(h, normal), 0.0f), m) / 8.0f;
    float3 specular = fresnel * roughness * light;
    
    // reduz um pouco o brilho
    return float4(specular / (specular + 1.0f), 0.0f);
}

// ------------------------------------------------------------------------------

float3 Lambert(float3 light, float3 normal)
{
    float3 lambert = float3(1.0f, 1.0f, 1.0f);
    
    if (Lam) 
        lambert = max(dot(light, normal), 0.0f);
    
    return lambert;
}

// ------------------------------------------------------------------------------

float4 Difuse(float4 color, float4 light, float2 texc)
{
    float4 difuse;
    float4 albedo;
    
    if (Mat) 
        albedo = Albedo;
    else
        albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if (Tex)
    {
        if (ObjIndex == 1)    
            difuse = Texture.Sample(Anisotropic, float2(texc.x + 0.1f * Elapsed, texc.y)) * albedo * light;
        else 
            difuse = Texture.Sample(Anisotropic, texc) * albedo * light;
    }
    else
    {
        difuse = color * albedo * light;
    }
    
    return difuse;
}

// ------------------------------------------------------------------------------

float Attenuation(Light L, float d)
{
    // atenuação linear
    return saturate((L.FalloffEnd - d) / (L.FalloffEnd - L.FalloffStart));
}

// ------------------------------------------------------------------------------

float4 SpotLight(Light L, float3 posIn, float4 colorIn, float3 normal, float3 toEye, float2 texc)
{  
    // vetor da posição do pixel até a posição da luz
    float3 toLight = L.Position - posIn;

    // a distância do pixel até a luz
    float d = length(toLight);
    
    // cor resultante 
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // cor ambiente
    float4 ambient = float4(Ambient, 0.0f);
    
    // teste de distância
    if (d > L.FalloffEnd)
    {
        if (Amb)
            color = ambient;
    }
    else
    {
        // normaliza o vetor 
        toLight /= d;
    
        // leva em conta o ângulo de incidência da luz
        float3 lambert = Lambert(toLight, normal);
    
        // calcula intensidade da luz    
        float3 light = lambert * L.Strength;
    
        // aplica atenuação pela distância
        float3 pointlight = light * Attenuation(L, d);
    
        // limita alcanca pelo efeito holofote
        float spotFactor = pow(max(dot(-toLight, -L.Direction), 0.0f), L.SpotPower);
    
        // luz do holofote
        float3 spotlight = pointlight * spotFactor;
    
        // componente difusa da luz
        float4 difuse = Difuse(colorIn, float4(spotlight,1.0f), texc);
    
        // calcula luz especular
        float4 specular = Specular(L.Direction, spotlight, normal, toEye);
    
        if (Amb) color += ambient;
        if (Dif) color += difuse;
        if (Spe) color += specular;
    }
    
    return color;
}

// ------------------------------------------------------------------------------

float4 PointLight(Light L, float3 posIn, float4 colorIn, float3 normal, float3 toEye, float2 texc)
{  
    // vetor da posição do pixel até a posição da luz
    float3 toLight = L.Position - posIn;

    // a distância do pixel até a luz
    float d = length(toLight);
    
    // cor resultante 
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // cor ambiente
    float4 ambient = float4(Ambient, 0.0f);
    
    // teste de distância
    if (d > L.FalloffEnd)
    {
        if (Amb)
            color = ambient;
    }
    else
    {
        // normaliza o vetor 
        toLight /= d;
    
        // leva em conta o ângulo de incidência da luz
        float3 lambert = Lambert(toLight, normal);
    
        // calcula intensidade da luz    
        float3 light = L.Strength * lambert;
    
        // aplica atenuação pela distância
        float3 pointlight = light * Attenuation(L, d);
    
        // componente difusa da luz
        float4 difuse = Difuse(colorIn, float4(pointlight,1.0f), texc);
    
        // calcula luz especular
        float4 specular = Specular(L.Direction, pointlight, normal, toEye);
    
        if (Amb) color += ambient;
        if (Dif) color += difuse;
        if (Spe) color += specular;
    }
    
    return color;
}

// ------------------------------------------------------------------------------

float4 Directional(Light L, float4 colorIn, float3 normal, float3 toEye, float2 texc)
{
    // cor ambiente
    float4 ambient = float4(Ambient, 0.0f);
    
    // leva em conta o ângulo de incidência da luz
    float3 lambert = Lambert(L.Direction, normal);
    
    // calcula intensidade da luz    
    float3 light = L.Strength * lambert;
    
    // componente difusa da luz
    float4 difuse = Difuse(colorIn, float4(light,1.0f), texc);
    
    // calcula luz especular
    float4 specular = Specular(L.Direction, light, normal, toEye);
    
    // cor resultante 
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (Amb) color += ambient;
    if (Dif) color += difuse;
    if (Spe) color += specular;
    
    return color;
}

// ------------------------------------------------------------------------------

float4 main(PixelIn pIn) : SV_TARGET
{ 
    // re-normaliza o vetor depois da interpolação feita na rasterização
    float3 normal = normalize(pIn.NormalW);
    
    // vetor do pixel para o observador
    float3 toEye = normalize(Eye - pIn.PosW.xyz);
    
    // cor inicial
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // contribuição de todas as luzes
    color += Directional(Lights, pIn.Color, normal, toEye, pIn.TexC);

    // resultado
    return color;
}

// ------------------------------------------------------------------------------