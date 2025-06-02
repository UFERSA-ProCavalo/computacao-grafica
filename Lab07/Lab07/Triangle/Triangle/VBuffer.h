/**********************************************************************************
// VBuffer (Arquivo de Cabe�alho)
//
// Cria��o:     07 Mar 2025
// Atualiza��o: 16 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descri��o:   Representa um recurso do tipo Vertex Buffer
//
**********************************************************************************/

#ifndef DXUT_VBUFFER_H_
#define DXUT_VBUFFER_H_

// -------------------------------------------------------------------------------

#include "Types.h"
#include "Engine.h"
#include "Graphics.h"

// -------------------------------------------------------------------------------

template<typename T> class VertexBuffer
{
private:
    ID3D12Resource * upload;            // buffer de Upload CPU -> GPU
    ID3D12Resource * vbuffer;           // buffer na GPU
    D3D12_VERTEX_BUFFER_VIEW view;      // descritor do buffer
    uint size;                          // tamanho do buffer
    uint stride;                        // tamanho de um v�rtice

public:

    // construtor que recebe um buffer
    VertexBuffer<T>(const void * vb, uint vertexCount)
    {
        // guarda tamanho do v�rtice
        stride = sizeof(T);

        // guarda tamanho do buffer
        size = vertexCount * stride;

        // aloca recursos para o vertex buffer
        Engine::graphics->Allocate(UPLOAD, size, &upload);
        Engine::graphics->Allocate(GPU, size, &vbuffer);

        // copia v�rtices para o buffer da GPU usando o buffer de Upload
        Engine::graphics->Copy(vb, size, upload, vbuffer);
    }

    // construtor de c�pia
    VertexBuffer(const VertexBuffer & rhs) = delete;

    // operador de atribui��o
    VertexBuffer & operator=(const VertexBuffer & rhs) = delete;

    // destrutor
    ~VertexBuffer()
    {
        if (upload) upload->Release();
        if (vbuffer) vbuffer->Release();
    }

    D3D12_VERTEX_BUFFER_VIEW * View()
    {
        view.BufferLocation = vbuffer->GetGPUVirtualAddress();
        view.StrideInBytes = stride;
        view.SizeInBytes = size;

        return &view;
    }
};

// -------------------------------------------------------------------------------

#endif



