/**********************************************************************************
// Vertex (Arquivo de Sombreamento)
//
// Criação:     08 Fev 2025
// Atualização: 08 Fev 2025
// Compilador:  Direct3D Shader Compiler (FXC)
//
// Descrição:   Aplica a transformação de mundo aos vértices
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

    // transforma para espaço homogêneo de recorte
    vout.PosH = mul(float4(vin.PosL, 1.0f), WorldViewProj);

    // apenas passa a cor do vértice para o pixel shader
    vout.Color = mul(vin.Color, Intensity);

    return vout;
}