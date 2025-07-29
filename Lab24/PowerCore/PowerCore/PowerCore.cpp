/**********************************************************************************
// PowerCore (Código Fonte)
//
// Criação:     02 Out 2023
// Atualização: 08 Mar 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Realiza texturização de objetos 3D
//
**********************************************************************************/

#include "PowerCore.h"

// ------------------------------------------------------------------------------

Timer PowerCore::timer;

// ------------------------------------------------------------------------------

void PowerCore::Init()
{
    // -----------------------
    // Inicializações da Cena
    // -----------------------

    // posição inicial da câmera
    orbitcam = { 0, 1.1f, 8.0f };

    // inicializa a matriz de projeção
    XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        window->AspectRatio(),
        1.0f, 100.0f));

    // constant buffer
    sceneBuffer = new ConstantBuffer<Scene>();

    // ------------------------------
    // Geometria: Vértices e Índices
    // ------------------------------

    Cylinder cylinder(1.0f, 1.0f, 3.0f, 32, 1, OrangeRed);

    // ----------------------
    // Criação dos Materiais
    // ----------------------

    Material original;
    original.Albedo = XMFLOAT4(1.00f, 1.00f, 1.00f, 1.0f);
    original.Fresnel = XMFLOAT3(0.15f, 0.15f, 0.15f);
    original.Roughness = 0.8f;

    // --------------------
    // Criação dos Objetos 
    // --------------------

    Object obj;

    XMStoreFloat4x4(&obj.world, 
        XMMatrixRotationY(XM_PI) * 
        XMMatrixScaling(0.8f, 1.2f, 0.8f));
    obj.mesh = new Mesh(cylinder);
    obj.vbuffer = new VertexBuffer<Vertex>(cylinder);
    obj.ibuffer = new IndexBuffer<uint>(cylinder);
    obj.cbuffer = new ConstantBuffer<Constants>();
    obj.material = new ConstantBuffer<Material>(&original);
    obj.texture = new Texture("Resources/PowerCore.png");
    scene.push_back(obj);

    XMStoreFloat4x4(&obj.world, XMMatrixRotationY(XM_PI));
    obj.mesh = new Mesh(cylinder);
    obj.vbuffer = new VertexBuffer<Vertex>(cylinder);
    obj.ibuffer = new IndexBuffer<uint>(cylinder);
    obj.cbuffer = new ConstantBuffer<Constants>();
    obj.material = new ConstantBuffer<Material>(&original);
    obj.texture = new Texture("Resources/Warning.png");
    scene.push_back(obj);

    // ---------------------

    BuildRootSignature();
    BuildPipelineState();

    // ----------------------

    timer.Start();
}

// ------------------------------------------------------------------------------

void PowerCore::Update()
{
    // sai com o pressionamento do ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    // -----------------------
    // Controles da Aplicação
    // -----------------------

    // rotação do objeto
    if (input->KeyPress('R'))
    {
        rotating = !rotating;

        if (rotating)
            timer.Start();
        else
            timer.Stop();
    }

    // ------------------
    // Câmera e Projeção 
    // ------------------

    // movimenta a câmera com o mouse
    orbitcam.Update();

    // constrói a matriz de visualização
    XMVECTOR pos = XMVectorSet(orbitcam.x, orbitcam.y, orbitcam.z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    // carrega matriz de projeção
    XMMATRIX proj = XMLoadFloat4x4(&Proj);

    // constrói matriz combinada
    XMMATRIX ViewProj = view * proj;

    // vetor na direção do observador
    XMFLOAT3 Eye = XMFLOAT3(orbitcam.x, orbitcam.y, orbitcam.z);

    // -------------------------
    // Controles de Iluminação
    // -------------------------

    if (input->KeyPress('A'))
        Amb = !Amb;
    if (input->KeyPress('D'))
        Dif = !Dif;
    if (input->KeyPress('E'))
        Spe = !Spe;
    if (input->KeyPress('L'))
        Lam = !Lam;
    if (input->KeyPress('M'))
        Mat = !Mat;
    if (input->KeyPress('T'))
        Tex = !Tex;    

    float delta = float(2.0 * frameTime);

    // regula intensidade da luz
    if (input->KeyDown(VK_OEM_PLUS))
    {
        Strength += delta;
        if (Strength > 5.0f)
            Strength = 5.0f;
    }
    if (input->KeyDown(VK_OEM_MINUS))
    {
        Strength -= delta;
        if (Strength < 0.2f)
            Strength = 0.2f;
    }

    // movimenta luz direcional
    if (input->KeyDown(VK_LEFT))
        thetaL -= delta;
    if (input->KeyDown(VK_RIGHT))
        thetaL += delta;
    if (input->KeyDown(VK_UP))
        phiL -= delta;
    if (input->KeyDown(VK_DOWN))
        phiL += delta;

    // restringe o ângulo phi da luz entre ]0-180[ graus
    phiL = max(0.001f, min(XM_PI - 0.001f, phiL));

    // converte coordenadas esféricas para cartesianas
    float x = radiusL * sinf(phiL) * cosf(thetaL);
    float z = radiusL * sinf(phiL) * sinf(thetaL);
    float y = radiusL * cosf(phiL);

    // define direção da luz
    XMVECTOR lightDir = XMVector3Normalize(XMVectorSet(x, y, z, 0.0f));

    // ------------------
    // Constant Buffers 
    // ------------------

    Light light;
    XMStoreFloat3(&light.Direction, lightDir);
    light.Strength = XMFLOAT3(Strength, Strength, Strength);

    Scene sceneConstants;
    XMStoreFloat4x4(&sceneConstants.ViewProj, XMMatrixTranspose(ViewProj));
    sceneConstants.Ambient = XMFLOAT3(0.02f, 0.02f, 0.02f);
    sceneConstants.Lights = light;
    sceneConstants.Eye = Eye;
    sceneConstants.Amb = Amb;
    sceneConstants.Dif = Dif;
    sceneConstants.Spe = Spe;
    sceneConstants.Lam = Lam;
    sceneConstants.Mat = Mat;
    sceneConstants.Tex = Tex;
    sceneConstants.Elapsed = float(timer.Elapsed());

    // atualiza constant buffer da cena
    sceneBuffer->Copy(&sceneConstants);

    // atualiza constant buffer de cada objeto
    int i = 0;
    for (auto& obj : scene)
    {
        Constants constants;
        XMMATRIX World = XMLoadFloat4x4(&obj.world);
        XMStoreFloat4x4(&constants.World, XMMatrixTranspose(World));
        constants.ObjIndex = i++;
        obj.cbuffer->Copy(&constants);
    }
}

// ------------------------------------------------------------------------------


void PowerCore::Draw()
{
    // limpa o backbuffer
    graphics->Clear();
    
    // ajustes do pipeline para desenho do objeto
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // clindro fosco interno   
    graphics->CommandList()->SetPipelineState(pipelineSolid);
	graphics->CommandList()->SetDescriptorHeaps(1, scene[0].texture->Heap());
	graphics->CommandList()->IASetVertexBuffers(0, 1, scene[0].vbuffer->View());
	graphics->CommandList()->IASetIndexBuffer(scene[0].ibuffer->View());
	graphics->CommandList()->SetGraphicsRootDescriptorTable(0, scene[0].texture->Table());
	graphics->CommandList()->SetGraphicsRootConstantBufferView(1, sceneBuffer->View());
	graphics->CommandList()->SetGraphicsRootConstantBufferView(2, scene[0].cbuffer->View());
	graphics->CommandList()->SetGraphicsRootConstantBufferView(3, scene[0].material->View());
	graphics->CommandList()->DrawIndexedInstanced(
        scene[0].mesh->indexCount, 1,
        scene[0].mesh->startIndex,
        scene[0].mesh->baseVertex,
		0);

    // clindro transparente externo  
    graphics->CommandList()->SetPipelineState(pipelineCullNone);
    graphics->CommandList()->SetDescriptorHeaps(1, scene[1].texture->Heap());
    graphics->CommandList()->IASetVertexBuffers(0, 1, scene[1].vbuffer->View());
    graphics->CommandList()->IASetIndexBuffer(scene[1].ibuffer->View());
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, scene[1].texture->Table());
    graphics->CommandList()->SetGraphicsRootConstantBufferView(1, sceneBuffer->View());
    graphics->CommandList()->SetGraphicsRootConstantBufferView(2, scene[1].cbuffer->View());
    graphics->CommandList()->SetGraphicsRootConstantBufferView(3, scene[1].material->View());
    graphics->CommandList()->DrawIndexedInstanced(
        scene[1].mesh->indexCount, 1,
        scene[1].mesh->startIndex,
        scene[1].mesh->baseVertex,
        0);    
 
    // apresenta o backbuffer na tela
    graphics->Present();    
}

// ------------------------------------------------------------------------------

void PowerCore::Finalize()
{
    // espera GPU finalizar comandos pendentes
    graphics->WaitForGpu();

    // libera memória alocada
    rootSignature->Release();
    pipelineCullNone->Release();
    pipelineSolid->Release();

    for (auto& obj : scene)
    {
        delete obj.mesh;
        delete obj.vbuffer;
        delete obj.ibuffer;
        delete obj.cbuffer;
        delete obj.material;
        delete obj.texture;
    }

    delete sceneBuffer;
}

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(ASPECTRATIO);
        engine->window->Size(1024, 720);
        engine->window->Color(10, 10, 10);
        engine->window->Title("PowerCore");
        engine->window->Icon("Icon");
        engine->window->Cursor("Cursor");
        engine->window->LostFocus(PowerCore::Pause);
        engine->window->InFocus(PowerCore::Resume);

        // cria e executa a aplicação
        engine->Start(new PowerCore());

        // finaliza execução
        delete engine;
    }
    catch (Error& e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "PowerCore", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------


