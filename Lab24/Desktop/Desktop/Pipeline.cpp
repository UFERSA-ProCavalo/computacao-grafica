/**********************************************************************************
// Pipeline (Código Fonte)
//
// Criação:     02 Out 2023
// Atualização: 03 Jul 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Configura o pipeline da aplicação 3D
//
**********************************************************************************/

#include "Desktop.h"

// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Desktop::BuildRootSignature()
{
    // configura tabela para descritores tipo SRV
    D3D12_DESCRIPTOR_RANGE srvTable = {};
    srvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    srvTable.NumDescriptors = 1;
    srvTable.BaseShaderRegister = 0;
    srvTable.RegisterSpace = 0;
    srvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // define parâmetro raiz com dois slots
    D3D12_ROOT_PARAMETER rootParameters[4];

    // slot 0: Textura
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &srvTable;

    // slot 1: Scene
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[1].Descriptor.RegisterSpace = 0;
    rootParameters[1].Descriptor.ShaderRegister = 0;

    // slot 2: Constants
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[2].Descriptor.RegisterSpace = 0;
    rootParameters[2].Descriptor.ShaderRegister = 1;

    // slot 3: Material
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[3].Descriptor.RegisterSpace = 0;
    rootParameters[3].Descriptor.ShaderRegister = 2;

    // amostrador de texturas tipo Point
    D3D12_STATIC_SAMPLER_DESC point = {};
    point.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    point.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    point.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    point.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    point.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // amostrador de texturas tipo Anisotropic
    D3D12_STATIC_SAMPLER_DESC anisotropic = {};
    anisotropic.Filter = D3D12_FILTER_ANISOTROPIC;
    anisotropic.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    anisotropic.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    anisotropic.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    anisotropic.MaxAnisotropy = 16;
    anisotropic.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    anisotropic.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    anisotropic.MinLOD = 0;
    anisotropic.MaxLOD = D3D12_FLOAT32_MAX;
    anisotropic.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // configura parâmetros da assinatura raiz
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 4;
    rootSigDesc.pParameters = rootParameters;
    rootSigDesc.NumStaticSamplers = 1;
    rootSigDesc.pStaticSamplers = &anisotropic;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // serializa assinatura raiz
    ID3DBlob * serializedRootSig = nullptr;
    ID3DBlob * error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    if (error != nullptr)
    {
        OutputDebugString((char *)error->GetBufferPointer());
    }

    // cria a assinatura raiz para o dispositivo
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

// ------------------------------------------------------------------------------

void Desktop::BuildPipelineState()
{
    // --------------------
    // --- Input Layout ---
    // --------------------

    D3D12_INPUT_ELEMENT_DESC inputLayout[4] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // --------------------
    // ----- Shaders ------
    // --------------------

    ID3DBlob * vertexShader;
    ID3DBlob * pixelShader;

    D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

    // --------------------
    // ---- Rasterizer ----
    // --------------------

    D3D12_RASTERIZER_DESC rasterizer = {};
    //rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ---------------------
    // --- Color Blender ---
    // ---------------------

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        TRUE,FALSE,
        D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    // ---------------------
    // --- Depth Stencil ---
    // ---------------------

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;

    // -----------------------------------
    // --- Pipeline State Object (PSO) ---
    // -----------------------------------

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE *>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE *>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 4 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineWire));

    // -----------------------------------
    // --- Pipeline State Object (PSO) ---
    // -----------------------------------

    rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState = rasterizer;
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineSolid));
    pipelineState = pipelineSolid;

    vertexShader->Release();
    pixelShader->Release();
}

// ----------------------------------------------------------------------------