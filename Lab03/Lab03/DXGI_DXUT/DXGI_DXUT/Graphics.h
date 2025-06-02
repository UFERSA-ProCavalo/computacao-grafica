/**********************************************************************************
// Graphics (Arquivo de Cabe�alho)
// 
// Cria��o:     06 Abr 2011
// Atualiza��o:	11 Jul 2023
// Compilador:	Visual C++ 2022
//
// Descri��o:   Usa fun��es do Direct3D 12 para acessar a GPU
//
**********************************************************************************/

#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

// --------------------------------------------------------------------------------
// Inclus�es

#include <dxgi1_6.h>    // infraestrutura gr�fica do DirectX
#include <d3d12.h>      // principais fun��es do Direct3D
#include "Window.h"     // cria e configura uma janela do Windows
#include "Types.h"      // tipos espec�ficos da engine

// --------------------------------------------------------------------------------

class Graphics
{
private:
    // pipeline
    ID3D12Device8              * device;                    // dispositivo gr�fico
    IDXGIFactory7              * factory;                   // interface principal da DXGI
    void LogHardwareInfo();									// mostra informa��es do hardware

public:
    Graphics();												// constructor
    ~Graphics();											// destructor
    void Initialize(Window * window);						// inicializa o Direct3D
};

// --------------------------------------------------------------------------------

#endif