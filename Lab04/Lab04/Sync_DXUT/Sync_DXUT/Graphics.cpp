/**********************************************************************************
// Graphics (Código Fonte)
// 
// Criação:		06 Abr 2011
// Atualização:	04 Mai 2025
// Compilador:	Visual C++ 2022
//
// Descrição:	Usa funções do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#include "Graphics.h"
#include "Error.h"
#include <sstream>
using std::wstringstream;

// ------------------------------------------------------------------------------

Graphics::Graphics()
{
	factory          = nullptr;
	device           = nullptr;
	commandQueue     = nullptr;
	commandList      = nullptr;
	commandAlloc     = nullptr;
	fence            = nullptr;
	fenceEvent       = nullptr;
	fenceValue       = 0;
}

// ------------------------------------------------------------------------------

Graphics::~Graphics()
{
	// espera GPU finalizar comandos na fila
	WaitForGpu();

	// libera sinalizador de eventos
	CloseHandle(fenceEvent);

	// libera barreira
	if (fence)
		fence->Release();

	// libera lista de comandos
	if (commandList)
		commandList->Release();

	// libera alocador de comandos
	if (commandAlloc)
		commandAlloc->Release();

	// libera fila de comandos
	if (commandQueue)
		commandQueue->Release();

	// libera dispositivo gráfico
	if (device)
		device->Release();

	// libera interface principal
	if (factory)
		factory->Release();
}

// ------------------------------------------------------------------------------

void Graphics::LogHardwareInfo()
{
	const uint BytesInMegaByte = 1048576U;

	// --------------------------------------
	// Adaptador de vídeo (placa de vídeo)
	// --------------------------------------
	IDXGIAdapter* adapter = nullptr;
	if (factory->EnumAdapters(0, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		wstringstream text;
		text << L"---> Placa de vídeo: " << desc.Description << L"\n";
		text << L"---> Memória de vídeo (dedicada): " << desc.DedicatedSystemMemory / BytesInMegaByte << L"MB\n";
		OutputDebugStringW(text.str().c_str());
	}

	IDXGIAdapter4* adapter4 = nullptr;
	if (SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter4))))
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
		adapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);
		
		wstringstream text;
		text << L"---> Memória de vídeo (livre): " << memInfo.Budget / BytesInMegaByte << L"MB\n";
		text << L"---> Memória de vídeo (usada): " << memInfo.CurrentUsage / BytesInMegaByte << L"MB\n";
		OutputDebugStringW(text.str().c_str());

		adapter4->Release();
	}	

	// -----------------------------------------
	// Feature Level máximo suportado pela GPU
	// -----------------------------------------
	D3D_FEATURE_LEVEL featureLevels[10] =
	{
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;
	featureLevelsInfo.NumFeatureLevels = 10;
	featureLevelsInfo.pFeatureLevelsRequested = featureLevels;

	device->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS,
		&featureLevelsInfo,
		sizeof(featureLevelsInfo));

	// bloco de instruções
	{
		wstringstream text;
		text << L"---> Feature Level: ";
		switch (featureLevelsInfo.MaxSupportedFeatureLevel)
		{
		case D3D_FEATURE_LEVEL_12_2: text << L"12_2\n"; break;
		case D3D_FEATURE_LEVEL_12_1: text << L"12_1\n"; break;
		case D3D_FEATURE_LEVEL_12_0: text << L"12_0\n"; break;
		case D3D_FEATURE_LEVEL_11_1: text << L"11_1\n"; break;
		case D3D_FEATURE_LEVEL_11_0: text << L"11_0\n"; break;
		case D3D_FEATURE_LEVEL_10_1: text << L"10_1\n"; break;
		case D3D_FEATURE_LEVEL_10_0: text << L"10_0\n"; break;
		case D3D_FEATURE_LEVEL_9_3:  text << L"9_3\n";  break;
		case D3D_FEATURE_LEVEL_9_2:  text << L"9_2\n";  break;
		case D3D_FEATURE_LEVEL_9_1:  text << L"9_1\n";  break;
		}
		OutputDebugStringW(text.str().c_str());
	}

	// -----------------------------------------
	// Saída de vídeo (monitor)
	// -----------------------------------------

	IDXGIOutput* output = nullptr;
	if (adapter->EnumOutputs(0, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		wstringstream text;
		text << L"---> Monitor: " << desc.DeviceName << L"\n";
		OutputDebugStringW(text.str().c_str());
	}

	// ------------------------------------------
	// Modo de vídeo (resolução)
	// ------------------------------------------

	// pega as dimensões da tela
	uint dpi = GetDpiForSystem();
	uint screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
	uint screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

	// pega a frequencia de atualização da tela
	DEVMODE devMode = { 0 };
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
	uint refresh = devMode.dmDisplayFrequency;

	wstringstream text;
	text << L"---> Resolução: " << screenWidth << L"x" << screenHeight << L" " << refresh << L" Hz\n";
	OutputDebugStringW(text.str().c_str());
	
	// ------------------------------------------

	// libera interfaces DXGI utilizadas
	if (adapter) adapter->Release();
	if (output) output->Release();
}


// -----------------------------------------------------------------------------

void Graphics::Initialize(Window * window)
{
	// ---------------------------------------------------
	// Cria a infraestrutura DXGI e o dispositivo D3D
	// ---------------------------------------------------

	uint factoryFlags = 0;

#ifdef _DEBUG
	// habilita a camada de depuração do D3D12
	ID3D12Debug * debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();

	// habilita a camada de depuração do DXGI
	factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	// cria objeto para infraestrutura gráfica do DirectX (DXGI)
	ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)));

	// cria objeto para dispositivo gráfico
	if FAILED(D3D12CreateDevice(
		nullptr,                                // adaptador de vídeo (nullptr = adaptador padrão)
		D3D_FEATURE_LEVEL_11_0,                 // versão mínima dos recursos do Direct3D
		IID_PPV_ARGS(&device)))                 // guarda o dispositivo D3D criado
	{
		// tenta criar um dispositivo WARP 
		IDXGIAdapter* warp;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));

		// cria objeto D3D usando dispositivo WARP
		ThrowIfFailed(D3D12CreateDevice(
			warp,                               // adaptador de vídeo WARP (software)
			D3D_FEATURE_LEVEL_11_0,             // versão mínima dos recursos do Direct3D
			IID_PPV_ARGS(&device)));            // guarda o dispositivo D3D criado

		// libera objeto não mais necessário
		warp->Release();

		// informa uso de um disposito WARP:
		// implementa as funcionalidades do 
		// D3D12 em software (lento)
		OutputDebugString("---> Usando Adaptador WARP: não há suporte ao D3D12\n");
	}

	// exibe informações do hardware gráfico no Output do Visual Studio
#ifdef _DEBUG
	LogHardwareInfo();
#endif 

	// ---------------------------------------------------
	// Cria fila, lista e alocador de commandos
	// ---------------------------------------------------

	// configura a fila de comandos da GPU
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	
	// cria fila de comandos da GPU
	ThrowIfFailed(device->CreateCommandQueue(
		&queueDesc,								// configuração da fila
		IID_PPV_ARGS(&commandQueue)));			// objeto fila de comandos

	// cria o alocador de comandos
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,			// tipo de lista de comandos
		IID_PPV_ARGS(&commandAlloc)));			// objeto alocador de comandos

	// cria a lista de comandos
	ThrowIfFailed(device->CreateCommandList(
		0,										// GPU a ser utilizada
		D3D12_COMMAND_LIST_TYPE_DIRECT,			// não herda estado na GPU
		commandAlloc,						    // alocador de comandos
		nullptr,								// estado inicial do pipeline
		IID_PPV_ARGS(&commandList)));			// objeto lista de comandos

	// ---------------------------------------------------
	// Cria cerca para sincronizar CPU/GPU
	// ---------------------------------------------------

	// cria cerca para sincronização
	ThrowIfFailed(device->CreateFence(
		0,                                      // valor inicial da cerca
		D3D12_FENCE_FLAG_NONE,                  // cerca padrão para uma GPU
		IID_PPV_ARGS(&fence)));                 // objeto cerca de sincronização
	
	// cria objeto para sinalização de eventos
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
}

// ------------------------------------------------------------------------------

void Graphics::WaitForGpu()
{
	// avança o valor da cerca para marcar novos comandos 
	fenceValue++;

	// adiciona uma cerca na fila de comandos
	ThrowIfFailed(commandQueue->Signal(fence, fenceValue));

	// espera a GPU completar todos os comandos
	if (fence->GetCompletedValue() < fenceValue)
	{
		// aciona evento quando a GPU atingir a cerca  
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));

		// espera até o evento ser acionado
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

// -----------------------------------------------------------------------------

void Graphics::SendToGpu()
{
	// envia a lista de comandos para execução na GPU
	commandList->Close();
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// espera até a GPU completar a execução dos comandos
	WaitForGpu();
}

// -----------------------------------------------------------------------------
