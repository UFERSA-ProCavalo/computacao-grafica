/**********************************************************************************
// Pixel (Arquivo de Sombreamento)
//
// Cria��o:     22 Jul 2020
// Atualiza��o: 03 Jul 2025
// Compilador:  Direct3D Shader Compiler (FXC)
//
// Descri��o:   Implementa ilumina��o ambiente, difusa e especular
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
    float3 Direction;               // vetor na dire��o da luz
    float  FalloffStart;            // inicio da atenua��o
    float3 Strength;                // pot�ncia da luz
    float  FalloffEnd;              // limite da atenua��o
    float3 Position;                // posi��o da luz
    float  SpotPower;               // pot�ncia do holofote
};

// ------------------------------------------------------------------------------

Texture2D DiffuseMap : register(t0);

SamplerState TextureSampler : register(s0);

cbuffer Scene : register(b0)
{
    float4x4 ViewProj;              // matriz de visualiza��o e proje��o
    float4   Ambient;               // cor da luz ambiente
    Light    Lights[4];             // luzes da cena
    float3   Eye;                   // vetor na dire��o do observador
    bool     Amb;                   // luz ambiente
    bool     Dif;                   // luz difusa
    bool     Spe;                   // luz especular
    bool     Lam;                   // �ngulo de incid�ncia
    bool     Mat;                   // material dos objetos
    bool     Tex;                   // textura dos objetos
}

cbuffer Constants : register(b1)
{
    float4x4 World;                 // matriz de mundo  
    int ObjIndex;                   // �ndice do objeto
}

cbuffer Material : register(b2)
{
    float4 Albedo;                  // cor do objeto
    float3 Fresnel;                 // refletividade 
    float Roughness;                // rugosidade
}

// ------------------------------------------------------------------------------

float4 Specular(float3 direction, float4 light, float3 normal, float3 toEye)
{
    // rugosidade da superf�cie
    const float m = (1 - Roughness) * 256.0f;    
    
    // half vector
    float3 h = normalize(toEye + direction);
    
    // cosseno do �ngulo entre o vetor normal e o vetor da fonte de luz
    float cosTheta = saturate(dot(normal, direction));
    
    // calcula componente especular
    float3 fresnel = Fresnel + (1.0f - Fresnel) * pow(1.0f - cosTheta, 5);
    float roughness = (m + 8.0f) * pow(max(dot(h, normal), 0.0f), m) / 8.0f;
    float4 specular = float4(fresnel * roughness, 1.0f) * light;
    
    // reduz um pouco o brilho
    return specular / (specular + 1.0f);    
}

// ------------------------------------------------------------------------------

float3 Lambert(float3 light, float3 normal)
{
    float3 lambert = float3(1.0f, 1.0f, 1.0f);
    
    if (ObjIndex != 12)
    {
        if (Lam) 
            lambert = max(dot(light, normal), 0.0f);
    }
    
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
        difuse = DiffuseMap.Sample(TextureSampler, texc) * albedo * light;
    else
        difuse = color * albedo * light;
    
    return difuse;
}

// ------------------------------------------------------------------------------

float Attenuation(Light L, float d)
{
    // atenua��o linear
    return saturate((L.FalloffEnd - d) / (L.FalloffEnd - L.FalloffStart));
}

// ------------------------------------------------------------------------------

float4 SpotLight(Light L, float3 posIn, float4 colorIn, float3 normal, float3 toEye, float2 texc)
{  
    // vetor da posi��o do pixel at� a posi��o da luz
    float3 toLight = L.Position - posIn;

    // a dist�ncia do pixel at� a luz
    float d = length(toLight);
    
    // cor resultante 
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // teste de dist�ncia
    if (d > L.FalloffEnd)
    {
        if (Amb)
            color = Ambient;
    }
    else
    {
        // normaliza o vetor 
        toLight /= d;
    
        // leva em conta o �ngulo de incid�ncia da luz
        float3 lambert = Lambert(toLight, normal);
    
        // calcula intensidade da luz    
        float3 light = lambert * L.Strength;
    
        // aplica atenua��o pela dist�ncia
        float3 pointlight = light * Attenuation(L, d);
    
        // limita alcanca pelo efeito holofote
        float spotFactor = pow(max(dot(-toLight, -L.Direction), 0.0f), L.SpotPower);
    
        // luz do holofote
        float4 spotlight = float4(pointlight * spotFactor, 1.0f);
    
        // componente difusa da luz
        float4 difuse = Difuse(colorIn, spotlight, texc);
    
        // calcula luz especular
        float4 specular = Specular(L.Direction, spotlight, normal, toEye);
    
        if (Amb) color += Ambient;
        if (Dif) color += difuse;
        if (Spe) color += specular;
    }
    
    return color;
}

// ------------------------------------------------------------------------------

float4 PointLight(Light L, float3 posIn, float4 colorIn, float3 normal, float3 toEye, float2 texc)
{  
    // vetor da posi��o do pixel at� a posi��o da luz
    float3 toLight = L.Position - posIn;

    // a dist�ncia do pixel at� a luz
    float d = length(toLight);
    
    // cor resultante 
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // teste de dist�ncia
    if (d > L.FalloffEnd)
    {
        if (Amb)
            color = Ambient;
    }
    else
    {
        // normaliza o vetor 
        toLight /= d;
    
        // leva em conta o �ngulo de incid�ncia da luz
        float3 lambert = Lambert(toLight, normal);
    
        // calcula intensidade da luz    
        float3 light = L.Strength * lambert;
    
        // aplica atenua��o pela dist�ncia
        float4 pointlight = float4(light * Attenuation(L, d), 1.0f);
    
        // componente difusa da luz
        float4 difuse = Difuse(colorIn, pointlight, texc);
    
        // calcula luz especular
        float4 specular = Specular(L.Direction, pointlight, normal, toEye);
    
        if (Amb) color += Ambient;
        if (Dif) color += difuse;
        if (Spe) color += specular;
    }
    
    return color;
}

// ------------------------------------------------------------------------------

float4 Directional(Light L, float4 colorIn, float3 normal, float3 toEye, float2 texc)
{
    // leva em conta o �ngulo de incid�ncia da luz
    float3 lambert =  Lambert(L.Direction, normal);
    
    // calcula intensidade da luz    
    float4 light = float4(L.Strength * lambert, 1.0f);
    
    // componente difusa da luz
    float4 difuse = Difuse(colorIn, light, texc);
    
    // calcula luz especular
    float4 specular = Specular(L.Direction, light, normal, toEye);
    
    // cor resultante 
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (Amb) color += Ambient;
    if (Dif) color += difuse;
    if (Spe) color += specular;
    
    return color;
}

// ------------------------------------------------------------------------------

float4 main(PixelIn pIn) : SV_TARGET
{ 
    // re-normaliza o vetor depois da interpola��o feita na rasteriza��o
    float3 normal = normalize(pIn.NormalW);
    
    // vetor do pixel para o observador
    float3 toEye = normalize(Eye - pIn.PosW.xyz);
    
    // cor inicial
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Monitor as SpotLight
    color += SpotLight(Lights[0], pIn.PosW.xyz, pIn.Color, normal, toEye, pIn.TexC);
    // Case as PointLight
    color += PointLight(Lights[1], pIn.PosW.xyz, pIn.Color, normal, toEye, pIn.TexC);
    // Directional lights
    color += Directional(Lights[2], pIn.Color, normal, toEye, pIn.TexC);
    color += Directional(Lights[3], pIn.Color, normal, toEye, pIn.TexC);
    // resultado
    return color;
}