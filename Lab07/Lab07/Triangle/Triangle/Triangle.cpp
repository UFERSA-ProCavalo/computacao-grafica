/**********************************************************************************
// Triangle (Código Fonte)
//
// Criação:     22 Jul 2020
// Atualização: 16 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Constrói um Triângulo usando o Direct3D 12
//
**********************************************************************************/

#include "Triangle.h"

// ------------------------------------------------------------------------------

void Triangle::Init()
{
    // -------------------
    // Carrega Geometria
    // -------------------
    
    // quantidade de vértices na geometria
    const uint VertexCount = 3;

    // vértices da geometria
    Vertex vertices[VertexCount] =
    {
        { XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Orange) },
        { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Yellow) }
    };

    // cria vertex buffer
    vbuffer = new VertexBuffer<Vertex>(vertices, VertexCount);

    // ------------------
    // Ajusta Pipeline
    // ------------------

    BuildRootSignature();
    BuildPipelineState();  

    // ------------------

    graphics->SendToGpu();
}

// ------------------------------------------------------------------------------

void Triangle::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();
}

// ------------------------------------------------------------------------------

void Triangle::Display()
{
    // limpa backbuffer
    graphics->Clear(pipelineState);
    
    // submete comandos de configuração do pipeline
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetVertexBuffers(0, 1, vbuffer->View());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // submete comando de desenho
    graphics->CommandList()->DrawInstanced(3, 1, 0, 0);

    // apresenta backbuffer
    graphics->Present();    
}

// ------------------------------------------------------------------------------

void Triangle::Finalize()
{
    // espera GPU finalizar comandos pendentes
    graphics->WaitForGpu();

    // libera memória alocada
    rootSignature->Release();
    pipelineState->Release();
    delete vbuffer;
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
        engine->window->Size(600, 600);
        engine->window->Color(0, 122, 204);
        engine->window->Title("Triangle");
        engine->window->Icon("Icon");
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        engine->Start(new Triangle());

        // finaliza execução
        delete engine;
    }
    catch (Error & e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Triangle", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------

