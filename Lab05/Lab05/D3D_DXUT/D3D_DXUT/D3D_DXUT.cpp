/**********************************************************************************
// D3D_DXUT (Código Fonte)
//
// Criação:     19 Jan 2020
// Atualização: 04 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Testa a inicialização do Direct3D 12
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

class D3DApp : public App
{
public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
};

// ------------------------------------------------------------------------------

void D3DApp::Init()
{ 
    OutputDebugString("Init\n");
}

// ------------------------------------------------------------------------------

void D3DApp::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void D3DApp::Draw()
{
    graphics->Clear(nullptr);

    // submete comandos de desenho aqui
    OutputDebugString("Draw ");


    graphics->Present();    
}

// ------------------------------------------------------------------------------

void D3DApp::Finalize()
{
    OutputDebugString("Exit\n");
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 600);
        engine->window->Color(0, 122, 204);
        engine->window->Title("Direct 3D");
        engine->window->Icon("Icon");
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        engine->Start(new D3DApp());

        // finaliza execução
        delete engine;
    }
    catch (Error & e)
    {
        MessageBox(nullptr, e.ToString().data(), "Direct 3D", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------

