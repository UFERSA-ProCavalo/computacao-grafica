/**********************************************************************************
// Desktop (Código Fonte)
//
// Criação:     02 Out 2023
// Atualização: 03 Jul 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Desenha vários objetos com texturas e iluminação
//
**********************************************************************************/

#include "Desktop.h"

// ------------------------------------------------------------------------------

Timer Desktop::timer;

// ------------------------------------------------------------------------------

void Desktop::Init()
{
    // -----------------------
    // Inicializações da Cena
    // -----------------------

    // posição inicial da câmera
    orbitcam = { XM_PIDIV4, 1.1f, 12.0f };

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

    Box tabletop { 7.0f, 0.1f, 4.0f, SandyBrown };
    Box tableleg { 0.2f, 2.5f, 0.2f, SandyBrown };
    Box box { 2.0f, 0.4f, 1.5f, Yellow };
    Cylinder thread{ 0.02f, 0.02f, 8.0f, 3, 1, Gray };
    Cylinder cylinder { 0.5f, 0.1f, 0.4f, 20, 5, LightGray };
    Sphere sphere { 1.2f, 30, 20, Blue };
    Grid grid { 4.0f, 2.0f, 2, 2, LimeGreen };

    // --------------------------------------------
    // Mapeamento das texturas e cores dos objetos
    // --------------------------------------------

    // folhas da direita
    box.vertices[0].texc = XMFLOAT2(0.5343f, 1.00f);
    box.vertices[1].texc = XMFLOAT2(0.4475f, 1.00f);
    box.vertices[2].texc = XMFLOAT2(0.4475f, 0.00f);
    box.vertices[3].texc = XMFLOAT2(0.5343f, 0.00f);

    // espinha do livro
    box.vertices[4].color = XMFLOAT4(Colors::Crimson);
    box.vertices[5].color = XMFLOAT4(Colors::Crimson);
    box.vertices[6].color = XMFLOAT4(Colors::Crimson);
    box.vertices[7].color = XMFLOAT4(Colors::Crimson);

    box.vertices[4].texc = XMFLOAT2(0.3606f, 1.00f);  
    box.vertices[5].texc = XMFLOAT2(0.4453f, 1.00f);
    box.vertices[6].texc = XMFLOAT2(0.4453f, 0.00f);
    box.vertices[7].texc = XMFLOAT2(0.3606f, 0.00f);

    // folhas de baixo
    box.vertices[8].texc = XMFLOAT2(0.8968f, 0.90f);
    box.vertices[9].texc = XMFLOAT2(1.0000f, 0.90f);
    box.vertices[10].texc = XMFLOAT2(0.8968f, 0.00f);
    box.vertices[11].texc = XMFLOAT2(1.0000f, 0.00f);

    // folhas de cima
    box.vertices[12].texc = XMFLOAT2(1.0000f, 0.90f);
    box.vertices[13].texc = XMFLOAT2(0.8968f, 0.90f);
    box.vertices[14].texc = XMFLOAT2(1.0000f, 0.00f);
    box.vertices[15].texc = XMFLOAT2(0.8968f, 0.00f);

    // capa frontal
    box.vertices[16].color = XMFLOAT4(Colors::Crimson);
    box.vertices[17].color = XMFLOAT4(Colors::Crimson);
    box.vertices[18].color = XMFLOAT4(Colors::Crimson);
    box.vertices[19].color = XMFLOAT4(Colors::Crimson);

    box.vertices[16].texc = XMFLOAT2(0.3606f, 1.00f);  
    box.vertices[17].texc = XMFLOAT2(0.0000f, 1.00f);
    box.vertices[18].texc = XMFLOAT2(0.0000f, 0.00f);
    box.vertices[19].texc = XMFLOAT2(0.3606f, 0.00f);

    // capa traseira
    box.vertices[20].color = XMFLOAT4(Colors::Crimson);
    box.vertices[21].color = XMFLOAT4(Colors::Crimson);
    box.vertices[22].color = XMFLOAT4(Colors::Crimson);
    box.vertices[23].color = XMFLOAT4(Colors::Crimson);

    box.vertices[20].texc = XMFLOAT2(0.5343f, 1.00f);  
    box.vertices[21].texc = XMFLOAT2(0.8968f, 1.00f);  
    box.vertices[22].texc = XMFLOAT2(0.8968f, 0.00f);
    box.vertices[23].texc = XMFLOAT2(0.5343f, 0.00f);

    // ----------------------
    // Criação dos Materiais
    // ----------------------

    Material wood;
    wood.Albedo = XMFLOAT4(0.87f, 0.82f, 0.73f, 1.0f);
    wood.Fresnel = XMFLOAT3(0.02f, 0.02f, 0.02f);
    wood.Roughness = 0.80f;

    Material paper;
    paper.Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    paper.Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05f);
    paper.Roughness = 0.98f;

    Material metal;
    metal.Albedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    metal.Fresnel = XMFLOAT3(0.95f, 0.93f, 0.88f);
    metal.Roughness = 0.05f;

    Material glass;
    glass.Albedo = XMFLOAT4(1.00f, 1.00f, 1.00f, 1.0f);
    glass.Fresnel = XMFLOAT3(0.01f, 0.01f, 0.01f);
    glass.Roughness = 0.01f;

    Material cooper;
    cooper.Albedo = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
    cooper.Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05f);
    cooper.Roughness = 0.95f;
    
    // --------------------
    // Criação dos Objetos 
    // --------------------

    Object table;
    XMStoreFloat4x4(&table.world, 
        XMMatrixTranslation(0.0f, -0.05f, 0.0f));
    table.mesh = new Mesh(tabletop);
    table.vbuffer = new VertexBuffer<Vertex>(tabletop);
    table.ibuffer = new IndexBuffer<uint>(tabletop);
    table.cbuffer = new ConstantBuffer<Constants>();
    table.material = new ConstantBuffer<Material>(&wood);
    table.texture = new Texture("Resources/Table.jpg");
    scene.push_back(table);

    Object leg1;
    XMStoreFloat4x4(&leg1.world,
        XMMatrixTranslation(-3.0f, -1.30f, -1.5f));
    leg1.mesh = new Mesh(tableleg);
    leg1.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg1.ibuffer = new IndexBuffer<uint>(tableleg);
    leg1.cbuffer = new ConstantBuffer<Constants>();
    leg1.material = new ConstantBuffer<Material>(&wood);
    leg1.texture = new Texture("Resources/Table.jpg");
    scene.push_back(leg1);

    Object leg2;
    XMStoreFloat4x4(&leg2.world,
        XMMatrixTranslation(3.0f, -1.30f, -1.5f));
    leg2.mesh = new Mesh(tableleg);
    leg2.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg2.ibuffer = new IndexBuffer<uint>(tableleg);
    leg2.cbuffer = new ConstantBuffer<Constants>();
    leg2.material = new ConstantBuffer<Material>(&wood);
    leg2.texture = new Texture("Resources/Table.jpg");
    scene.push_back(leg2);

    Object leg3;
    XMStoreFloat4x4(&leg3.world,
        XMMatrixTranslation(-3.0f, -1.30f, 1.5f));
    leg3.mesh = new Mesh(tableleg);
    leg3.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg3.ibuffer = new IndexBuffer<uint>(tableleg);
    leg3.cbuffer = new ConstantBuffer<Constants>();
    leg3.material = new ConstantBuffer<Material>(&wood);
    leg3.texture = new Texture("Resources/Table.jpg");
    scene.push_back(leg3);

    Object leg4;
    XMStoreFloat4x4(&leg4.world,
        XMMatrixTranslation(3.0f, -1.30f, 1.5f));
    leg4.mesh = new Mesh(tableleg);
    leg4.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg4.ibuffer = new IndexBuffer<uint>(tableleg);
    leg4.cbuffer = new ConstantBuffer<Constants>();
    leg4.material = new ConstantBuffer<Material>(&wood);
    leg4.texture = new Texture("Resources/Table.jpg");
    scene.push_back(leg4);

    Object book1;
    XMStoreFloat4x4(&book1.world,
        XMMatrixScaling(0.8f, 0.8f, 0.8f) *
        XMMatrixRotationY(XMConvertToRadians(80.0f)) *
        XMMatrixTranslation(2.0f, 0.16f, 0.0f));
    book1.mesh = new Mesh(box);
    book1.vbuffer = new VertexBuffer<Vertex>(box);
    book1.ibuffer = new IndexBuffer<uint>(box);
    book1.cbuffer = new ConstantBuffer<Constants>();
    book1.material = new ConstantBuffer<Material>(&paper);
    book1.texture = new Texture("Resources/Book1.png");
    scene.push_back(book1);

    Object book2;
    XMStoreFloat4x4(&book2.world,
        XMMatrixScaling(0.8f, 0.8f, 0.8f) *
        XMMatrixRotationY(XMConvertToRadians(110.0f)) *
        XMMatrixTranslation(2.0f, 0.48f, 0.0f));
    book2.mesh = new Mesh(box);
    book2.vbuffer = new VertexBuffer<Vertex>(box);
    book2.ibuffer = new IndexBuffer<uint>(box);
    book2.cbuffer = new ConstantBuffer<Constants>();
    book2.material = new ConstantBuffer<Material>(&paper);
    book2.texture = new Texture("Resources/Book2.png");
    scene.push_back(book2);

    Object base;
    XMStoreFloat4x4(&base.world,
        XMMatrixRotationY(XM_PIDIV2) *
        XMMatrixTranslation(-2.0f, 0.15f, -1.0f));
    base.mesh = new Mesh(cylinder);
    base.vbuffer = new VertexBuffer<Vertex>(cylinder);
    base.ibuffer = new IndexBuffer<uint>(cylinder);
    base.cbuffer = new ConstantBuffer<Constants>();
    base.material = new ConstantBuffer<Material>(&metal);
    base.texture = new Texture("Resources/Metal.png");
    scene.push_back(base);

    Object globe;
    XMStoreFloat4x4(&globe.world, 
        XMMatrixTranslation(-2.0f, 1.5f, -1.0f));
    globe.mesh = new Mesh(sphere);
    globe.vbuffer = new VertexBuffer<Vertex>(sphere);
    globe.ibuffer = new IndexBuffer<uint>(sphere);
    globe.cbuffer = new ConstantBuffer<Constants>();
    globe.material = new ConstantBuffer<Material>(&paper);
    globe.texture = new Texture("Resources/Earth.jpg");
    scene.push_back(globe);

    Object map;
    XMStoreFloat4x4(&map.world,
        XMMatrixRotationY(XMConvertToRadians(15)) *
        XMMatrixTranslation(-0.8f, 0.01f, 0.0f));
    map.mesh = new Mesh(grid);
    map.vbuffer = new VertexBuffer<Vertex>(grid);
    map.ibuffer = new IndexBuffer<uint>(grid);
    map.cbuffer = new ConstantBuffer<Constants>();
    map.material = new ConstantBuffer<Material>(&paper);
    map.texture = new Texture("Resources/Sheet.png");
    scene.push_back(map);

    Object wiring;
    XMStoreFloat4x4(&wiring.world,
        XMMatrixTranslation(1.4f, 8.0f, 0.0f));
    wiring.mesh = new Mesh(thread);
    wiring.vbuffer = new VertexBuffer<Vertex>(thread);
    wiring.ibuffer = new IndexBuffer<uint>(thread);
    wiring.cbuffer = new ConstantBuffer<Constants>();
    wiring.material = new ConstantBuffer<Material>(&cooper);
    wiring.texture = new Texture("Resources/Plastic.jpg");
    scene.push_back(wiring);

    Object desklamp;
    XMStoreFloat4x4(&desklamp.world,
        XMMatrixTranslation(1.4f, 4.0f, 0.0f));
    desklamp.mesh = new Mesh(cylinder);
    desklamp.vbuffer = new VertexBuffer<Vertex>(cylinder);
    desklamp.ibuffer = new IndexBuffer<uint>(cylinder);
    desklamp.cbuffer = new ConstantBuffer<Constants>();
    desklamp.material = new ConstantBuffer<Material>(&cooper);
    desklamp.texture = new Texture("Resources/Plastic.jpg");
    scene.push_back(desklamp);

    Object lamp;
    XMStoreFloat4x4(&lamp.world,
        XMMatrixScaling(0.3f, 0.1f, 0.3f)*
        XMMatrixTranslation(1.4f, 3.9f, 0.0f));
    lamp.mesh = new Mesh(sphere);
    lamp.vbuffer = new VertexBuffer<Vertex>(sphere);
    lamp.ibuffer = new IndexBuffer<uint>(sphere);
    lamp.cbuffer = new ConstantBuffer<Constants>();
    lamp.material = new ConstantBuffer<Material>(&glass);
    lamp.texture = new Texture("Resources/Lamp.jpg");
    scene.push_back(lamp);

    // ---------------------

    BuildRootSignature();
    BuildPipelineState();

    // ----------------------

    timer.Start();
}

// ------------------------------------------------------------------------------

void Desktop::Update()
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

    // carrega matriz de projeção em uma XMMATRIX
    XMMATRIX proj = XMLoadFloat4x4(&Proj);

    // constrói matriz combinada (world x view x proj)
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

    // regula intensidade do holofote
    if (input->KeyDown(VK_OEM_PLUS))
    {
        SpotStrength += delta;
        if (SpotStrength > 5.0f)
            SpotStrength = 5.0f;
    }
    if (input->KeyDown(VK_OEM_MINUS))
    {
        SpotStrength -= delta;
        if (SpotStrength < 0.2f)
            SpotStrength = 0.2f;
    }

    // movimenta luz paralela
    if (input->KeyDown(VK_LEFT))
        thetaL -= delta;
    if (input->KeyDown(VK_RIGHT))
        thetaL += delta;

    // regula intensidade da luz paralela
    if (input->KeyDown(VK_UP))
    {
        StrengthL += 0.25f * delta;
        StrengthL = min(StrengthL, 2.0f);
    }
    if (input->KeyDown(VK_DOWN))
    {
        StrengthL -= 0.25f * delta;
        StrengthL = max(StrengthL, 0.0f);
    }

    // restringe o ângulo phi da luz entre ]0-180[ graus
    phiL = max(0.001f, min(XM_PI - 0.001f, phiL));

    // converte coordenadas esféricas para cartesianas
    float x = radiusL * sinf(phiL) * cosf(thetaL);
    float z = radiusL * sinf(phiL) * sinf(thetaL);
    float y = radiusL * cosf(phiL);

    // define direção da luz
    XMVECTOR LightDir = XMVector3Normalize(XMVectorSet(x, y, z, 0.0f));

    // vetor na direção da luz
    XMVECTOR SpotLightPos = XMVectorSet(0.0f, 3.9f, 0.0f, 1.0f);
    XMVECTOR SpotPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR SpotLightDir = XMVector3Normalize(SpotLightPos - SpotPos);

    // ------------------
    // Constant Buffers 
    // ------------------

    Light light;
    XMStoreFloat3(&light.Direction, LightDir);
    light.Strength = XMFLOAT3(StrengthL, StrengthL, StrengthL);

    Light spotLight;
    XMStoreFloat3(&spotLight.Direction, SpotLightDir);
    spotLight.Strength = XMFLOAT3(SpotStrength, SpotStrength, SpotStrength);
    spotLight.Position = XMFLOAT3(1.4f, 3.9f, 0.0f);
    spotLight.FalloffStart = 1.0f;
    spotLight.FalloffEnd = 10.0f;
    spotLight.SpotPower = 4.0f;

    Scene sceneConstants;
    XMStoreFloat4x4(&sceneConstants.ViewProj, XMMatrixTranspose(ViewProj));
    sceneConstants.Ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
    sceneConstants.Lights[0] = light;
    sceneConstants.Lights[1] = spotLight;
    sceneConstants.Eye = Eye;
    sceneConstants.Amb = Amb;
    sceneConstants.Dif = Dif;
    sceneConstants.Spe = Spe;
    sceneConstants.Lam = Lam;
    sceneConstants.Mat = Mat;
    sceneConstants.Tex = Tex;

    // atualiza constant buffer da cena
    sceneBuffer->Copy(&sceneConstants);

    // -------------------------
    // Atualiza Objetos na Cena
    // -------------------------

    if (rotating)
    {
        // faz o globo girar
        XMStoreFloat4x4(&scene[8].world,
            XMMatrixScaling(1.0f, 1.0f, 1.0f) *
            XMMatrixRotationY(float(timer.Elapsed())) *
            XMMatrixTranslation(-2.0f, 1.5f, -1.0f));
    }

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


void Desktop::Draw()
{
    // limpa o backbuffer
    graphics->Clear();

    // ajustes do pipeline para desenho do objeto
    graphics->CommandList()->SetPipelineState(pipelineState);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (auto& obj : scene)
    {
        graphics->CommandList()->SetDescriptorHeaps(1, obj.texture->Heap());
        graphics->CommandList()->IASetVertexBuffers(0, 1, obj.vbuffer->View());
        graphics->CommandList()->IASetIndexBuffer(obj.ibuffer->View());
        graphics->CommandList()->SetGraphicsRootDescriptorTable(0, obj.texture->Table());
        graphics->CommandList()->SetGraphicsRootConstantBufferView(1, sceneBuffer->View());
        graphics->CommandList()->SetGraphicsRootConstantBufferView(2, obj.cbuffer->View());
        graphics->CommandList()->SetGraphicsRootConstantBufferView(3, obj.material->View());

        // desenha objeto
        graphics->CommandList()->DrawIndexedInstanced(
            obj.mesh->indexCount, 1,
            obj.mesh->startIndex,
            obj.mesh->baseVertex,
            0);
    }

    // apresenta o backbuffer na tela
    graphics->Present();
}

// ------------------------------------------------------------------------------

void Desktop::Finalize()
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
        engine->window->Title("Desktop");
        engine->window->Icon("Icon");
        engine->window->Cursor("Cursor");
        engine->window->LostFocus(Desktop::Pause);
        engine->window->InFocus(Desktop::Resume);

        // cria e executa a aplicação
        engine->Start(new Desktop());

        // finaliza execução
        delete engine;
    }
    catch (Error& e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Desktop", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------


