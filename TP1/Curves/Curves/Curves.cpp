/**********************************************************************************
// Curves (Código Fonte)
//
// Criação:     12 Ago 2020
// Atualização: 21 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Base para gerar curvas usando Corner-Cutting
//
**********************************************************************************/

#include "Curves.h"

#include <fstream>
// ------------------------------------------------------------------------------

void Curves::Init()
{
    bezierMode = true;
    // ------------------
    // Aloca Geometria
    // ------------------

    // cria vertex buffer
    vbuffer = new VertexBuffer<Vertex>(nullptr, MaxSize);
    cp_vbuffer = new VertexBuffer<Vertex>(nullptr, MaxControlPoints);
    curve_vbuffer = new VertexBuffer<Vertex>(nullptr, MaxSize);
    guide_vbuffer = new VertexBuffer<Vertex>(nullptr, 4);
    rect_vbuffer = new VertexBuffer<Vertex>(nullptr, 20);

    // ------------------
    // Ajusta Pipeline
    // ------------------

    BuildRootSignature();
    BuildPipelineState();

    // ------------------

    graphics->SendToGpu();
}

// ------------------------------------------------------------------------------

void Curves::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    if (input->KeyPress(VK_DELETE))
    {
        controlPointCount = 0;
        savedCurveCount = 0;
        curveCount = 0;
        guideLineCount = 0;

        if (bezierMode)
            bezierState = BezierState::Esperando_p1;
    }

    // --- NOVO: Alterna o modo Bézier com a tecla 'B' ---
    if (input->KeyPress('B'))
    {
        bezierMode = !bezierMode;
        // Reseta o estado ao entrar no modo Bézier
        if (bezierMode)
        {
            bezierState = Esperando_p1;
            controlPointCount = 0;
            curveCount = 0;
        }
    }

    if (input->KeyPress('S')) Save();

    if (input->KeyPress('L')) Load();

    if (!bezierMode) {
        // cria vértices com o botão do mouse
        if (input->KeyPress(VK_LBUTTON))
        {
            float cx = float(window->CenterX());
            float cy = float(window->CenterY());
            float mx = float(input->MouseX());
            float my = float(input->MouseY());

            // converte as coordenadas da tela para a faixa -1.0 a 1.0
            // cy e my foram invertidos para levar em consideração que 
            // o eixo y da tela cresce na direção oposta do cartesiano
            float x = (mx - cx) / cx;
            float y = (cy - my) / cy;

            vertices[index] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::White) };
            index = (index + 1) % MaxSize;

            if (count < MaxSize)
                ++count;

            // copia vértices para o buffer da GPU usando o buffer de Upload
            graphics->PrepareGpu(pipelineState);
            vbuffer->Copy(vertices, count);
            graphics->SendToGpu();

            // desenha curva
            Display();
        }
        return;
    }

    // --- MODO BÉZIER ---
        // Converte coordenadas do mouse para o espaço da tela (-1 a 1)
    float cx = float(window->CenterX());
    float cy = float(window->CenterY());
    float mx = float(input->MouseX());
    float my = float(input->MouseY());
    float x = (mx - cx) / cx;
    float y = (cy - my) / cy;


    // --- FUNCIONAMENTO ---
    // O programa para Bézier envolve 4 estados
    switch (bezierState)
    {
    // (1) estado Esperando_p1
    // O estado inicial do programa;
    // Só é executado antes da primeira curva;
    // Fica aguardando o primeiro clique;
    // Após o primeiro clique, cria os 2 primeiros pontos na coordenada do mouse;
    // Passa para o próximo estado.
    case Esperando_p1:
        if (input->KeyPress(VK_LBUTTON))
        {
            // Define o primeiro ponto de controle
            P[0] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Red) };
            P[1] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Red) };
            controlPointCount = 2;
            bezierState = Ajustando_p2;
        }
        break;

    // (2) estado Ajustando_p2
    // O segundo estado do programa;
    // Inicializa verificando a quantidade de pontos de controle existentes;
    case Ajustando_p2:

        // caso qtd == 2:
        // A primeira sequência de execução do algoritmo;
        // Ajusta a posição do segundo ponto de controle;
        // Cria as linhas de guia da posição do mouse;
        // Aguarda o ajuste do usuário esperando o clique;
        // Passa pro próximo estado;
        if (controlPointCount == 2) {
            P[1].Pos.x = 2.0f * P[0].Pos.x - x;
            P[1].Pos.y = 2.0f * P[0].Pos.y - y;

            guideLine[0] = P[0];
            guideLine[0].Color = { XMFLOAT4(Colors::LightCyan) };
            guideLine[1] = { {x,y,0.0f}, XMFLOAT4(Colors::Transparent) };
            guideLineCount = 2;

            if (input->KeyPress(VK_LBUTTON))
            {
                bezierState = Esperando_p3;
            }
        }

        // caso qtd == 4:
        // O processo de execução após a primeira curva ser feita;
        // Passa o ponto 1 para o ponto 4;
        // Ajusta a posição do ponto 2 (reflete a posição para fazer o ajuste correto);
        // Ajusta as linhas de guia da posição do mouse;
        // Passa para o próximo estado;
        else if (controlPointCount == 4) {
            P[0] = P[3];
            P[1].Pos.x = 2.0f * P[3].Pos.x - P[2].Pos.x;
            P[1].Pos.y = 2.0f * P[3].Pos.y - P[2].Pos.y;
            guideLine[0] = guideLine[3];
            guideLine[1] = guideLine[2];

            bezierState = Esperando_p3;
        }

        
        break;

    // (3) estado Esperando_p3
    // Cria OU redefine os pontos 3 e 4;
    // Permite o ajuste dos pontos de controle;
    // Continuamente calcula a curva, permitindo a pré-visualização da curva;
    // Aguarda o ajuste do usuário através do clique;
    // Passa para o próximo estado;
    case Esperando_p3:

        P[2] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Red) };
        P[3] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Red) };
        controlPointCount = 4;

        P[2].Pos.x = x;
        P[2].Pos.y = y;
        P[3].Pos.x = x;
        P[3].Pos.y = y;

        CalculateBezierCurve();

        if (input->KeyPress(VK_LBUTTON))
        {
            curveCount = 0; // Reseta a contagem de curvas
            P[3] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::Red) };
            bezierState = Ajustando_p4;
        }
        break;


    // (4) estado Ajustando_p4
    // Estado de ajuste do último ponto;
    // Fixa o ponto 4 e ajusta a posição do ponto 3;
    // Cria e ajusta as 2 linhas guias dos 2 últimos pontos de controle;
    // Calcula a curva novamente, agora com o ponto 3 fixado;
    // No próximo clique, repete o ciclo zerando o valor de pontos da curva e voltando pro estado 2;
    // Caso chegue no limite de curvas (Segmentos * 20), utiliza uma lógica para sobrescrever as primeiras curvas;
    case Ajustando_p4:

        P[2].Pos.x = 2.0f * P[3].Pos.x - x; // Ajusta o ponto de controle 3 para manter simetria
        P[2].Pos.y = 2.0f * P[3].Pos.y - y;

        guideLine[2] = P[2];
        guideLine[2].Color = { XMFLOAT4(Colors::LightCyan) };
        guideLine[3] = { {x,y,0.0f}, XMFLOAT4(Colors::Transparent) };
        guideLineCount = 4;

        CalculateBezierCurve();
        if (input->KeyPress(VK_LBUTTON))
        {
            savedCurveCount += curveCount;
            if (savedCurveCount >= MaxSize - (LineSegs + 1)) {
                savedCurveCount = 0;
                controlPointCount = 0;
                guideLineCount = 0;
                curveCount = 0;

                bezierState = Esperando_p1;
                break;
            }

            curveCount = 0; // Reseta a contagem de curvas
            bezierState = Ajustando_p2;
        }
        break;
    }

    //Retângulos sob os pontos de controle
    UpdateControlRect();

    graphics->PrepareGpu(pipelineState);

    //Garantia dos buffers
    uint totalCurveCount = savedCurveCount + curveCount;
    if (controlPointCount > 0)
    {
        cp_vbuffer->Copy(P, controlPointCount);

        guide_vbuffer->Copy(guideLine, guideLineCount);
        rect_vbuffer->Copy(rectVertices, controlPointCount);
    }
    if (totalCurveCount > 0)
    {
        curve_vbuffer->Copy(vertices, totalCurveCount);
    }

    graphics->SendToGpu();
    Display();
}

// ------------------------------------------------------------------------------

void Curves::Display()
{
    graphics->Clear(pipelineState);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);

    if (bezierMode)
    {
        // 1. Desenha o Polígono de Controle
        if (controlPointCount > 0)
        {
            graphics->CommandList()->IASetVertexBuffers(0, 1, rect_vbuffer->View());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

            for (uint i = 0; i < controlPointCount; ++i)
            {
                // Desenha 4 vértices, começando do offset i*4
                graphics->CommandList()->DrawInstanced(5, 1, i * 5, 0);
            }
        }

        if (controlPointCount > 1)
        {
            // Define o buffer para as linhas
            graphics->CommandList()->IASetVertexBuffers(0, 1, cp_vbuffer->View());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
            graphics->CommandList()->DrawInstanced(controlPointCount, 1, 0, 0);
        }

        uint totalCurveCount = savedCurveCount + curveCount;
        // 2. Desenha a Curva de Bézier
        if (totalCurveCount > 1)
        {
            graphics->CommandList()->IASetVertexBuffers(0, 1, curve_vbuffer->View());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
            graphics->CommandList()->DrawInstanced(totalCurveCount, 1, 0, 0);
        }
        if (guideLineCount > 0)
        {
            graphics->CommandList()->IASetVertexBuffers(0, 1, guide_vbuffer->View());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
            graphics->CommandList()->DrawInstanced(guideLineCount, 1, 0, 0);
        }
    }
    else
    {
        // Desenha usando o buffer do modo original
        graphics->CommandList()->IASetVertexBuffers(0, 1, vbuffer->View());
        graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
        graphics->CommandList()->DrawInstanced(count, 1, 0, 0);
    }

    graphics->Present();
}
// ------------------------------------------------------------------------------

void Curves::Finalize()
{
    // espera GPU finalizar comandos pendentes
    graphics->WaitForGpu();

    // libera memória alocada
    rootSignature->Release();
    pipelineState->Release();

    delete vbuffer;
    delete cp_vbuffer;
    delete curve_vbuffer;
    delete guide_vbuffer;
    delete rect_vbuffer;
}

// Fórmula da Curva de Bézier Cúbica
void Curves::CalculateBezierCurve()
{
    float t, x, y;

    for (int i = 0; i <= LineSegs; i++)
    {
        t = 1.0f / LineSegs * i;

        float one_minus_t = 1.0f - t;
        float one_minus_t_sq = one_minus_t * one_minus_t;
        float t_sq = t * t;

        x = (one_minus_t_sq * one_minus_t) * P[0].Pos.x +
            (3.0f * t * one_minus_t_sq) * P[1].Pos.x +
            (3.0f * t_sq * one_minus_t) * P[2].Pos.x +
            (t_sq * t) * P[3].Pos.x;

        y = (one_minus_t_sq * one_minus_t) * P[0].Pos.y +
            (3.0f * t * one_minus_t_sq) * P[1].Pos.y +
            (3.0f * t_sq * one_minus_t) * P[2].Pos.y +
            (t_sq * t) * P[3].Pos.y;

        vertices[savedCurveCount + i] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::DarkCyan) };
    }

    curveCount = LineSegs + 1;
}

void Curves::UpdateControlRect()
{
    if (controlPointCount == 0) return;

    float pointSize = 0.02f;

    //Ajuste de acordo com a resolução para manter o quadrado
    float w = float(window->Width());
    float h = float(window->Height());
    float ratio = w / h;
    float sizeY = pointSize;
    float sizeX = pointSize / ratio;

    for (uint i = 0; i < controlPointCount; ++i)
    {
        // Pega a posição central do ponto de controle
        XMFLOAT3 centerPos = P[i].Pos;

        // Calcula os 4 cantos do quadrado ao redor do centro
        XMFLOAT3 topLeft = { centerPos.x - sizeX, centerPos.y + sizeY, 0.0f };
        XMFLOAT3 topRight = { centerPos.x + sizeX, centerPos.y + sizeY, 0.0f };
        XMFLOAT3 bottomRight = { centerPos.x + sizeX, centerPos.y - sizeY, 0.0f };
        XMFLOAT3 bottomLeft = { centerPos.x - sizeX, centerPos.y - sizeY, 0.0f };

        // Pega a cor do ponto de controle
        XMFLOAT4 color = P[i].Color;

        // Preenche os 4 vértices para este quadrado no nosso grande array
        // O offset é i * 4
        rectVertices[i * 5 + 0] = { topLeft,     color };
        rectVertices[i * 5 + 1] = { topRight,    color };
        rectVertices[i * 5 + 2] = { bottomRight, color };
        rectVertices[i * 5 + 3] = { bottomLeft,  color };
        rectVertices[i * 5 + 4] = { topLeft, color };
    }
}

void Curves::Save()
{
    std::ofstream file("save.bin", std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    file.write(reinterpret_cast<const char*>(&savedCurveCount), sizeof(savedCurveCount));
    file.write(reinterpret_cast<const char*>(&controlPointCount), sizeof(controlPointCount));
    file.write(reinterpret_cast<const char*>(&guideLineCount), sizeof(guideLineCount));
    file.write(reinterpret_cast<const char*>(&bezierState), sizeof(bezierState));

    file.write(reinterpret_cast<const char*>(vertices), sizeof(Vertex) * savedCurveCount);
    file.write(reinterpret_cast<const char*>(P), sizeof(Vertex) * controlPointCount);
    file.write(reinterpret_cast<const char*>(guideLine), sizeof(Vertex) * guideLineCount);


    file.close();


}

void Curves::Load()
{
    std::ifstream file("save.bin", std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    file.read(reinterpret_cast<char*>(&savedCurveCount), sizeof(savedCurveCount));
    file.read(reinterpret_cast<char*>(&controlPointCount), sizeof(controlPointCount));
    file.read(reinterpret_cast<char*>(&guideLineCount), sizeof(guideLineCount));
    file.read(reinterpret_cast<char*>(&bezierState), sizeof(bezierState));

    file.read(reinterpret_cast<char*>(vertices), sizeof(Vertex) * savedCurveCount);
    file.read(reinterpret_cast<char*>(P), sizeof(Vertex) * controlPointCount);
    file.read(reinterpret_cast<char*>(guideLine), sizeof(Vertex) * guideLineCount);

    file.close();

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
        engine->window->Mode(ASPECTRATIO);
        engine->window->Size(1024, 600);
        engine->window->Color(25, 25, 25);
        engine->window->Title("Curves");
        engine->window->Icon("Icon");
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);
        engine->graphics->VSync(true);


        // cria e executa a aplicação
        engine->Start(new Curves());

        // finaliza execução
        delete engine;
    }
    catch (Error& e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "Curves", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------