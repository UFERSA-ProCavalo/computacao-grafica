/**********************************************************************************
// Texture (Código Fonte)
//
// Criação:     04 Oct 2023
// Atualização: 20 Jul 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Representa uma textura de uma malha 3D
//
**********************************************************************************/

#include "Engine.h"
#include "Error.h"
#include "Texture.h"
#include "WIC.h"
#include <vector>
#include <memory>
using std::unique_ptr;
using std::vector;

// -------------------------------------------------------------------------------

Texture::Texture(const char* file)
{
    // heap de descritores para a textura
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(Engine::graphics->Device()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&heap)));

    // converte de char para wchar_t
    wchar_t filename[4096];
    MultiByteToWideChar(CP_ACP, 0, file, -1, filename, 4096);
        
    // carrega textura no
    LoadTexture(filename);
}

// -------------------------------------------------------------------------------

Texture::~Texture()
{
    if (heap)
        heap->Release();

	if (upload)
		upload->Release();

	if (texture)
		texture->Release();
}

// -------------------------------------------------------------------------------

void Texture::LoadTexture(wchar_t* filename)
{
    // guarda dados obtidos da textura
    unique_ptr<uint8_t[]> decodedData;
    D3D12_SUBRESOURCE_DATA subresource = {};

	// carrega textura em um recurso
	ThrowIfFailed(DirectX::LoadTextureFromFile(
		Engine::graphics->Device(), // dispositivo Direct3D
		filename,                   // caminho para o arquivo
		&texture,                   // saída: texture resource
		decodedData,                // saída: dados decodificados
		subresource                 // saída: dados do subrecurso
	));

    // -------------------------
    // Upload na Memória da GPU 
    // -------------------------

    // informações da textura
    D3D12_RESOURCE_DESC texDesc = texture->GetDesc();

    // número de mipmaps na textura
    const uint numSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;

    // recupera tamanho da textura
    ullong textureSize = 0;
    Engine::graphics->Device()->GetCopyableFootprints(
        &texDesc, 0, numSubresources, 0, nullptr, nullptr, nullptr, &textureSize
    );

    // aloca memória de upload
    Engine::graphics->Allocate(UPLOAD, textureSize, &upload);

    // copia textura para a memória de Upload e depois para a GPU
    Engine::graphics->Copy(&subresource, textureSize, upload, texture);

    // ---------------------
    // Shader Resource View
    // ---------------------

    // configura descritor para a textura
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    // endereço do primeiro descritor na heap
    D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();

    // cria descritor para a textura
    Engine::graphics->Device()->CreateShaderResourceView(texture, &srvDesc, handle);
}

// -------------------------------------------------------------------------------
