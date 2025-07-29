/**********************************************************************************
// Texture (Arquivo de Cabeçalho)
//
// Criação:     04 Oct 2023
// Atualização: 20 Jul 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Representa uma textura de uma malha 3D
//
**********************************************************************************/

#ifndef DXUT_TEXTURE_H_
#define DXUT_TEXTURE_H_

// -------------------------------------------------------------------------------

class Texture
{
private:
	ID3D12Resource* upload = nullptr;				// buffer de Upload CPU -> GPU
	ID3D12Resource* texture = nullptr;				// buffer na GPU
	ID3D12DescriptorHeap* heap = nullptr;			// head de descritores

	void LoadTexture(wchar_t * file);				// carrega textura

public:
	Texture(const char* file);						// construtor para arquivo único
	~Texture();										// destrutor

	ID3D12DescriptorHeap* const* Heap();			// retorna heap de descritores
	D3D12_GPU_DESCRIPTOR_HANDLE Table();			// retorna tabela de descritores
};

// -------------------------------------------------------------------------------

inline ID3D12DescriptorHeap* const* Texture::Heap()
{ return &heap; }

inline D3D12_GPU_DESCRIPTOR_HANDLE Texture::Table()
{ return heap->GetGPUDescriptorHandleForHeapStart(); }

// -------------------------------------------------------------------------------

#endif