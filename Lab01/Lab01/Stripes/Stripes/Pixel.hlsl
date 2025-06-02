/**********************************************************************************
// Pixel (Arquivo de Sombreamento)
//
// Cria��o:     08 Fev 2025
// Atualiza��o: 08 Fev 2025
// Compilador:  Direct3D Shader Compiler (FXC)
//
// Descri��o:   Tratamento b�sico do Pixel Shader
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