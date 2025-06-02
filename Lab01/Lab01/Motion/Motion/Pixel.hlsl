/**********************************************************************************
// Pixel (Arquivo de Sombreamento)
//
// Criação:     08 Fev 2025
// Atualização: 08 Fev 2025
// Compilador:  Direct3D Shader Compiler (FXC)
//
// Descrição:   Tratamento básico do Pixel Shader
//
**********************************************************************************/

struct pixelIn
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(pixelIn pIn) : SV_TARGET
{
    return pIn.Color;
}