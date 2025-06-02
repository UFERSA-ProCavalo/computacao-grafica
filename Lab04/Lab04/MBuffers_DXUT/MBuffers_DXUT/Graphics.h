/**********************************************************************************
// Graphics (Arquivo de Cabe�alho)
// 
// Cria��o:     06 Abr 2011
// Atualiza��o:	04 Mai 2025
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
    ID3D12Device8              * device;                        // dispositivo gr�fico
    IDXGIFactory7              * factory;                       // interface principal da DXGI
    
    uint                         bufferIndex;                   // �ndice do alocador                      
    static const uint            bufferCount = 2;               // quantidade de alocadores 

    ID3D12CommandQueue         * commandQueue;                  // fila de comandos da GPU
    ID3D12GraphicsCommandList  * commandList;                   // lista de comandos 
    ID3D12CommandAllocator     * commandAlloc[bufferCount];     // alocadores de comandos

    // sincroniza��o 
    ID3D12Fence                * fence;						    // cerca para sincronizar CPU/GPU
    HANDLE                       fenceEvent;                    // sinalizador de eventos
    ullong                       fenceValue[bufferCount];	    // n�mero atual das cercas

    void LogHardwareInfo();									    // mostra informa��es do hardware

public:
    Graphics();												    // constructor
    ~Graphics();											    // destructor

    void Initialize(Window * window);						    // inicializa o Direct3D

    void WaitForGpu();								            // espera a execu��o da fila de comandos
    void SyncToGpu();								            // espera a libera��o de um alocador de comandos
    void SendToGpu();									        // submete comandos pendentes para execu��o
};

// --------------------------------------------------------------------------------

#endif