#pragma once
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <fstream>

// Shared compile policy. ComPtr releases diagnostics/bytecode on every exit path.
inline bool CompileShader(const wchar_t* filename, const char* entryPoint,
                          const char* profile, HWND owner,
                          Microsoft::WRL::ComPtr<ID3DBlob>& bytecode)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    const HRESULT result = D3DCompileFromFile(
        filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, profile,
        flags, 0, bytecode.ReleaseAndGetAddressOf(), errors.GetAddressOf());
    if (SUCCEEDED(result))
        return true;

    std::ofstream log("shader-error.txt", std::ios::binary);
    if (errors)
    {
        log.write(static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
        OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
    }
    else
        log << "Could not read shader file. HRESULT: " << std::hex << result;
    if (owner)
        MessageBoxW(owner, L"Shader compilation failed. See shader-error.txt.",
                    filename, MB_OK | MB_ICONERROR);
    return false;
}
