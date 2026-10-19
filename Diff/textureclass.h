#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include "DDSTextureLoader.h"


class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&) = delete;
    TextureClass& operator=(const TextureClass&) = delete;
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	bool InitializeCubedds(ID3D11Device*, WCHAR*);

	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};

#endif