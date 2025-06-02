/**********************************************************************************
// Graphics (Arquivo de Cabeçalho)
// 
// Criação:     06 Abr 2011
// Atualização:	11 Jul 2023
// Compilador:	Visual C++ 2022
//
// Descrição:   Usa funções do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

// --------------------------------------------------------------------------------
// Inclusões

#include <dxgi1_6.h>    // infraestrutura gráfica do DirectX
#include <d3d12.h>      // principais funções do Direct3D
#include "Window.h"     // cria e configura uma janela do Windows
#include "Types.h"      // tipos específicos da engine

// --------------------------------------------------------------------------------

class Graphics
{
private:
    // pipeline
    ID3D12Device8              * device;                    // dispositivo gráfico
    IDXGIFactory7              * factory;                   // interface principal da DXGI
    void LogHardwareInfo();									// mostra informações do hardware

public:
    Graphics();												// constructor
    ~Graphics();											// destructor
    void Initialize(Window * window);						// inicializa o Direct3D
};

// --------------------------------------------------------------------------------

#endif