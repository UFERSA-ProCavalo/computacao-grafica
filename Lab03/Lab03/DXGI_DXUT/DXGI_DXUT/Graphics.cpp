/**********************************************************************************
// Graphics (C�digo Fonte)
// 
// Cria��o:		06 Abr 2011
// Atualiza��o:	11 Jul 2023
// Compilador:	Visual C++ 2022
//
// Descri��o:	Usa fun��es do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#include "Graphics.h"
#include "Error.h"
#include <sstream>
#include <dxgidebug.h>
using std::wstringstream;

// ------------------------------------------------------------------------------

Graphics::Graphics()
{
	factory = nullptr;
	device  = nullptr;
}

// ------------------------------------------------------------------------------

Graphics::~Graphics()
{
	// libera dispositivo gr�fico
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
	// Adaptador de v�deo (placa de v�deo)
	// --------------------------------------
	IDXGIAdapter* adapter = nullptr;
	if (factory->EnumAdapters(0, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		wstringstream text;
		text << L"---> Placa de v�deo: " << desc.Description << L"\n";
		text << L"---> Mem�ria de v�deo (dedicada): " << desc.DedicatedVideoMemory / BytesInMegaByte << L"MB\n";
		OutputDebugStringW(text.str().c_str());
	}

	IDXGIAdapter4* adapter4 = nullptr;
	if (SUCCEEDED(adapter->QueryInterface(IID_PPV_ARGS(&adapter4))))
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
		adapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memInfo);
		
		wstringstream text;
		text << L"---> Mem�ria de v�deo (livre): " << memInfo.Budget / BytesInMegaByte << L"MB\n";
		text << L"---> Mem�ria de v�deo (usada): " << memInfo.CurrentUsage / BytesInMegaByte << L"MB\n";
		OutputDebugStringW(text.str().c_str());

		adapter4->Release();
	}	

	// -----------------------------------------
	// Feature Level m�ximo suportado pela GPU
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

	// bloco de instru��es
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
	// Sa�da de v�deo (monitor)
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
	// Modo de v�deo (resolu��o)
	// ------------------------------------------

	// pega as dimens�es da tela
	uint dpi = GetDpiForSystem();
	uint screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
	uint screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

	// pega a frequencia de atualiza��o da tela
	DEVMODE devMode = { 0 };
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
	uint refresh = devMode.dmDisplayFrequency;

	wstringstream text;
	text << L"---> Resolu��o: " << screenWidth << L"x" << screenHeight << L" " << refresh << L" Hz\n";
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
	// habilita a camada de depura��o do DXGI
	factoryFlags = DXGI_CREATE_FACTORY_DEBUG;

	// habilita a camada de depura��o do D3D12
	ID3D12Debug * debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif

	// cria objeto para infraestrutura gr�fica do DirectX (DXGI)
	ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)));

	// cria objeto para dispositivo gr�fico
	if FAILED(D3D12CreateDevice(
		nullptr,                                // adaptador de v�deo (nullptr = adaptador padr�o)
		D3D_FEATURE_LEVEL_11_0,                 // vers�o m�nima dos recursos do Direct3D
		IID_PPV_ARGS(&device)))                 // guarda o dispositivo D3D criado
	{
		// tenta criar um dispositivo WARP 
		IDXGIAdapter* warp;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));

		// cria objeto D3D usando dispositivo WARP
		ThrowIfFailed(D3D12CreateDevice(
			warp,                               // adaptador de v�deo WARP (software)
			D3D_FEATURE_LEVEL_11_0,             // vers�o m�nima dos recursos do Direct3D
			IID_PPV_ARGS(&device)));            // guarda o dispositivo D3D criado

		// libera objeto n�o mais necess�rio
		warp->Release();

		// informa uso de um disposito WARP:
		// implementa as funcionalidades do 
		// D3D12 em software (lento)
		OutputDebugString("---> Usando Adaptador WARP: n�o h� suporte ao D3D12\n");
	}

#ifdef _DEBUG

	// ---------------------------------------------------
	// Exibe informa��es sobre o Hardware Gr�fico
	// ---------------------------------------------------
	LogHardwareInfo();

#endif 
}

// -----------------------------------------------------------------------------
