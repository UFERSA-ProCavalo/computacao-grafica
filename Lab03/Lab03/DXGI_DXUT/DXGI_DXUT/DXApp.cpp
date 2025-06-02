/**********************************************************************************
// DXApp (C�digo Fonte)
//
// Cria��o:     19 Jan 2020
// Atualiza��o:	29 Abr 2025
// Compilador:	Visual C++ 2022
//
// Descri��o:   Testa a infraestrutura gr�fica do DirectX
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

class DXApp : public App
{
public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
};

// ------------------------------------------------------------------------------

void DXApp::Init()
{

}

// ------------------------------------------------------------------------------

void DXApp::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void DXApp::Draw()
{
    
}

// ------------------------------------------------------------------------------

void DXApp::Finalize()
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

        // configura a janela
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 600);
        engine->window->Color(0, 122, 204);
        engine->window->Title("DX App");
        engine->window->Icon("Icon");
        
        // aplica��o pausa ao perder o foco
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplica��o
        engine->Start(new DXApp());

        // destr�i motor
        delete engine;
    }
    catch (Error & e)
    {
        MessageBox(nullptr, e.ToString().data(), "DX App", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------

