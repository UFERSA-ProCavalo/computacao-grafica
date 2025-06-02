/**********************************************************************************
// Vertex (Arquivo de Sombreamento)
//
// Cria��o:     08 Fev 2025
// Atualiza��o: 08 Fev 2025
// Compilador:  Direct3D Shader Compiler (FXC)
//
// Descri��o:   Aplica a transforma��o de mundo aos v�rtices
//
**********************************************************************************/

cbuffer Object
{
    float4x4 WorldViewProj;
    float Intensity;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;

    // transforma para espa�o homog�neo de recorte
    vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProj);

    // apenas passa a cor do v�rtice para o pixel shader
    vout.Color = mul(vin.Color, Intensity);

    return vout;
}