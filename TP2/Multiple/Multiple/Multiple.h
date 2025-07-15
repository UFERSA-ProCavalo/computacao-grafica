/**********************************************************************************
// Multiple (Arquivo de Cabeçalho)
//
// Criação:     27 Abr 2016
// Atualização: 22 Jun 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Cena 3D usando múltiplos buffers, um por objeto
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

struct Constants
{
    XMFLOAT4X4 WorldViewProj =
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };
};

struct Object
{
    XMFLOAT4X4 world = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };

    Mesh* mesh = nullptr;
    VertexBuffer<Vertex>* vbuffer = nullptr;
    IndexBuffer<uint>* ibuffer = nullptr;
	ConstantBuffer<Constants>* cbuffer[4] = { nullptr, nullptr, nullptr, nullptr }; // um buffer por viewport
};

// ------------------------------------------------------------------------------

class Multiple : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    
    OrbitCamera camera;
    XMFLOAT4X4 Proj;
    static Timer timer;
    bool spinning = true;
    
    vector<Object> scene;

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();

    static void Pause()  { timer.Stop();  }
    static void Resume() { timer.Start(); }

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
