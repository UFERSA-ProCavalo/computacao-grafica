/**********************************************************************************
// Graphics (Arquivo de Cabeçalho)
// 
// Criação:     06 Abr 2011
// Atualização:	04 Mai 2025
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
    ID3D12Device8              * device;                        // dispositivo gráfico
    IDXGIFactory7              * factory;                       // interface principal da DXGI
    
    uint                         bufferIndex;                   // índice do alocador                      
    static const uint            bufferCount = 2;               // quantidade de alocadores 

    ID3D12CommandQueue         * commandQueue;                  // fila de comandos da GPU
    ID3D12GraphicsCommandList  * commandList;                   // lista de comandos 
    ID3D12CommandAllocator     * commandAlloc[bufferCount];     // alocadores de comandos

    // sincronização 
    ID3D12Fence                * fence;						    // cerca para sincronizar CPU/GPU
    HANDLE                       fenceEvent;                    // sinalizador de eventos
    ullong                       fenceValue[bufferCount];	    // número atual das cercas

    void LogHardwareInfo();									    // mostra informações do hardware

public:
    Graphics();												    // constructor
    ~Graphics();											    // destructor

    void Initialize(Window * window);						    // inicializa o Direct3D

    void WaitForGpu();								            // espera a execução da fila de comandos
    void SyncToGpu();								            // espera a liberação de um alocador de comandos
    void SendToGpu();									        // submete comandos pendentes para execução
};

// --------------------------------------------------------------------------------

#endif