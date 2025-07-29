/**********************************************************************************
// Desktop (Arquivo de Cabe�alho)
//
// Cria��o:     02 Out 2023
// Atualiza��o: 03 Jul 2025
// Compilador:  Visual C++ 2022
//
// Descri��o:   Desenha v�rios objetos com texturas e ilumina��o
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

struct Light
{
    XMFLOAT3 Direction;     // vetor na dire��o da luz
    float FalloffStart;     // inicio da atenua��o
    XMFLOAT3 Strength;      // pot�ncia da luz
    float FalloffEnd;       // limite da atenua��o
    XMFLOAT3 Position;      // posi��o da luz
    float SpotPower;        // pot�ncia do holofote
};

struct Scene
{
    XMFLOAT4X4 ViewProj;    // matriz de visualiza��o e proje��o
    XMFLOAT4 Ambient;       // cor da luz ambiente
    Light Lights[4];        // luzes da cena
    XMFLOAT3 Eye;           // vetor na dire��o do observador
    int Amb;                // luz ambiente
    int Dif;                // luz difusa
    int Spe;                // luz especular
    int Lam;                // �ngulo de incid�ncia
    int Mat;                // material dos objetos
    int Tex;                // textura dos objetos
};

struct Constants
{
    XMFLOAT4X4 World;       // matriz de mundo
    int ObjIndex;           // �ndice do objeto
};

struct Material
{
    XMFLOAT4 Albedo;        // cor 
    XMFLOAT3 Fresnel;       // refletividade
    float    Roughness;     // rugosidade
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
    ConstantBuffer<Constants>* cbuffer = nullptr;
    ConstantBuffer<Material>* material = nullptr;
    Texture* texture = nullptr;
};

// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------

class Desktop : public App
{
private:
    ID3D12RootSignature * rootSignature = nullptr;
    ID3D12PipelineState * pipelineState = nullptr;
    ID3D12PipelineState * pipelineWire = nullptr;
    ID3D12PipelineState * pipelineSolid = nullptr;
    
    ConstantBuffer<Scene>* sceneBuffer = nullptr;
    vector<Object> scene;
    OrbitCamera orbitcam;
    XMFLOAT4X4 Proj;

    static Timer timer;
    bool rotating = false;
    bool solid = true;

    XMFLOAT4X4 Identity
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // posi��o da luz
    float thetaL = XM_PIDIV4;
    float phiL = XM_PIDIV4;
    float radiusL = 8.0f;

    // intensidade da luz
    float StrengthL = 0.0f;
    float SpotStrength =0.0f;

    // controles
    bool Amb = true;
    bool Dif = true;
    bool Spe = true;
    bool Lam = true;
    bool Mat = true;
    bool Tex = true;

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();

    static void Pause() { timer.Stop(); }
    static void Resume() { timer.Start(); }

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
