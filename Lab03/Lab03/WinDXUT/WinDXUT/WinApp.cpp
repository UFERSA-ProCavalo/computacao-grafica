/**********************************************************************************
// WinApp (C�digo Fonte)
//
// Cria��o:		11 Jan 2020
// Atualiza��o:	29 Abr 2025
// Compilador:	Visual C++ 2022
//
// Descri��o:	Exemplo de aplica��o usando o framework DXUT
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

#include <sstream>
using std::stringstream;

class WinApp : public App
{
private:
	stringstream textSize;
	stringstream textMode;
	stringstream textMouse;

	int lastX, lastY;

public:
	void Init();
	void Update();
	void Display();
	void Finalize();	
};

// ------------------------------------------------------------------------------

void WinApp::Init()
{
	// captura posi��o do mouse
	lastX = input->MouseX();
	lastY = input->MouseY();

	// captura tamanho e tipo de janela
	textSize << "Tamanho: " << window->Width() << " x " << window->Height();
	textMode << "Formato: " << (window->Mode() == WINDOWED ? "Em Janela" : "Tela Cheia");
	textMouse << input->MouseX() << " x " << input->MouseY();
}

// ------------------------------------------------------------------------------

void WinApp::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	// converte posi��o do mouse em texto para exibi��o
	textMouse.str("");
	textMouse << input->MouseX() << " x " << input->MouseY();

	// se o mouse mudou de posi��o
	if (lastX != input->MouseX() || lastY != input->MouseY())
	{
		window->Clear();
		Display();
	}	

	// atualiza posi��o do mouse
	lastX = input->MouseX();
	lastY = input->MouseY();
}

// ------------------------------------------------------------------------------

void WinApp::Display()
{
	window->Print("Window App Demo", 10, 10, RGB(255, 255, 255));
	window->Print(textSize.str(), 10, 50, RGB(255, 255, 255));
	window->Print(textMode.str(), 10, 75, RGB(255, 255, 255));
	window->Print(textMouse.str(), 10, 100, RGB(255, 255, 255));
}

// ------------------------------------------------------------------------------

void WinApp::Finalize()
{
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	                 _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	try
	{
		// cria motor 
		Engine* engine = new Engine();

		// configura janela
		engine->window->Mode(WINDOWED);
		engine->window->Size(1024, 600);
		engine->window->Color(0, 122, 204);
		engine->window->Title("Aplica��o Windows");
		engine->window->Icon("Icon");

		// cria e executa a aplica��o
		engine->Start(new WinApp());

		// destr�i motor
		delete engine;
	}
	catch (Error& e)
	{
		MessageBox(nullptr, e.ToString().data(), "Aplica��o Windows", MB_OK);
	}

	return 0;
}

// ----------------------------------------------------------------------------

