/**********************************************************************************
// WinApp (Código Fonte)
//
// Criação:		11 Jan 2020
// Atualização:	29 Abr 2025
// Compilador:	Visual C++ 2022
//
// Descrição:	Exemplo de aplicação usando o framework DXUT
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
	int startX, startY, endX, endY;
	bool isDrawing;

public:
	void Init();
	void Update();
	void Display();
	void DrawLine();
	void Finalize();	
};

// ------------------------------------------------------------------------------

void WinApp::Init()
{
	isDrawing = false;

	// captura posição do mouse
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
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	if (input->KeyPress(VK_LEFT)) {
		if (!isDrawing) {
			startX = input->MouseX();
			startY = input->MouseY();
			isDrawing = true;
		}
		else {
			endX = input->MouseX();
			endY = input->MouseY();
			isDrawing = false;

			window->Clear();   // Full redraw still needed when drawing line
			Display();
			DrawLine();
		}
	}

	int mouseX = input->MouseX();
	int mouseY = input->MouseY();

	if (lastX != mouseX || lastY != mouseY)
	{
		// Update textMouse with new coordinates
		textMouse.str("");
		textMouse << mouseX << " x " << mouseY;

		// Use Win32 GDI to overwrite the old mouse coordinates text
		HWND hwnd = window->Id();
		HDC hdc = GetDC(hwnd);

		// Simulate clearing the region by drawing a filled rectangle over the old text
		HBRUSH brush = CreateSolidBrush(RGB(0, 122, 204)); // background color
		RECT rect = { 10, 100, 200, 120 }; // assumed text area (x, y, x+width, y+height)
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);

		// Draw new mouse coordinate text
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));
		TextOut(hdc, 10, 100, textMouse.str().c_str(), (int)textMouse.str().length());

		ReleaseDC(hwnd, hdc);
	}

	lastX = mouseX;
	lastY = mouseY;
}


// ------------------------------------------------------------------------------

void WinApp::Display()
{
	window->Print("Window App Demo", 10, 10, RGB(255, 255, 255));
	window->Print(textSize.str(), 10, 50, RGB(255, 255, 255));
	window->Print(textMode.str(), 10, 75, RGB(255, 255, 255));
	
}

void WinApp::DrawLine() {
	HWND hwnd = window->Id();
	HDC hdc = GetDC(hwnd);

	MoveToEx(hdc, startX, startY, NULL);
	LineTo(hdc, endX, endY);

	ReleaseDC(hwnd, hdc);
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
		engine->window->Title("Aplicação Windows");
		engine->window->Icon("Icon");

		// cria e executa a aplicação
		engine->Start(new WinApp());

		// destrói motor
		delete engine;
	}
	catch (Error& e)
	{
		MessageBox(nullptr, e.ToString().data(), "Aplicação Windows", MB_OK);
	}

	return 0;
}

// ----------------------------------------------------------------------------

