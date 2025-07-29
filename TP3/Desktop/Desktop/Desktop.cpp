/**********************************************************************************
// Desktop (C�digo Fonte)
//
// Cria��o:     02 Out 2023
// Atualiza��o: 03 Jul 2025
// Compilador:  Visual C++ 2022
//
// Descri��o:   Desenha v�rios objetos com texturas e ilumina��o
//
**********************************************************************************/

#include "Desktop.h"

// ------------------------------------------------------------------------------

Timer Desktop::timer;

// ------------------------------------------------------------------------------

void Desktop::Init()
{
    // -----------------------
    // Inicializa��es da Cena
    // -----------------------

    // posi��o inicial da c�mera
    orbitcam = { XM_PIDIV4, 1.1f, 7.0f };

    // inicializa a matriz de proje��o
    XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        window->AspectRatio(),
        1.0f, 100.0f));

    // constant buffer
    sceneBuffer = new ConstantBuffer<Scene>();

    // ------------------------------
    // Geometria: Vértices e índices
    // ------------------------------

    Box wall { 12.0f, 5.0f, 0.2f, LightGray };
    Box tabletop { 7.0f, 0.1f, 4.0f, SandyBrown };
    Box tableleg { 0.2f, 2.5f, 0.2f, SandyBrown };
    Box box { 2.0f, 0.4f, 1.5f, Yellow };
    Cylinder thread{ 0.02f, 0.02f, 8.0f, 3, 1, Gray };
    Cylinder cylinder { 0.5f, 0.1f, 0.4f, 20, 5, LightGray };
    Sphere sphere { 1.2f, 30, 20, Blue };
    Grid grid { 4.0f, 2.0f, 2, 2, LimeGreen };

    // ----------------------
    // Cria��o dos Materiais
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

    // Wall
    Object wallObj;
    XMStoreFloat4x4(&wallObj.world,
        XMMatrixTranslation(0.0f, 1.5f, -3.0f));
    wallObj.mesh = new Mesh(wall);
    wallObj.vbuffer = new VertexBuffer<Vertex>(wall);
    wallObj.ibuffer = new IndexBuffer<uint>(wall);
    wallObj.cbuffer = new ConstantBuffer<Constants>();
    Material wallMat;
    wallMat.Albedo = XMFLOAT4(0.85f, 0.85f, 0.88f, 1.0f);
    wallMat.Fresnel = XMFLOAT3(0.02f, 0.02f, 0.02f);
    wallMat.Roughness = 0.05f;
    wallObj.material = new ConstantBuffer<Material>(&cooper);
    wallObj.texture = new Texture("Resources/Sheet.png"); // Use sheet as a generic wall texture
    scene.push_back(wallObj);

    // Table
    Object table;
    XMStoreFloat4x4(&table.world, 
        XMMatrixTranslation(0.0f, -0.05f, 0.0f));
    table.mesh = new Mesh(tabletop);
    table.vbuffer = new VertexBuffer<Vertex>(tabletop);
    table.ibuffer = new IndexBuffer<uint>(tabletop);
    table.cbuffer = new ConstantBuffer<Constants>();
    table.material = new ConstantBuffer<Material>(&wood);
    table.texture = new Texture("Resources/Madeira.jpg");
    scene.push_back(table);

    // Table legs
    Object leg1;
    XMStoreFloat4x4(&leg1.world,
        XMMatrixTranslation(-3.0f, -1.30f, -1.5f));
    leg1.mesh = new Mesh(tableleg);
    leg1.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg1.ibuffer = new IndexBuffer<uint>(tableleg);
    leg1.cbuffer = new ConstantBuffer<Constants>();
    leg1.material = new ConstantBuffer<Material>(&wood);
    leg1.texture = new Texture("Resources/Madeira.jpg");
    scene.push_back(leg1);

    Object leg2;
    XMStoreFloat4x4(&leg2.world,
        XMMatrixTranslation(3.0f, -1.30f, -1.5f));
    leg2.mesh = new Mesh(tableleg);
    leg2.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg2.ibuffer = new IndexBuffer<uint>(tableleg);
    leg2.cbuffer = new ConstantBuffer<Constants>();
    leg2.material = new ConstantBuffer<Material>(&wood);
    leg2.texture = new Texture("Resources/Madeira.jpg");
    scene.push_back(leg2);

    Object leg3;
    XMStoreFloat4x4(&leg3.world,
        XMMatrixTranslation(-3.0f, -1.30f, 1.5f));
    leg3.mesh = new Mesh(tableleg);
    leg3.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg3.ibuffer = new IndexBuffer<uint>(tableleg);
    leg3.cbuffer = new ConstantBuffer<Constants>();
    leg3.material = new ConstantBuffer<Material>(&wood);
    leg3.texture = new Texture("Resources/Madeira.jpg");
    scene.push_back(leg3);

    Object leg4;
    XMStoreFloat4x4(&leg4.world,
        XMMatrixTranslation(3.0f, -1.30f, 1.5f));
    leg4.mesh = new Mesh(tableleg);
    leg4.vbuffer = new VertexBuffer<Vertex>(tableleg);
    leg4.ibuffer = new IndexBuffer<uint>(tableleg);
    leg4.cbuffer = new ConstantBuffer<Constants>();
    leg4.material = new ConstantBuffer<Material>(&wood);
    leg4.texture = new Texture("Resources/Madeira.jpg");
    scene.push_back(leg4);

    // Computer case with per-face textures
    float caseW = 0.6f, caseH = 1.0f, caseD = 1.0f;
    XMFLOAT3 caseCenter = XMFLOAT3(-2.0f, 0.55f, 0.0f);
    float eps = 0.005f; // small offset for face thickness
    Material computerCaseMat;
    computerCaseMat.Albedo = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f);
    computerCaseMat.Fresnel = XMFLOAT3(0.04f, 0.04f, 0.04f);
    computerCaseMat.Roughness = 0.7f;

    // Front face
    Box caseFront { caseW, caseH, eps, LightGray };
    Object caseFrontObj;
    XMStoreFloat4x4(&caseFrontObj.world,
        XMMatrixTranslation(caseCenter.x, caseCenter.y, caseCenter.z + caseD/2.0f - eps/2.0f));
    caseFrontObj.mesh = new Mesh(caseFront);
    caseFrontObj.vbuffer = new VertexBuffer<Vertex>(caseFront);
    caseFrontObj.ibuffer = new IndexBuffer<uint>(caseFront);
    caseFrontObj.cbuffer = new ConstantBuffer<Constants>();
    caseFrontObj.material = new ConstantBuffer<Material>(&computerCaseMat);
    caseFrontObj.texture = new Texture("Resources/computer-front.jpg");
    scene.push_back(caseFrontObj);

    // Back face
    Box caseBack { caseW, caseH, eps, LightGray };
    Object caseBackObj;
    XMStoreFloat4x4(&caseBackObj.world,
        XMMatrixTranslation(caseCenter.x, caseCenter.y, caseCenter.z - caseD/2.0f + eps/2.0f));
    caseBackObj.mesh = new Mesh(caseBack);
    caseBackObj.vbuffer = new VertexBuffer<Vertex>(caseBack);
    caseBackObj.ibuffer = new IndexBuffer<uint>(caseBack);
    caseBackObj.cbuffer = new ConstantBuffer<Constants>();
    caseBackObj.material = new ConstantBuffer<Material>(&computerCaseMat);
    caseBackObj.texture = new Texture("Resources/computer-back.jpg");
    scene.push_back(caseBackObj);

    // Left face
    Box caseLeft { eps, caseH, caseD, LightGray };
    Object caseLeftObj;
    XMStoreFloat4x4(&caseLeftObj.world,
        XMMatrixTranslation(caseCenter.x - caseW/2.0f + eps/2.0f, caseCenter.y, caseCenter.z));
    caseLeftObj.mesh = new Mesh(caseLeft);
    caseLeftObj.vbuffer = new VertexBuffer<Vertex>(caseLeft);
    caseLeftObj.ibuffer = new IndexBuffer<uint>(caseLeft);
    caseLeftObj.cbuffer = new ConstantBuffer<Constants>();
    caseLeftObj.material = new ConstantBuffer<Material>(&computerCaseMat);
    caseLeftObj.texture = new Texture("Resources/computer-left.jpg");
    scene.push_back(caseLeftObj);

    // Right face
    Box caseRight { eps, caseH, caseD, LightGray };
    Object caseRightObj;
    XMStoreFloat4x4(&caseRightObj.world,
        XMMatrixTranslation(caseCenter.x + caseW/2.0f - eps/2.0f, caseCenter.y, caseCenter.z));
    caseRightObj.mesh = new Mesh(caseRight);
    caseRightObj.vbuffer = new VertexBuffer<Vertex>(caseRight);
    caseRightObj.ibuffer = new IndexBuffer<uint>(caseRight);
    caseRightObj.cbuffer = new ConstantBuffer<Constants>();
    caseRightObj.material = new ConstantBuffer<Material>(&computerCaseMat);
    caseRightObj.texture = new Texture("Resources/computer-right.jpg");
    scene.push_back(caseRightObj);

    // Top face (reuse Metal texture)
    Box caseTop { caseW, eps, caseD, LightGray };
    Object caseTopObj;
    XMStoreFloat4x4(&caseTopObj.world,
        XMMatrixTranslation(caseCenter.x, caseCenter.y + caseH/2.0f - eps/2.0f, caseCenter.z));
    caseTopObj.mesh = new Mesh(caseTop);
    caseTopObj.vbuffer = new VertexBuffer<Vertex>(caseTop);
    caseTopObj.ibuffer = new IndexBuffer<uint>(caseTop);
    caseTopObj.cbuffer = new ConstantBuffer<Constants>();
    caseTopObj.material = new ConstantBuffer<Material>(&computerCaseMat);
    caseTopObj.texture = new Texture("Resources/computer-front.jpg");
    scene.push_back(caseTopObj);

    // Bottom face (reuse Metal texture)
    Box caseBottom { caseW, eps, caseD, LightGray };
    Object caseBottomObj;
    XMStoreFloat4x4(&caseBottomObj.world,
        XMMatrixTranslation(caseCenter.x, caseCenter.y - caseH/2.0f + eps/2.0f, caseCenter.z));
    caseBottomObj.mesh = new Mesh(caseBottom);
    caseBottomObj.vbuffer = new VertexBuffer<Vertex>(caseBottom);
    caseBottomObj.ibuffer = new IndexBuffer<uint>(caseBottom);
    caseBottomObj.cbuffer = new ConstantBuffer<Constants>();
    caseBottomObj.material = new ConstantBuffer<Material>(&computerCaseMat);
    caseBottomObj.texture = new Texture("Resources/computer-front.jpg");
    scene.push_back(caseBottomObj);

    // Monitor base (simple box under monitor)
    Box monitorBaseBox { 0.3f, 1.0f, 0.2f, LightGray };
    Object monitorBase;
    XMStoreFloat4x4(&monitorBase.world,
        XMMatrixTranslation(0.5f, 0.04f, -1.0f));
    monitorBase.mesh = new Mesh(monitorBaseBox);
    monitorBase.vbuffer = new VertexBuffer<Vertex>(monitorBaseBox);
    monitorBase.ibuffer = new IndexBuffer<uint>(monitorBaseBox);
    monitorBase.cbuffer = new ConstantBuffer<Constants>();
    monitorBase.material = new ConstantBuffer<Material>(&metal);
    monitorBase.texture = new Texture("Resources/plastico-teste.jpg");
    scene.push_back(monitorBase);

    // Keyboard (flat box in front of monitor)
    Box keyboardBox { 1.6f, 0.05f, 0.5f, LightGray };
    Object keyboard;
    XMStoreFloat4x4(&keyboard.world,
        XMMatrixTranslation(0.5f, 0.1f, 1.0f));
    keyboard.mesh = new Mesh(keyboardBox);
    keyboard.vbuffer = new VertexBuffer<Vertex>(keyboardBox);
    keyboard.ibuffer = new IndexBuffer<uint>(keyboardBox);
    keyboard.cbuffer = new ConstantBuffer<Constants>();
    keyboard.material = new ConstantBuffer<Material>(&metal);
    keyboard.texture = new Texture("Resources/Plastic.jpg");
    scene.push_back(keyboard);

    // Keyboard face (top, as a thin box)
    Box keyboardFaceBox { 1.6f, 0.01f, 0.5f, White };
    Object keyboardFace;
    // Place the face so its bottom sits on top of the keyboard (center at y = 0.125f + 0.005f = 0.13f)
    XMStoreFloat4x4(&keyboardFace.world,
        XMMatrixTranslation(0.5f, 0.13f, 1.0f));
    keyboardFace.mesh = new Mesh(keyboardFaceBox);
    keyboardFace.vbuffer = new VertexBuffer<Vertex>(keyboardFaceBox);
    keyboardFace.ibuffer = new IndexBuffer<uint>(keyboardFaceBox);
    keyboardFace.cbuffer = new ConstantBuffer<Constants>();
    Material keyMat;
    keyMat.Albedo = XMFLOAT4(0.85f, 0.85f, 0.88f, 1.0f); // Slightly grayish white
    keyMat.Fresnel = XMFLOAT3(0.04f, 0.04f, 0.04f);
    keyMat.Roughness = 0.2f;
    keyboardFace.material = new ConstantBuffer<Material>(&keyMat);
    keyboardFace.texture = new Texture("Resources/Teclado-invertido.jpg");
    scene.push_back(keyboardFace);

    // Computer monitor
    Box monitorBox { 3.0f, 1.8f, 0.24f, LightGray };
    Object monitor;
    XMStoreFloat4x4(&monitor.world,
        XMMatrixTranslation(0.5f, 1.285f, -1.0f)); // Raise Y to match new height
    monitor.mesh = new Mesh(monitorBox);
    monitor.vbuffer = new VertexBuffer<Vertex>(monitorBox);
    monitor.ibuffer = new IndexBuffer<uint>(monitorBox);
    monitor.cbuffer = new ConstantBuffer<Constants>();
    Material monitorMat;
    monitorMat.Albedo = XMFLOAT4(0.2f, 0.2f, 0.25f, 1.0f);
    monitorMat.Fresnel = XMFLOAT3(0.08f, 0.08f, 0.08f);
    monitorMat.Roughness = 0.3f;
    monitor.material = new ConstantBuffer<Material>(&monitorMat);
    monitor.texture = new Texture("Resources/plastico-teste.jpg");
    scene.push_back(monitor);

    // Monitor screen (front face)
    Quad monitorScreenQuad { 2.8f, 1.6f, White };
    Object monitorScreen;
    // Place the screen just in front of the monitor box (monitor front is at z = -1.0f + 0.06f = -0.94f)
    XMStoreFloat4x4(&monitorScreen.world,
        XMMatrixTranslation(0.5f, 1.335f, -0.87f));
    monitorScreen.mesh = new Mesh(monitorScreenQuad);
    monitorScreen.vbuffer = new VertexBuffer<Vertex>(monitorScreenQuad);
    monitorScreen.ibuffer = new IndexBuffer<uint>(monitorScreenQuad);
    monitorScreen.cbuffer = new ConstantBuffer<Constants>();
    Material screenMat;
    screenMat.Albedo = XMFLOAT4(0.95f, 0.95f, 0.98f, 1.0f); // Slightly bluish white
    screenMat.Fresnel = XMFLOAT3(0.04f, 0.04f, 0.04f);
    screenMat.Roughness = 0.1f;
    monitorScreen.material = new ConstantBuffer<Material>(&screenMat);
    monitorScreen.texture = new Texture("Resources/Tela.jpg");
    scene.push_back(monitorScreen);

    // Chair seat (bigger)
    Box chairSeatBox { 1.5f, 0.25f, 1.5f, LightGray };
    Object chairSeat;
    XMStoreFloat4x4(&chairSeat.world,
        XMMatrixTranslation(0.0f, -1.3f, 2.5f));
    chairSeat.mesh = new Mesh(chairSeatBox);
    chairSeat.vbuffer = new VertexBuffer<Vertex>(chairSeatBox);
    chairSeat.ibuffer = new IndexBuffer<uint>(chairSeatBox);
    chairSeat.cbuffer = new ConstantBuffer<Constants>();
    Material chairMat;
    chairMat.Albedo = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    chairMat.Fresnel = XMFLOAT3(0.04f, 0.04f, 0.04f);
    chairMat.Roughness = 0.8f;
    chairSeat.material = new ConstantBuffer<Material>(&chairMat);
    chairSeat.texture = new Texture("Resources/plastico-teste.jpg");
    scene.push_back(chairSeat);

    // Chair legs (bigger and wider)
    Box chairLegBox { 0.1f, 1.2f, 0.1f, LightGray };
    float legX = 0.6f; // wider spacing
    float legZ1 = 1.9f, legZ2 = 3.1f;
    for (int i = 0; i < 4; ++i) {
        Object chairLeg;
        float x = (i < 2) ? -legX : legX;
        float z = (i % 2 == 0) ? legZ1 : legZ2;
        XMStoreFloat4x4(&chairLeg.world,
            XMMatrixTranslation(x, -2.0f, z));
        chairLeg.mesh = new Mesh(chairLegBox);
        chairLeg.vbuffer = new VertexBuffer<Vertex>(chairLegBox);
        chairLeg.ibuffer = new IndexBuffer<uint>(chairLegBox);
        chairLeg.cbuffer = new ConstantBuffer<Constants>();
        chairLeg.material = new ConstantBuffer<Material>(&chairMat);
        chairLeg.texture = new Texture("Resources/plastico-teste.jpg");
        scene.push_back(chairLeg);
    }

    // Chair backrest (bigger)
    Box chairBackBox { 1.5f, 1.2f, 0.1f, LightGray };
    Object chairBack;
    XMStoreFloat4x4(&chairBack.world,
        XMMatrixTranslation(0.0f, -0.6f, 3.2f));
    chairBack.mesh = new Mesh(chairBackBox);
    chairBack.vbuffer = new VertexBuffer<Vertex>(chairBackBox);
    chairBack.ibuffer = new IndexBuffer<uint>(chairBackBox);
    chairBack.cbuffer = new ConstantBuffer<Constants>();
    chairBack.material = new ConstantBuffer<Material>(&chairMat);
    chairBack.texture = new Texture("Resources/plastico-teste.jpg");
    scene.push_back(chairBack);


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
    // Controles da Aplica��o
    // -----------------------

    // rota��o do objeto
    if (input->KeyPress('R'))
    {
        rotating = !rotating;

        if (rotating)
            timer.Start();
        else
            timer.Stop();
    }

    // modo s�lido/wireframe
    if (input->KeyPress('S'))
    {
        solid = !solid;

        if (solid)
            pipelineState = pipelineSolid;
        else
            pipelineState = pipelineWire;
    }

    // ------------------
    // C�mera e Proje��o 
    // ------------------

    // movimenta a c�mera com o mouse
    orbitcam.Update();

    // constr�i a matriz de visualiza��o
    XMVECTOR pos = XMVectorSet(orbitcam.x, orbitcam.y, orbitcam.z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    // carrega matriz de proje��o em uma XMMATRIX
    XMMATRIX proj = XMLoadFloat4x4(&Proj);

    // constr�i matriz combinada (world x view x proj)
    XMMATRIX ViewProj = view * proj;

    // vetor na dire��o do observador
    XMFLOAT3 Eye = XMFLOAT3(orbitcam.x, orbitcam.y, orbitcam.z);

    // -------------------------
    // Controles de Ilumina��o
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

    // converte coordenadas esf�ricas para cartesianas
    float x = radiusL * sinf(phiL) * cosf(thetaL);
    float z = radiusL * sinf(phiL) * sinf(thetaL);
    float y = radiusL * cosf(phiL);

    // define dire��o da luz
    XMVECTOR LightDir = XMVector3Normalize(XMVectorSet(x, y, z, 0.0f));

    // vetor na dire��o da luz
    XMVECTOR SpotLightPos = XMVectorSet(0.0f, 3.9f, 0.0f, 1.0f);
    XMVECTOR SpotPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR SpotLightDir = XMVector3Normalize(SpotLightPos - SpotPos);

    // ------------------
    // Constant Buffers 
    // ------------------

    // Monitor light (SpotLight)
    Light monitorLight;
    monitorLight.Position = XMFLOAT3(0.5f, 0.5f, 1.94f); // Centered, just in front of monitor screen
    monitorLight.Strength = XMFLOAT3(0.5f, 0.5f, 5.5f); // Strong blueish-white
    monitorLight.FalloffStart = 0.1f;
    monitorLight.FalloffEnd = 3.0f;
    monitorLight.Direction = XMFLOAT3(0.0f, 0.0f, 1.0f); // Points forward
    monitorLight.SpotPower = 8.0f; // Narrow cone for monitor

    // Case light (PointLight)
    Light caseLight;
    caseLight.Position = XMFLOAT3(-1.4f, 0.75f, 0.0f); // Left of computer case
    caseLight.Strength = XMFLOAT3(0.6f, 0.6f, 3.0f); // Warm white
    caseLight.FalloffStart = 0.01f;
    caseLight.FalloffEnd = 1.0f;
    caseLight.Direction = XMFLOAT3(0.0f, 0.0f, 0.0f); // Not used for point
    caseLight.SpotPower = 0.0f; // Not used for point

    // Directional light 1 (top-left)
    Light dirLight1;
    dirLight1.Position = XMFLOAT3(0.0f, 0.0f, 0.0f); // Not used for directional
    dirLight1.Strength = XMFLOAT3(0.38f, 0.38f, 0.38f);
    dirLight1.FalloffStart = 0.0f;
    dirLight1.FalloffEnd = 0.0f;
    dirLight1.Direction = XMFLOAT3(-1.0f, 0.0f, -0.9f); // Diagonal from top-left
    dirLight1.SpotPower = 1.0f;

    // Directional light 2 (bottom-right)
    Light dirLight2;
    dirLight2.Position = XMFLOAT3(0.0f, 0.0f, 0.0f); // Not used for directional
    dirLight2.Strength = XMFLOAT3(0.8f, 0.8f, 0.8f);
    dirLight2.FalloffStart = 0.0f;
    dirLight2.FalloffEnd = 0.0f;
    dirLight2.Direction = XMFLOAT3(1.0f, 1.0f, 0.6f); // Diagonal from bottom-right
    dirLight2.SpotPower = 1.0f;

    Scene sceneConstants;
    XMStoreFloat4x4(&sceneConstants.ViewProj, XMMatrixTranspose(ViewProj));
    sceneConstants.Ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
    sceneConstants.Lights[0] = monitorLight;
    sceneConstants.Lights[1] = caseLight;
    sceneConstants.Lights[2] = dirLight1;
    sceneConstants.Lights[3] = dirLight2;
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

    // libera mem�ria alocada
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
        engine->graphics->VSync(true);


        // cria e executa a aplica��o
        engine->Start(new Desktop());
        

        // finaliza execu��o
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


