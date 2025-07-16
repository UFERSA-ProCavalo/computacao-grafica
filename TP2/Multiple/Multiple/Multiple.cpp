/**********************************************************************************
// Multiple (Código Fonte)
//
// Criação:     27 Abr 2016
// Atualização: 22 Jun 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Cena 3D com múltiplos objetos, cada um com seu próprio buffer.
//
//
**********************************************************************************/

#include "Multiple.h"
#include <windows.h>
#include <sstream>
#include <stdio.h>
#include<algorithm>
#include <fstream>

// ------------------------------------------------------------------------------
//Função auxiliar para criar um objeto com geometria, cor, posição, escala e rotação
// Funcionamento: 
// - Cria um novo objeto
// - Define a geometria, cor, posição, escala e rotação do objeto
// - Cria buffers de vértices e índices para o objeto
// - Retorna o objeto criado

// Uso:
// Object obj = CreateObject(geometry, color, pos, scale, rot);
Object CreateObject(
	const Geometry& geometry,
	const XMFLOAT4& color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f),
	const XMFLOAT3& scale = XMFLOAT3(0.5f, 0.5f, 0.5f),
	const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f)
) {
	Object obj;
	obj.pos = pos;
	obj.scale = scale;
	obj.rot = rot;
	obj.color = color;
	XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMStoreFloat4x4(&obj.world, S * R * T);
	obj.mesh = new Mesh(geometry);
	obj.vbuffer = new VertexBuffer<Vertex>(geometry);
	obj.ibuffer = new IndexBuffer<uint>(geometry);
	obj.originalVertices = geometry.vertices;
	for (int i = 0; i < 4; ++i)
		obj.cbuffer[i] = new ConstantBuffer<Constants>();
	return obj;
}


// ------------------------------------------------------------------------------

// Atualiza a cor de todos os vértices do objeto
// Esta função percorre os vértices originais do objeto e atualiza a cor de cada um
// Funcionamento: 
// - Cria uma cópia dos vértices originais
// - Atualiza a cor de cada vértice na cópia
// - Copia os vértices atualizados de volta para o buffer

// Uso:
// SetObjectVertexColor(obj, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)); // Define a cor vermelha
void SetObjectVertexColor(Object& obj, const XMFLOAT4& color) {
	if (!obj.vbuffer || obj.originalVertices.empty()) return;
	size_t vertexCount = obj.originalVertices.size();
	std::vector<Vertex> tempVertices = obj.originalVertices;
	for (size_t i = 0; i < vertexCount; ++i) {
		tempVertices[i].color = color;
	}
	obj.vbuffer->Copy(tempVertices.data(), (uint)vertexCount);
}

// Função para carregar um arquivo OBJ e extrair vértices e índices
// Funcionamento: 
// - Lê cada linha do arquivo
// - Extrai vértices (v) e faces (f)
// - Converte as faces em índices de vértices
// - Armazena os vértices e índices em vetores
// - Imprime o número de vértices e faces carregados

// Uso:
// LoadObj("caminho/do/arquivo.obj", vertices, indices);
void LoadObj(const std::string& path, std::vector<XMFLOAT3>& vertices, std::vector<uint32_t>& indices) {
	std::ifstream file(path);
	if (!file.is_open()) {
		printf("Erro ao abrir o arquivo OBJ: %s\n", path.c_str());
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			vertices.emplace_back(x, y, z);
		}
		else if (type == "f") {
			std::vector<int> faceIndices;
			std::string vert;
			while (iss >> vert) {
				size_t slash = vert.find('/');
				int idx = std::stoi(vert.substr(0, slash)) - 1;
				faceIndices.push_back(idx);
			}
			for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
				indices.push_back(faceIndices[0]);
				indices.push_back(faceIndices[i]);
				indices.push_back(faceIndices[i + 1]);
			}
		}
	}
	printf("OBJ carregado: %s | Vertices: %d | Indices: %d\n", path.c_str(), (int)vertices.size(), (int)indices.size());
}

Timer Multiple::timer;

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

	// Linhas de separação do modo viewport
	LineVertex lineVerts[4] = {
		// linha vertical
		{ XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1,1,1,1) },
		{ XMFLOAT3(0.0f,  1.0f, 0.0f), XMFLOAT4(1,1,1,1) },
		// linha horizontal
		{ XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1,1,1,1) },
		{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1,1,1,1) },
	};
	lineVBuffer = new VertexBuffer<LineVertex>(lineVerts, 4);
	// Matriz identidade para constantes das linhas
	Constants lineConstants;
	XMStoreFloat4x4(&lineConstants.WorldViewProj, XMMatrixIdentity());
	lineConstants.color = XMFLOAT4(1, 1, 1, 1);
	lineCBuffer = new ConstantBuffer<Constants>();
	lineCBuffer->Copy(&lineConstants);

	Box box(2.0f, 2.0f, 2.0f, White);
	Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20, White);
	Sphere sphere(1.0f, 20, 20, White);
	Grid grid(3.0f, 3.0f, 20, 20, White);

	// -------------------------
	// Definição dos Objetos 3D
	// -------------------------

	// box
	Object boxObj = CreateObject(
		box,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, 0.41f, 1.0f),
		XMFLOAT3(0.4f, 0.4f, 0.5f),
		XMFLOAT3(0.0f, 0.0f, 0.0f));
	scene.push_back(boxObj);

	// cylinder
	Object cylinderObj = CreateObject(
		cylinder,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, 0.75f, -1.0f),
		XMFLOAT3(0.5f, 0.5f, 0.5f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	scene.push_back(cylinderObj);

	// sphere
	Object sphereObj = CreateObject(
		sphere,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f, 0.5f, 0.0f),
		XMFLOAT3(0.5f, 0.5f, 0.5f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
	scene.push_back(sphereObj);

	// grid da cena (não selecionável ou removível)
	gridObj.mesh = new Mesh(grid);

	XMStoreFloat4x4(&gridObj.world, XMMatrixTranslation(0.0f, -0.01f, 0.0f));
	gridObj.vbuffer = new VertexBuffer<Vertex>(grid);
	gridObj.ibuffer = new IndexBuffer<uint>(grid);
	for (int i = 0; i < 4; ++i)
		gridCBuffer[i] = new ConstantBuffer<Constants>();

	// Pensando o que fazer com a grid
	// Mas vou deixar como um objeto fixo, não removível, 
	// independente da cena
	//scene.push_back(gridObj);


	// ---------------------

	BuildRootSignature();
	BuildPipelineState();

	// ----------------------

	timer.Start();
}

// ------------------------------------------------------------------------------



// ------------------------------------------------------------------------------
// Processa entradas do usuário para manipulação, seleção, adição, remoção e importação de objetos.
// Atualiza transformações dos objetos selecionados.
// Teclas:
// ESC: Fecha
// TAB: Seleciona o próximo objeto
// DEL: Remove o objeto selecionado (com atraso de 2 frames)
// B: Adiciona um Box
// C: Adiciona um Cylinder
// D: Adiciona uma Sphere
// S: Adiciona um GeoSphere
// T: Adiciona um Plane
// Q: Adiciona um Quad
// 1-5: Importa objetos OBJ pré-definidos
// setas: Translada o objeto selecionado + pgUp/pgDn
// CTRL + setas: Rotaciona o objeto selecionado
// SHIFT + setas: Escala o objeto selecionado

// ------------------------------------------------------------------------------



// ------------------------------------------------------------------------------

void Multiple::Update()
{
	// =============================
	// Passo-a-passo do fluxo Update:
	// 1. Atualiza transformações do objeto selecionado (translação, escala, rotação)
	// 2. Processa comandos para adicionar, remover, importar e selecionar objetos
	// 3. Gerencia destaque visual do objeto selecionado
	// 4. Atualiza a câmera e faz remoção de recursos
	// =============================

	// 1. Fecha a janela se ESC for pressionado
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	// 2. Atualiza transformações do objeto selecionado
	if (hasValidSelection()) {
		Object& obj = scene[selectedIndex];
		bool alterado = false;
		float passoTrans = 0.01f; // passo de translação
		float passoEscala = 0.002f; // passo de escala
		float passoRot = XMConvertToRadians(1.5f); // passo de rotação em radianos

		// Translação: setas movimentam o objeto no grid (PgUp/PgDn para eixo Y)
		{
			if (input->KeyDown(VK_LEFT) && !input->KeyDown(VK_CONTROL) && !input->KeyDown(VK_SHIFT)) { obj.pos.x += passoTrans; alterado = true; }
			if (input->KeyDown(VK_RIGHT) && !input->KeyDown(VK_CONTROL) && !input->KeyDown(VK_SHIFT)) { obj.pos.x -= passoTrans; alterado = true; }
			if (input->KeyDown(VK_UP) && !input->KeyDown(VK_CONTROL) && !input->KeyDown(VK_SHIFT)) { obj.pos.z -= passoTrans; alterado = true; }
			if (input->KeyDown(VK_DOWN) && !input->KeyDown(VK_CONTROL) && !input->KeyDown(VK_SHIFT)) { obj.pos.z += passoTrans; alterado = true; }
			if (input->KeyDown(VK_PRIOR) && !input->KeyDown(VK_CONTROL) && !input->KeyDown(VK_SHIFT)) { obj.pos.y += passoTrans; alterado = true; } // PgUp
			if (input->KeyDown(VK_NEXT) && !input->KeyDown(VK_CONTROL) && !input->KeyDown(VK_SHIFT)) { obj.pos.y -= passoTrans; alterado = true; } // PgDn
		}
		// Escala uniforme: SHIFT + setas direita/esquerda
		{
			if (input->KeyDown(VK_SHIFT) && input->KeyDown(VK_RIGHT)) {
				obj.scale.x += passoEscala;
				obj.scale.y += passoEscala;
				obj.scale.z += passoEscala;
				alterado = true;
			}

			if (input->KeyDown(VK_SHIFT) && input->KeyDown(VK_LEFT)) {
				obj.scale.x = max(0.05f, obj.scale.x - passoEscala);
				obj.scale.y = max(0.05f, obj.scale.y - passoEscala);
				obj.scale.z = max(0.05f, obj.scale.z - passoEscala);
				alterado = true;
			}
		}

		// Rotação com R + setas/Q/E
		{
			if (input->KeyDown(VK_CONTROL)) {
				if (input->KeyDown(VK_LEFT)) { obj.rot.x -= passoRot; alterado = true; } // Yaw-
				if (input->KeyDown(VK_RIGHT)) { obj.rot.x += passoRot; alterado = true; } // Yaw+
				if (input->KeyDown(VK_UP)) { obj.rot.y -= passoRot; alterado = true; } // Pitch-
				if (input->KeyDown(VK_DOWN)) { obj.rot.y += passoRot; alterado = true; } // Pitch+
				if (input->KeyDown(VK_PRIOR)) { obj.rot.z -= passoRot; alterado = true; } // Roll-
				if (input->KeyDown(VK_NEXT)) { obj.rot.z += passoRot; alterado = true; } // Roll+
			}
		}

		//Ajuste das transformações e limitação pela grid
		{
			// Limita a posição do objeto dentro dos limites do grid
			obj.pos.x = std::clamp(obj.pos.x, GRID_MIN_X, GRID_MAX_X);
			obj.pos.y = std::clamp(obj.pos.y, GRID_MIN_Y, GRID_MAX_Y);
			obj.pos.z = std::clamp(obj.pos.z, GRID_MIN_Z, GRID_MAX_Z);

			// Atualiza a matriz world a partir de posição, rotação e escala
			XMMATRIX S = XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
			XMMATRIX R = XMMatrixRotationRollPitchYaw(obj.rot.x, obj.rot.y, obj.rot.z);
			XMMATRIX T = XMMatrixTranslation(obj.pos.x, obj.pos.y, obj.pos.z);
			XMStoreFloat4x4(&obj.world, S * R * T);
		}

		// Imprime debug se houve alteração
		if (alterado) {
			printf("Transformacao do objeto %d:\n", selectedIndex);
			printf("  Posicao: (%.2f, %.2f, %.2f)\n", obj.pos.x, obj.pos.y, obj.pos.z);
			printf("  Escala:  (%.2f, %.2f, %.2f)\n", obj.scale.x, obj.scale.y, obj.scale.z);
			printf("  Rotacao: (%.2f, %.2f, %.2f) graus\n", XMConvertToDegrees(obj.rot.x), XMConvertToDegrees(obj.rot.y), XMConvertToDegrees(obj.rot.z));
		}
	}

	if (input->KeyPress('V'))
	{
		isAltMode = !isAltMode;
	}

	// Adição de objeto (box, cylinder, sphere, geosphere, plane, quad
	{
		if (input->KeyPress('B')) { // Box
			Box box(2.0f, 2.0f, 2.0f, Orange);
			Object obj = CreateObject(box);
			scene.push_back(obj);
			selectedIndex = (int)scene.size() - 1;
			printf("Objeto inserido: %d\n", selectedIndex);
		}
		if (input->KeyPress('C')) { // Cylinder
			Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20, Yellow);
			Object obj = CreateObject(cylinder);
			scene.push_back(obj);
			selectedIndex = (int)scene.size() - 1;
			printf("Objeto inserido: %d\n", selectedIndex);
		}
		if (input->KeyPress('S')) { // Sphere
			Sphere sphere(1.0f, 20, 20, Crimson);
			Object obj = CreateObject(sphere);
			scene.push_back(obj);
			selectedIndex = (int)scene.size() - 1;
			printf("Objeto inserido: %d\n", selectedIndex);
		}
		if (input->KeyPress('G')) { // GeoSphere
			GeoSphere geosphere(1.0f, 3, Blue);
			Object obj = CreateObject(geosphere);
			scene.push_back(obj);
			selectedIndex = (int)scene.size() - 1;
			printf("Objeto inserido: %d\n", selectedIndex);
		}
		if (input->KeyPress('T')) { // Plane
			Grid plane(3.0f, 3.0f, 20, 20, Gray);
			Object obj = CreateObject(plane);
			scene.push_back(obj);
			selectedIndex = (int)scene.size() - 1;
			printf("Objeto inserido: %d\n", selectedIndex);
		}
		if (input->KeyPress('Q')) { // Quad
			Quad quad(2.0f, 2.0f, Green);
			Object obj = CreateObject(quad);
			scene.push_back(obj);
			selectedIndex = (int)scene.size() - 1;
			printf("Objeto inserido: %d\n", selectedIndex);
		}
	}

	// Importa objetos com as teclas 1-5
	{
		const char* objFiles[5] = {
			"Exemplos/ball.obj",
			"Exemplos/capsule.obj",
			"Exemplos/house.obj",
			"Exemplos/monkey.obj",
			"Exemplos/thorus.obj"
		};
		for (int k = 0; k < 5; ++k) {
			if (input->KeyPress('1' + k)) {
				std::vector<XMFLOAT3> vertices;
				std::vector<uint32_t> indices;
				LoadObj(objFiles[k], vertices, indices);
				if (!vertices.empty() && !indices.empty()) {
					Geometry geo;
					geo.vertices.reserve(vertices.size());
					for (const auto& v : vertices)
						geo.vertices.push_back(Vertex{ v, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
					geo.indices.assign(indices.begin(), indices.end());
					Object obj = CreateObject(geo);
					scene.push_back(obj);
					selectedIndex = (int)scene.size() - 1;
					printf("OBJ importado: %s (%d vértices, %d faces)\n", objFiles[k], (int)vertices.size(), (int)indices.size() / 3);
				}
				else {
					printf("Falha ao importar OBJ: %s\n", objFiles[k]);
				}
			}
		}
	}


	// --- Lógica de destaque para a seleção de objeto ---
	// TAB, DEL
	{
		if (selectedIndex != prevSelectedIndex) {
			// Guarda a seleção anterior
			if (prevSelectedIndex >= 0 && prevSelectedIndex < (int)scene.size()) {
				SetObjectVertexColor(scene[prevSelectedIndex], scene[prevSelectedIndex].color);
			}
			// Destaca o próximo
			if (hasValidSelection()) {
				SetObjectVertexColor(scene[selectedIndex], XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)); // Vermelho
				printf("Objeto selecionado: %d\n", selectedIndex);
			}
			else {
				printf("Nenhum objeto selecionado\n");
			}
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
			// Adiciona uma 'fila' de remoção
			pendingDeletion.push_back(scene[selectedIndex]);
			scene.erase(scene.begin() + selectedIndex);
			deletionDelay = 2;

			// Ajusta o índice selecionado
			if (scene.empty())
				selectedIndex = -1;
			else if (selectedIndex >= (int)scene.size())
				selectedIndex = (int)scene.size() - 1;
			printf("Objeto marcado para ser deletado. ");
			if (hasValidSelection())
				printf("Agora selecionado: %d\n", selectedIndex);
			else
				printf("Nenhum objeto selecionado\n");
			// Força atualização do highlight na próxima iteração
			prevSelectedIndex = -2;
		}
	}

	// atualiza posição da câmera
	camera.Update();

	// --- Deletar os objetos marcados no final ---
	{
		if (deletionDelay > 0) {
			--deletionDelay;
			if (deletionDelay == 0 && !pendingDeletion.empty()) {
				for (auto& obj : pendingDeletion) {
					delete obj.mesh;
					delete obj.vbuffer;
					delete obj.ibuffer;
					for (int i = 0; i < 4; ++i) {
						delete obj.cbuffer[i];
					}
				}
				pendingDeletion.clear();
			}
		}
	}
}

// ------------------------------------------------------------------------------
// Renderização da cena.
// Desenha a grid e todos os objetos, destacando o selecionado.
// Também tem a implementação das viewports (Front, Top, Left, Perspective).

//Deixei LEFT no lugar de RIGHT porque RIGHT e FRONT ficam visualmente iguais
// No modo alternativo, existem 4 viewports:
// 0: Front (topo-esquerdo)
// 1: Side (topo-direita)
// 2: Top (baixo-esquerdo)
// 3: Perspective (baixo-direita)
// ------------------------------------------------------------------------------



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
	//D3D12_RECT scissors[4] = {
	//	{ 0,        0,       (LONG)w,       (LONG)h },        // Top-Left: Front
	//	{ (LONG)w,  0,       (LONG)(w * 2), (LONG)h },        // Top-Right: Side
	//	{ 0,        (LONG)h, (LONG)w,       (LONG)(h * 2) },  // Bottom-Left: Top
	//	{ (LONG)w,  (LONG)h, (LONG)(w * 2), (LONG)(h * 2) }   // Bottom-Right: Perspective
	//};
	
	// limpa o backbuffer
	graphics->Clear();

	// comandos de configuração comuns a todos os objetos
	graphics->CommandList()->SetPipelineState(pipelineState);
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float d = 5.0f; // distância da câmera
	XMMATRIX view, proj;
	XMVECTOR pos;
	XMVECTOR target;
	XMVECTOR up;

	// --- Fluxo do modo com as 4 viewports ---
	if (isAltMode)
	{
		for (int i = 0; i < 4; i++) {
			graphics->CommandList()->RSSetViewports(1, &viewports[i]);
			// Queria usar scissors, mas não funciona no modo alt como eu queria,
			// pois eu teria que modificar as linhas de separação do viewport,
			// então vou deixar comentado
			//graphics->CommandList()->RSSetScissorRects(1, &scissors[i]);

			// ajusta a matriz de visualização e projeção para cada viewport
			switch (i) {
			case 0: // Front
				pos = XMVectorSet(0.0f, 0.001f, -d, 1.0f);
				target = XMVectorZero();
				up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(pos, target, up);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);

				break;

			case 1: // Side (Direita)
				/*XMVECTOR posSide = XMVectorSet(-d, 0.0f, 0.0f, 1.0f);
				XMVECTOR targetSide = XMVectorZero();
				XMVECTOR upSide = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(posSide, targetSide, upSide);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);
				break;*/


				//Esquerda
				pos = XMVectorSet(d, 0.001f, 0.0f, 1.0f);
				target = XMVectorZero();
				up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(pos, target, up);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);
				break;


			case 2: // Top
				pos = XMVectorSet(0.0f, d, 0.0f, 1.0f);
				target = XMVectorZero();
				up = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				view = XMMatrixLookAtLH(pos, target, up);
				proj = XMMatrixOrthographicLH(6.0f, 6.0f, 1.0f, 100.0f);
				break;

			case 3: // Perspective
				// camera.z sendo positivo, a camera fica em frente da cena (+z da origem, ou seja esquerda e direita invertida)
				// pra ver por trás (como um POV) só inverter o Z (-abs) 
				pos = XMVectorSet(camera.x, camera.y, camera.z, 1.0f);
				target = XMVectorZero();
				up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				view = XMMatrixLookAtLH(pos, target, up);
				proj = XMLoadFloat4x4(&Proj);
				break;
			}

			world = XMLoadFloat4x4(&gridObj.world);
			WorldViewProj = world * view * proj;
			XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
			constants.color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // Gray
			gridCBuffer[i]->Copy(&constants);

			// Desenha a grid fixa por viewport
			graphics->CommandList()->SetGraphicsRootConstantBufferView(0, gridCBuffer[i]->View());
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
				// Não atualiza cor dos vértices aqui; já feito em Update()
				XMMATRIX world = XMLoadFloat4x4(&obj.world);
				XMMATRIX WorldViewProj = world * view * proj;
				XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
				obj.cbuffer[i]->Copy(&constants);
				graphics->CommandList()->SetGraphicsRootConstantBufferView(0, obj.cbuffer[i]->View());
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

	// --- Fluxo do modo normal (apenas perspectiva) ---
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
		gridCBuffer[3]->Copy(&constants);

		// Desenha a grid fixa por viewport
		graphics->CommandList()->SetGraphicsRootConstantBufferView(0, gridCBuffer[3]->View());
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
			// Não atualiza cor dos vértices aqui; já feito em Update()
			XMMATRIX world = XMLoadFloat4x4(&obj.world);
			XMMATRIX WorldViewProj = world * view * proj;
			XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
			obj.cbuffer[3]->Copy(&constants);
			graphics->CommandList()->SetGraphicsRootConstantBufferView(0, obj.cbuffer[3]->View());
			graphics->CommandList()->IASetVertexBuffers(0, 1, obj.vbuffer->View());
			graphics->CommandList()->IASetIndexBuffer(obj.ibuffer->View());
			graphics->CommandList()->DrawIndexedInstanced(
				obj.mesh->indexCount, 1,
				obj.mesh->startIndex,
				obj.mesh->baseVertex,
				0);
		}
	}

	// Desenha as linhas de separação das viewports
	if (isAltMode && lineVBuffer && lineCBuffer) {
		D3D12_VIEWPORT fullViewport = { 0, 0, window->Width(), window->Height(), 0.0f, 1.0f };
		graphics->CommandList()->RSSetViewports(1, &fullViewport);

		graphics->CommandList()->SetGraphicsRootConstantBufferView(0, lineCBuffer->View());
		graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		graphics->CommandList()->IASetVertexBuffers(0, 1, lineVBuffer->View());
		graphics->CommandList()->DrawInstanced(4, 1, 0, 0);
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
		for (int i = 0; i < 4; ++i)
			delete obj.cbuffer[i];
	}
	// Libera recursos do grid
	delete gridObj.mesh;
	delete gridObj.vbuffer;
	delete gridObj.ibuffer;
	for (int i = 0; i < 4; ++i)
		delete gridCBuffer[i];
}

// ------------------------------------------------------------------------------



// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------



// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// Coloquei um console pra me ajudar a debugar,
	// Achei legal na implementação e resolvi deixar aqui msm
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
