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
// Limites do grid para manter objetos dentro da cena
const float GRID_MIN_X = -1.0f;
const float GRID_MAX_X =  1.0f;
const float GRID_MIN_Y =  0.0f;
const float GRID_MAX_Y =  1.0f;
const float GRID_MIN_Z = -1.0f;
const float GRID_MAX_Z =  1.0f;

// ------------------------------------------------------------------------------
struct Constants
{
    XMFLOAT4X4 WorldViewProj =
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };

    XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // cor do objeto
};

struct Object
{
    // Parâmetros de transformação
    XMFLOAT3 posicao = {0.0f, 0.0f, 0.0f}; // posição
    XMFLOAT3 escala = {1.0f, 1.0f, 1.0f};  // escala
    XMFLOAT3 rotacao = {0.0f, 0.0f, 0.0f}; // rotação (Euler XYZ, em radianos)
    XMFLOAT4 color = {255, 255, 255, 1.0f};// cor

    XMFLOAT4X4 world = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };

    Mesh* mesh = nullptr;
    VertexBuffer<Vertex>* vbuffer = nullptr;
    IndexBuffer<uint>* ibuffer = nullptr;
    ConstantBuffer<Constants>* cbuffer[4] = { nullptr, nullptr, nullptr, nullptr };

    std::vector<Vertex> originalVertices; // Cópia dos vértices originais para manipulação de cor
};
// ------------------------------------------------------------------------------

// Atualiza a cor de todos os vértices do objeto
void SetObjectVertexColor(Object& obj, const XMFLOAT4& color);

class Multiple : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    
    OrbitCamera camera;
    XMFLOAT4X4 Proj;
    static Timer timer;

    //ESTADOS
    bool isSpinning = true;
    bool isChanged = false;
    bool isAltMode = false;
    bool isRotating = false;
    bool isZooming = false;

    std::vector<Object> scene;
    int selectedIndex = -1; // Index do objeto selecionado, -1 = cena vazia
    int prevSelectedIndex = -1; // Guardar o objeto selecionado anteriormente

    // Lista de objetos pendentes para remoção
    // Usada para evitar remoção imediata durante o loop de atualização
    // Pois eu estava tendo problema de iteração com a lista de objetos e objetos removidos
    std::vector<Object> pendingDeletion;
    int deletionDelay = 0;

    // Retorna verdadeiro se selectedIndex for válido
    bool hasValidSelection() const {
        return selectedIndex >= 0 && selectedIndex < (int)scene.size();
    }

    Object gridObj; // Grid da cena
    ConstantBuffer<Constants>* gridCBuffer[4] = { nullptr, nullptr, nullptr, nullptr }; // 4 viewports

    // Linhas de separação para modo alternativo
    struct LineVertex {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };
    VertexBuffer<LineVertex>* lineVBuffer = nullptr;
    ConstantBuffer<Constants>* lineCBuffer = nullptr;

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
