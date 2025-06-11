/**********************************************************************************
// Curves (Arquivo de Cabeçalho)
//
// Criação:     12 Ago 2020
// Atualização: 21 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Base para gerar curvas usando Corner-Cutting
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

class Curves : public App
{
private:
    ID3D12RootSignature* rootSignature;
    ID3D12PipelineState* pipelineState;
    VertexBuffer<Vertex>* vbuffer;

    static const uint MaxControlPoints = 4;
    static const uint LineSegs = 256;

    static const uint MaxSize = LineSegs * 21;
    Vertex vertices[MaxSize];
    uint count = 0;
    uint index = 0;

    bool bezierMode = false;

    enum BezierState {
        Esperando_p1, // Esperando primeiro clique
        Ajustando_p2, // Primeiro clique
        Esperando_p3,
        Ajustando_p4
    };

    BezierState bezierState = Esperando_p1;

    VertexBuffer<Vertex>* cp_vbuffer;
    Vertex P[MaxControlPoints];
    uint controlPointCount = 0;

    VertexBuffer<Vertex>* curve_vbuffer;
    uint savedCurveCount = 0;
    uint curveCount = 0;

    VertexBuffer<Vertex>* guide_vbuffer;
    Vertex guideLine[4];
    uint guideLineCount = 0;

    VertexBuffer<Vertex>* rect_vbuffer;
    Vertex rectVertices[20];

public:
    void Init();
    void Update();
    void Display();
    void Finalize();

    void BuildRootSignature();
    void BuildPipelineState();

    void Save();
    void Load();
private:
    void CalculateBezierCurve();
    void UpdateControlRect();


};

// ------------------------------------------------------------------------------