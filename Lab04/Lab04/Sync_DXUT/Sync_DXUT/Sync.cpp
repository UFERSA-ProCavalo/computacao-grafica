/**********************************************************************************
// Sync (C�digo Fonte)
//
// Cria��o:     19 Jan 2020
// Atualiza��o:	02 Mai 2025
// Compilador:	Visual C++ 2022
//
// Descri��o:   Sincroniza��o entre CPU e GPU
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

class Sync : public App
{
public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
};

// ------------------------------------------------------------------------------

void Sync::Init()
{

}

// ------------------------------------------------------------------------------

void Sync::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void Sync::Draw()
{
    
}

// ------------------------------------------------------------------------------

void Sync::Finalize()
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
        engine->window->Title("Sync");
        engine->window->Icon("Icon");
        
        // aplica��o pausa ao perder o foco
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplica��o
        engine->Start(new Sync());

        // destr�i motor
        delete engine;
    }
    catch (Error & e)
    {
        MessageBox(nullptr, e.ToString().data(), "Sync", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------

