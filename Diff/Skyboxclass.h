#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <math.h>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "textureclass.h"

class SkyboxClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:
	SkyboxClass();
	SkyboxClass(const SkyboxClass&) = delete;
    SkyboxClass& operator=(const SkyboxClass&) = delete;
	~SkyboxClass();
	
	bool Initialize(ID3D11Device*, int, int, WCHAR*, HWND);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool CreateSphere(int,int,ID3D11Device* device,HWND);
	void RenderBuffers(ID3D11DeviceContext*);
	void ShutdownBuffers();

	bool LoadCubeTexture(ID3D11Device*, WCHAR*, HWND);
	void ReleaseTexture();

private:
	TextureClass* m_skyboxTexture;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int NumSphereVertices;
	int NumSphereFaces;


};

#endif