/**********************************************************************************
// Texturing (Código Fonte)
//
// Criação:     02 Out 2023
// Atualização: 08 Mar 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Realiza texturização de objetos 3D
//
**********************************************************************************/

#include "Texturing.h"

// ------------------------------------------------------------------------------

Timer Texturing::timer;

// ------------------------------------------------------------------------------

void Texturing::Init()
{
    // -----------------------
    // Inicializações da Cena
    // -----------------------

    // posição inicial da câmera
    orbitcam = { XM_PIDIV4, 1.1f, 8.0f };

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

    Box box(2.0f, 2.0f, 2.0f, Wood);
    Cylinder cylinder(1.0f, 1.0f, 3.0f, 16, 5, OrangeRed);
    Sphere sphere(1.5f, 20, 20, White);
    Grid grid(8.0f, 5.0f, 10, 10, Gray);

    // ----------------------
    // Criação dos Materiais
    // ----------------------

    Material wood;
    wood.Albedo = XMFLOAT4(0.87f, 0.82f, 0.73f, 1.0f);
    wood.Fresnel = XMFLOAT3(0.15f, 0.15f, 0.15f);
    wood.Roughness = 0.8f;

    Material original;
    original.Albedo = XMFLOAT4(1.00f, 1.00f, 1.00f, 1.0f);
    original.Fresnel = XMFLOAT3(0.15f, 0.15f, 0.15f);
    original.Roughness = 0.8f;

    // --------------------
    // Criação dos Objetos 
    // --------------------

    Object obj;
    obj.world = Identity;
    obj.mesh = new Mesh(box);
    obj.vbuffer = new VertexBuffer<Vertex>(box);
    obj.ibuffer = new IndexBuffer<uint>(box);
    obj.cbuffer = new ConstantBuffer<Constants>();
    obj.material = new ConstantBuffer<Material>(&wood);
    obj.texture = new Texture("Resources/Box.jpg");
    scene.push_back(obj);

    obj.world = Identity;
    obj.mesh = new Mesh(cylinder);
    obj.vbuffer = new VertexBuffer<Vertex>(cylinder);
    obj.ibuffer = new IndexBuffer<uint>(cylinder);
    obj.cbuffer = new ConstantBuffer<Constants>();
    obj.material = new ConstantBuffer<Material>(&original);
    obj.texture = new Texture("Resources/Barrel.jpg");
    scene.push_back(obj);

    obj.world = Identity;
    obj.mesh = new Mesh(sphere);
    obj.vbuffer = new VertexBuffer<Vertex>(sphere);
    obj.ibuffer = new IndexBuffer<uint>(sphere);
    obj.cbuffer = new ConstantBuffer<Constants>();
    obj.material = new ConstantBuffer<Material>(&original);
    obj.texture = new Texture("Resources/Disco.png");
    scene.push_back(obj);

    obj.world = Identity;
    obj.mesh = new Mesh(grid);
    obj.vbuffer = new VertexBuffer<Vertex>(grid);
    obj.ibuffer = new IndexBuffer<uint>(grid);
    obj.cbuffer = new ConstantBuffer<Constants>();
    obj.material = new ConstantBuffer<Material>(&original);
    obj.texture = new Texture("Resources/Road.jpg");
    scene.push_back(obj);

    // ---------------------

    BuildRootSignature();
    BuildPipelineState();

    // ----------------------

    timer.Start();
}

// ------------------------------------------------------------------------------

void Texturing::Update()
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

    // modo sólido/wireframe
    if (input->KeyPress('S'))
    {
        solid = !solid;

        if (solid)
            pipelineState = pipelineSolid;
        else
            pipelineState = pipelineWire;
    }

    // passa de um objeto para outro
    if (input->KeyPress(VK_OEM_PERIOD))
    {
        objIndex++;
        objIndex %= 4;
    }
    if (input->KeyPress(VK_OEM_COMMA))
    {
        objIndex--;
        objIndex = objIndex < 0 ? 3 : objIndex;
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
    sceneConstants.Ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
    sceneConstants.Lights = light;
    sceneConstants.Eye = Eye;
    sceneConstants.Amb = Amb;
    sceneConstants.Dif = Dif;
    sceneConstants.Spe = Spe;
    sceneConstants.Lam = Lam;
    sceneConstants.Mat = Mat;
    sceneConstants.Tex = Tex;

    // atualiza constant buffer da cena
    sceneBuffer->Copy(&sceneConstants);

    // atualiza constant buffer de cada objeto
    int i = 0;
    for (auto& obj : scene)
    {
        if (rotating && i != 3)
        {
            // rotaciona objeto
            XMStoreFloat4x4(&obj.world,
                XMMatrixRotationY(float(timer.Elapsed())));
        }

        Constants constants;
        XMMATRIX World = XMLoadFloat4x4(&obj.world);
        XMStoreFloat4x4(&constants.World, XMMatrixTranspose(World));
        constants.ObjIndex = i++;
        obj.cbuffer->Copy(&constants);
    }
}

// ------------------------------------------------------------------------------


void Texturing::Draw()
{
    // limpa o backbuffer
    graphics->Clear();
    
    // ajustes do pipeline para desenho do objeto
    graphics->CommandList()->SetPipelineState(pipelineState);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->CommandList()->IASetVertexBuffers(0, 1, scene[objIndex].vbuffer->View());
    graphics->CommandList()->IASetIndexBuffer(scene[objIndex].ibuffer->View());
    graphics->CommandList()->SetDescriptorHeaps(1, scene[objIndex].texture->Heap());
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, scene[objIndex].texture->Table());
    graphics->CommandList()->SetGraphicsRootConstantBufferView(1, sceneBuffer->View());
    graphics->CommandList()->SetGraphicsRootConstantBufferView(2, scene[objIndex].cbuffer->View());
    graphics->CommandList()->SetGraphicsRootConstantBufferView(3, scene[objIndex].material->View());

    // desenha objeto
    graphics->CommandList()->DrawIndexedInstanced(
        scene[objIndex].mesh->indexCount, 1,
        scene[objIndex].mesh->startIndex,
        scene[objIndex].mesh->baseVertex,
        0);    
 
    // apresenta o backbuffer na tela
    graphics->Present();    
}

// ------------------------------------------------------------------------------

void Texturing::Finalize()
{
    // espera GPU finalizar comandos pendentes
    graphics->WaitForGpu();

    // libera memória alocada
    rootSignature->Release();
    pipelineWire->Release();
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
        engine->window->Color(25, 25, 25);
        engine->window->Title("Texturing");
        engine->window->Icon("Icon");
        engine->window->Cursor("Cursor");
        engine->window->LostFocus(Texturing::Pause);
        engine->window->InFocus(Texturing::Resume);

        // cria e executa a aplicação
        engine->Start(new Texturing());

        // finaliza execução
        delete engine;
    }
    catch (Error& e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Texturing", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------


