/**********************************************************************************
// Triangle (Arquivo de Cabe�alho)
//
// Cria��o:     22 Jul 2020
// Atualiza��o: 16 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descri��o:   Constr�i um Tri�ngulo usando o Direct3D 12
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

// ------------------------------------------------------------------------------

class Triangle : public App
{
private:
    ID3D12RootSignature* rootSignature;
    ID3D12PipelineState* pipelineState;
    VertexBuffer<Vertex>* vbuffer;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
