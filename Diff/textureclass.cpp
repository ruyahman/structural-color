#include "textureclass.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstring>

namespace
{
    HRESULT LoadDds(ID3D11Device* device, const WCHAR* filename,
                    ID3D11ShaderResourceView** texture)
    {
        HRESULT result = DirectX::CreateDDSTextureFromFile(device, filename, nullptr, texture);
        if (result != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
            return result;

        // Legacy X8B8G8R8 DDS (font.dds) has no DXGI equivalent.
        // Expand the unused byte to opaque alpha in memory; keep the asset intact.
        std::ifstream file(filename, std::ios::binary);
        if (!file)
            return result;
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
        if (data.size() < 128 || std::memcmp(data.data(), "DDS ", 4) != 0)
            return result;
        uint32_t header[31];
        std::memcpy(header, data.data() + 4, sizeof(header));
        if (header[0] != 124 || header[18] != 32 || header[19] != 0x40 ||
            header[21] != 32 || header[22] != 0xff || header[23] != 0xff00 ||
            header[24] != 0xff0000 || header[25] != 0 ||
            (data.size() - 128) % 4 != 0)
            return result;
        header[19] |= 1; // DDPF_ALPHAPIXELS
        header[25] = 0xff000000;
        std::memcpy(data.data() + 4, header, sizeof(header));
        for (size_t i = 131; i < data.size(); i += 4)
            data[i] = 255;
        return DirectX::CreateDDSTextureFromMemory(device, data.data(), data.size(), nullptr, texture);
    }
}


TextureClass::TextureClass()
{
	m_texture = 0;
}





TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	result = LoadDds(device, filename, &m_texture);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool TextureClass::InitializeCubedds(ID3D11Device* device, WCHAR* filename)
{
    // The DDS header identifies the cubemap and its mip levels.
    return SUCCEEDED(DirectX::CreateDDSTextureFromFile(device, filename, nullptr, &m_texture));
}

void TextureClass::Shutdown()
{
	
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}

