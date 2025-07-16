/**********************************************************************************
// Multiple (Código Fonte)
//
// Criação:     27 Abr 2016
// Atualização: 22 Jun 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Cena 3D usando múltiplos buffers, um por objeto
//
**********************************************************************************/

#include "Multiple.h"
#include <windows.h>
#include <sstream>
#include <stdio.h>
#include<algorithm>

// ------------------------------------------------------------------------------

Timer Multiple::timer;

// ------------------------------------------------------------------------------

void Multiple::Init()
{
	// --------------------------------------
	// Transformação, Visualização e Projeção
	// --------------------------------------

	// posição inicial da câmera
	camera = { XM_PIDIV2, XM_PIDIV4, 5.0f };

	// inicializa a matriz de projeção
	XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		window->AspectRatio(),
		1.0f, 100.0f));

	// -------------------------------------------
	// Criação das Geometrias: Vértices e Índices
	// -------------------------------------------

	Box box(2.0f, 2.0f, 2.0f, Orange);
	Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20, Yellow);
	Sphere sphere(1.0f, 20, 20, Crimson);
	Grid grid(3.0f, 3.0f, 20, 20, Gray);

	// -------------------------
	// Definição dos Objetos 3D
	// -------------------------

	// box
	Object boxObj;
	boxObj.posicao = XMFLOAT3(-1.0f, 0.41f, 1.0f);
	boxObj.escala = XMFLOAT3(0.4f, 0.4f, 0.4f);
	boxObj.color = XMFLOAT4(1.0f, 0.55f, 0.0f, 1.0f); // Orange

	XMStoreFloat4x4(&boxObj.world,
		XMMatrixScaling(0.4f, 0.4f, 0.4f) *
		XMMatrixTranslation(-1.0f, 0.41f, 1.0f));
	boxObj.mesh = new Mesh(box);
	boxObj.vbuffer = new VertexBuffer<Vertex>(box);
	boxObj.ibuffer = new IndexBuffer<uint>(box);

	boxObj.cbuffer = new ConstantBuffer<Constants>();
	scene.push_back(boxObj);

	// cylinder
	Object cylinderObj;
	cylinderObj.posicao = XMFLOAT3(1.0f, 0.75f, -1.0f);
	cylinderObj.escala = XMFLOAT3(0.5f, 0.5f, 0.5f);
	cylinderObj.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
	XMStoreFloat4x4(&cylinderObj.world,
		XMMatrixScaling(0.5f, 0.5f, 0.5f) *
		XMMatrixTranslation(1.0f, 0.75f, -1.0f));
	cylinderObj.mesh = new Mesh(cylinder);
	cylinderObj.vbuffer = new VertexBuffer<Vertex>(cylinder);
	cylinderObj.ibuffer = new IndexBuffer<uint>(cylinder);

	cylinderObj.cbuffer = new ConstantBuffer<Constants>();
	scene.push_back(cylinderObj);

	// sphere
	Object sphereObj;
	sphereObj.posicao = XMFLOAT3(0.0f, 0.5f, 0.0f);
	sphereObj.escala = XMFLOAT3(0.5f, 0.5f, 0.5f);
	XMStoreFloat4x4(&sphereObj.world,
		XMMatrixScaling(0.5f, 0.5f, 0.5f) *
		XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	sphereObj.mesh = new Mesh(sphere);
	sphereObj.vbuffer = new VertexBuffer<Vertex>(sphere);
	sphereObj.ibuffer = new IndexBuffer<uint>(sphere);

	sphereObj.cbuffer = new ConstantBuffer<Constants>();
	scene.push_back(sphereObj);

	// grid da cena (não selecionável ou removível)
	gridObj.mesh = new Mesh(grid);

	XMStoreFloat4x4(&gridObj.world, XMMatrixIdentity());
	gridObj.vbuffer = new VertexBuffer<Vertex>(grid);
	gridObj.ibuffer = new IndexBuffer<uint>(grid);
	gridObj.cbuffer = new ConstantBuffer<Constants>();

	// Pensando o que fazer com a grid
	//scene.push_back(gridObj);


	// ---------------------

	BuildRootSignature();
	BuildPipelineState();

	// ----------------------

	timer.Start();
}

// ------------------------------------------------------------------------------

void Multiple::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	// --- Transformação do objeto selecionado ---
	if (hasValidSelection()) {
		Object& obj = scene[selectedIndex];
		bool alterado = false;
		float passoTrans = 0.01f; // passo de translação
		float passoEscala = 0.05f; // passo de escala
		float passoRot = XMConvertToRadians(5.0f); // passo de rotação em radianos

		// Translação com setas e PgUp/PgDn
		if (input->KeyDown(VK_LEFT)) { obj.posicao.x += passoTrans; alterado = true; }
		if (input->KeyDown(VK_RIGHT)) { obj.posicao.x -= passoTrans; alterado = true; }
		if (input->KeyDown(VK_UP)) { obj.posicao.z -= passoTrans; alterado = true; }
		if (input->KeyDown(VK_DOWN)) { obj.posicao.z += passoTrans; alterado = true; }
		if (input->KeyDown(VK_PRIOR)) { obj.posicao.y += passoTrans; alterado = true; } // PgUp
		if (input->KeyDown(VK_NEXT)) { obj.posicao.y -= passoTrans; alterado = true; } // PgDn

		// Escala uniforme com + e -
		if (input->KeyDown(VK_OEM_PLUS) || input->KeyDown('=')) {
			obj.escala.x += passoEscala;
			obj.escala.y += passoEscala;
			obj.escala.z += passoEscala;
			alterado = true;
		}

		if (input->KeyDown(VK_OEM_MINUS) || input->KeyDown('-')) {
			obj.escala.x = max(0.05f, obj.escala.x - passoEscala);
			obj.escala.y = max(0.05f, obj.escala.y - passoEscala);
			obj.escala.z = max(0.05f, obj.escala.z - passoEscala);
			alterado = true;
		}

		// Rotação com R + setas/Q/E
		if (input->KeyDown('R')) {
			if (input->KeyDown(VK_LEFT)) { obj.rotacao.y -= passoRot; alterado = true; } // Yaw-
			if (input->KeyDown(VK_RIGHT)) { obj.rotacao.y += passoRot; alterado = true; } // Yaw+
			if (input->KeyDown(VK_UP)) { obj.rotacao.x -= passoRot; alterado = true; } // Pitch-
			if (input->KeyDown(VK_DOWN)) { obj.rotacao.x += passoRot; alterado = true; } // Pitch+
			if (input->KeyDown('Q')) { obj.rotacao.z -= passoRot; alterado = true; } // Roll-
			if (input->KeyDown('E')) { obj.rotacao.z += passoRot; alterado = true; } // Roll+
		}

		// Limita a posição do objeto dentro dos limites do grid
		obj.posicao.x = std::clamp(obj.posicao.x, GRID_MIN_X, GRID_MAX_X);
		obj.posicao.y = std::clamp(obj.posicao.y, GRID_MIN_Y, GRID_MAX_Y);
		obj.posicao.z = std::clamp(obj.posicao.z, GRID_MIN_Z, GRID_MAX_Z);

		// Atualiza a matriz world a partir de posição, rotação e escala
		XMMATRIX S = XMMatrixScaling(obj.escala.x, obj.escala.y, obj.escala.z);
		XMMATRIX R = XMMatrixRotationRollPitchYaw(obj.rotacao.x, obj.rotacao.y, obj.rotacao.z);
		XMMATRIX T = XMMatrixTranslation(obj.posicao.x, obj.posicao.y, obj.posicao.z);
		XMStoreFloat4x4(&obj.world, S * R * T);

		// Imprime debug se houve alteração
		if (alterado) {
			printf("Transformacao do objeto %d:\n", selectedIndex);
			printf("  Posicao: (%.2f, %.2f, %.2f)\n", obj.posicao.x, obj.posicao.y, obj.posicao.z);
			printf("  Escala:  (%.2f, %.2f, %.2f)\n", obj.escala.x, obj.escala.y, obj.escala.z);
			printf("  Rotacao: (%.2f, %.2f, %.2f) graus\n", XMConvertToDegrees(obj.rotacao.x), XMConvertToDegrees(obj.rotacao.y), XMConvertToDegrees(obj.rotacao.z));
		}
	}

	// ativa ou desativa o giro do objeto
	if (input->KeyPress('S'))
	{
		isSpinning = !isSpinning;
		if (isSpinning)
			timer.Start();
		else
			timer.Stop();
	}

	if (input->KeyPress('V'))
	{
		isAltMode = !isAltMode;
	}

	// --- Adição de objeto ---
	if (input->KeyPress('B')) { // Box
		Box box(2.0f, 2.0f, 2.0f, Orange);
		Object obj;
		// Garantir que a posição inicial está dentro do grid
		obj.posicao = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.color = XMFLOAT4(1.0f, 0.55f, 0.0f, 1.0f); // Orange
		XMStoreFloat4x4(&obj.world, XMMatrixIdentity());		obj.mesh = new Mesh(box);
		obj.vbuffer = new VertexBuffer<Vertex>(box);
		obj.ibuffer = new IndexBuffer<uint>(box);
		obj.cbuffer = new ConstantBuffer<Constants>();
		scene.push_back(obj);
		selectedIndex = (int)scene.size() - 1;
		printf("Objeto inserido: %d\n", selectedIndex);
	}
	if (input->KeyPress('C')) { // Cylinder
		Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20, Yellow);
		Object obj;
		// Garantir que a posição inicial está dentro do grid
		obj.posicao = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		XMStoreFloat4x4(&obj.world, XMMatrixIdentity());		obj.mesh = new Mesh(cylinder);
		obj.vbuffer = new VertexBuffer<Vertex>(cylinder);
		obj.ibuffer = new IndexBuffer<uint>(cylinder);
		obj.cbuffer = new ConstantBuffer<Constants>();
		scene.push_back(obj);
		selectedIndex = (int)scene.size() - 1;
		printf("Objeto inserido: %d\n", selectedIndex);
	}
	if (input->KeyPress('S')) { // Sphere
		Sphere sphere(1.0f, 20, 20, Crimson);
		Object obj;
		// Garantir que a posição inicial está dentro do grid
		obj.posicao = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.color = XMFLOAT4(0.86f, 0.08f, 0.24f, 1.0f); // Crimson		XMStoreFloat4x4(&obj.world, XMMatrixIdentity());
		obj.mesh = new Mesh(sphere);
		obj.vbuffer = new VertexBuffer<Vertex>(sphere);
		obj.ibuffer = new IndexBuffer<uint>(sphere);
		obj.cbuffer = new ConstantBuffer<Constants>();
		scene.push_back(obj);
		selectedIndex = (int)scene.size() - 1;
		printf("Objeto inserido: %d\n", selectedIndex);
	}
	if (input->KeyPress('G')) { // GeoSphere
		GeoSphere geosphere(1.0f, 3, Blue);
		Object obj;
		// Garantir que a posição inicial está dentro do grid
		obj.posicao = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.color = XMFLOAT4(0.0f, 0.4f, 1.0f, 1.0f); // Blue		XMStoreFloat4x4(&obj.world, XMMatrixIdentity());
		obj.mesh = new Mesh(geosphere);
		obj.vbuffer = new VertexBuffer<Vertex>(geosphere);
		obj.ibuffer = new IndexBuffer<uint>(geosphere);
		obj.cbuffer = new ConstantBuffer<Constants>();
		scene.push_back(obj);
		selectedIndex = (int)scene.size() - 1;
		printf("Objeto inserido: %d\n", selectedIndex);
	}
	if (input->KeyPress('P')) { // Plane
		Grid plane(3.0f, 3.0f, 20, 20, Gray);
		Object obj;
		// Garantir que a posição inicial está dentro do grid
		obj.posicao = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // Gray		XMStoreFloat4x4(&obj.world, XMMatrixIdentity());
		obj.mesh = new Mesh(plane);
		obj.vbuffer = new VertexBuffer<Vertex>(plane);
		obj.ibuffer = new IndexBuffer<uint>(plane);
		obj.cbuffer = new ConstantBuffer<Constants>();
		scene.push_back(obj);
		selectedIndex = (int)scene.size() - 1;
		printf("Objeto inserido: %d\n", selectedIndex);
	}
	if (input->KeyPress('Q')) { // Quad
		Quad quad(2.0f, 2.0f, Green);
		Object obj;
		// Garantir que a posição inicial está dentro do grid
		obj.posicao = XMFLOAT3(0.0f, 0.0f, 0.0f);
		obj.color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // Green
		XMStoreFloat4x4(&obj.world, XMMatrixIdentity());
		obj.mesh = new Mesh(quad);
		obj.vbuffer = new VertexBuffer<Vertex>(quad);
		obj.ibuffer = new IndexBuffer<uint>(quad);
		obj.cbuffer = new ConstantBuffer<Constants>();
		scene.push_back(obj);
		selectedIndex = (int)scene.size() - 1;
		printf("Objeto inserido: %d\n", selectedIndex);
	}

	// TODO: Adicionar carregamento de modelos OBJ (teclas 1-5)

	// --- Printa informação de seleção ---
	static int prevSelectedIndex = -1;
	if (selectedIndex != prevSelectedIndex) {
		if (hasValidSelection())
			printf("Objeto selecionado: %d\n", selectedIndex);
		else
			printf("Nenhum objeto selecionado\n");
		prevSelectedIndex = selectedIndex;
	}
	// --- Seleção de objeto (TAB) ---
	if (input->KeyPress(VK_TAB) && !scene.empty()) {
		if (!hasValidSelection())
			selectedIndex = 0;
		else
			selectedIndex = (selectedIndex + 1) % (int)scene.size();
	}

	// --- Remoção de objeto selecionado (DEL) ---
	if (input->KeyPress(VK_DELETE) && hasValidSelection()) {
		// Libera recursos do objeto
		delete scene[selectedIndex].mesh;
		delete scene[selectedIndex].vbuffer;
		delete scene[selectedIndex].ibuffer;
		delete scene[selectedIndex].cbuffer;
		scene.erase(scene.begin() + selectedIndex);
		// Ajusta o índice selecionado
		if (scene.empty())
			selectedIndex = -1;
		else if (selectedIndex >= (int)scene.size())
			selectedIndex = (int)scene.size() - 1;
		printf("Objeto deletado. ");
		if (hasValidSelection())
			printf("Agora selecionado: %d\n", selectedIndex);
		else
			printf("Nenhum objeto selecionado\n");
	}

	// atualiza posição da câmera
	camera.Update();
}
// ------------------------------------------------------------------------------

void Multiple::Draw()
{
	// obtém o tamanho da janela
	float w = window->Width() / 2.0f;
	float h = window->Height() / 2.0f;

	XMMATRIX world;
	XMMATRIX WorldViewProj;
	Constants constants;
	// cria uma viewport para cada quadrante da janela
	D3D12_VIEWPORT viewports[4] = {
		{ 0.0f, 0.0f, w, h, 0.0f, 1.0f},    // Top-Left: Front
		{ w, 0.0f, w, h, 0.0f, 1.0f},      // Top-Right: Side
		{ 0.0f, h, w, h, 0.0f, 1.0f},      // Bottom-Left: Top
		{ w, h, w, h, 0.0f, 1.0f}          // Bottom-Right: Perspective
	};

	// cria uma scissor rect para cada quadrante da janela
	D3D12_RECT scissors[4] = {
		{ 0,        0,       (LONG)w,       (LONG)h },        // Top-Left: Front
		{ (LONG)w,  0,       (LONG)(w * 2), (LONG)h },        // Top-Right: Side
		{ 0,        (LONG)h, (LONG)w,       (LONG)(h * 2) },  // Bottom-Left: Top
		{ (LONG)w,  (LONG)h, (LONG)(w * 2), (LONG)(h * 2) }   // Bottom-Right: Perspective
	};
	// limpa o backbuffer
	graphics->Clear();

	// comandos de configuração comuns a todos os objetos
	graphics->CommandList()->SetPipelineState(pipelineState);
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float d = 5.0f; // distância da câmera
	XMMATRIX view, proj;

	if (isAltMode)
	{
		for (int i = 0; i < 4; i++) {
			graphics->CommandList()->RSSetViewports(1, &viewports[i]);
			graphics->CommandList()->RSSetScissorRects(1, &scissors[i]);

			// ajusta a matriz de visualização e projeção para cada viewport
			switch (i) {
			case 0: // Front
				XMVECTOR posFront = XMVectorSet(0.0f, 0.0f, -d, 1.0f);
				XMVECTOR targetFront = XMVectorZero();
				XMVECTOR upFront = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(posFront, targetFront, upFront);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);

				break;

			case 1: // Side
				XMVECTOR posSide = XMVectorSet(-d, 0.0f, 0.0f, 1.0f);
				XMVECTOR targetSide = XMVectorZero();
				XMVECTOR upSide = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(posSide, targetSide, upSide);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);
				break;

			case 2: // Top
				XMVECTOR posTop = XMVectorSet(0.0f, d, 0.0f, 1.0f);
				XMVECTOR targetTop = XMVectorZero();
				XMVECTOR upTop = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				view = XMMatrixLookAtLH(posTop, targetTop, upTop);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);
				break;

			case 3: // Perspective
				XMVECTOR posPersp = XMVectorSet(camera.x, camera.y, camera.z, 1.0f);
				XMVECTOR targetPersp = XMVectorZero();
				XMVECTOR upPersp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(posPersp, targetPersp, upPersp);
				proj = XMLoadFloat4x4(&Proj);
				break;
			}

			world = XMLoadFloat4x4(&gridObj.world);
			WorldViewProj = world * view * proj;
			XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
			gridObj.cbuffer->Copy(&constants);

			// Desenha a grid fixa por viewport
			graphics->CommandList()->SetGraphicsRootConstantBufferView(0, gridObj.cbuffer->View());
			graphics->CommandList()->IASetVertexBuffers(0, 1, gridObj.vbuffer->View());
			graphics->CommandList()->IASetIndexBuffer(gridObj.ibuffer->View());
			graphics->CommandList()->DrawIndexedInstanced(
				gridObj.mesh->indexCount, 1,
				gridObj.mesh->startIndex,
				gridObj.mesh->baseVertex,
				0);

			// Desenha os objetos na cena
			for (int j = 0; j < (int)scene.size(); ++j)
			{
				auto& obj = scene[j];
				XMMATRIX world = XMLoadFloat4x4(&obj.world);
				XMMATRIX WorldViewProj = world * view * proj;
				XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));

				if (j == selectedIndex)
					constants.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
				else
					constants.color = obj.color;
				obj.cbuffer->Copy(&constants);


				graphics->CommandList()->SetGraphicsRootConstantBufferView(0, obj.cbuffer->View());
				graphics->CommandList()->IASetVertexBuffers(0, 1, obj.vbuffer->View());
				graphics->CommandList()->IASetIndexBuffer(obj.ibuffer->View());
				graphics->CommandList()->DrawIndexedInstanced(
					obj.mesh->indexCount, 1,
					obj.mesh->startIndex,
					obj.mesh->baseVertex,
					0);
			}
		}
	}
	else
	{
		XMVECTOR posPersp = XMVectorSet(camera.x, camera.y, camera.z, 1.0f);
		XMVECTOR targetPersp = XMVectorZero();
		XMVECTOR upPersp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		view = XMMatrixLookAtLH(posPersp, targetPersp, upPersp);
		proj = XMLoadFloat4x4(&Proj);

		world = XMLoadFloat4x4(&gridObj.world);
		WorldViewProj = world * view * proj;
		XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
		gridObj.cbuffer->Copy(&constants);

		// Desenha a grid fixa por viewport
		graphics->CommandList()->SetGraphicsRootConstantBufferView(0, gridObj.cbuffer->View());
		graphics->CommandList()->IASetVertexBuffers(0, 1, gridObj.vbuffer->View());
		graphics->CommandList()->IASetIndexBuffer(gridObj.ibuffer->View());
		graphics->CommandList()->DrawIndexedInstanced(
			gridObj.mesh->indexCount, 1,
			gridObj.mesh->startIndex,
			gridObj.mesh->baseVertex,
			0);

		// Desenha os objetos na cena
		for (int j = 0; j < (int)scene.size(); ++j)
		{
			auto& obj = scene[j];
			XMMATRIX world = XMLoadFloat4x4(&obj.world);
			XMMATRIX WorldViewProj = world * view * proj;
			XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));

			if (j == selectedIndex)
				constants.color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			else
				constants.color = obj.color;
			obj.cbuffer->Copy(&constants);


			graphics->CommandList()->SetGraphicsRootConstantBufferView(0, obj.cbuffer->View());
			graphics->CommandList()->IASetVertexBuffers(0, 1, obj.vbuffer->View());
			graphics->CommandList()->IASetIndexBuffer(obj.ibuffer->View());
			graphics->CommandList()->DrawIndexedInstanced(
				obj.mesh->indexCount, 1,
				obj.mesh->startIndex,
				obj.mesh->baseVertex,
				0);
		}
	}
	// apresenta o backbuffer na tela
	graphics->Present();

}

// ------------------------------------------------------------------------------

void Multiple::Finalize()
{
	// espera GPU finalizar comandos pendentes
	graphics->WaitForGpu();

	// libera memória alocada
	rootSignature->Release();
	pipelineState->Release();

	for (auto& obj : scene)
	{
		delete obj.mesh;
		delete obj.vbuffer;
		delete obj.ibuffer;
		delete obj.cbuffer;
	}
	// Libera recursos do grid
	delete gridObj.mesh;
	delete gridObj.vbuffer;
	delete gridObj.ibuffer;
	delete gridObj.cbuffer;
}

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	AllocConsole();
	FILE* dummy;
	freopen_s(&dummy, "CONOUT$", "w", stdout); // Redireciona stdout para o console

	try
	{
		// cria motor e configura a janela
		Engine* engine = new Engine();
		engine->window->Mode(ASPECTRATIO);
		engine->window->Size(1024, 720);
		engine->window->Color(25, 25, 25);
		engine->window->Title("Multiple");
		engine->window->Icon("Icon");
		engine->window->LostFocus(Multiple::Pause);
		engine->window->InFocus(Multiple::Resume);
		engine->graphics->VSync(true);

		// cria e executa a aplicação
		engine->Start(new Multiple());

		// finaliza execução
		delete engine;
	}
	catch (Error& e)
	{
		// exibe mensagem em caso de erro
		MessageBox(nullptr, e.ToString().data(), "Multiple", MB_OK);
	}

	return 0;
}

// ----------------------------------------------------------------------------
