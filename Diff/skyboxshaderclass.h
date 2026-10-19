#ifndef _SKYBOXSkyboxShaderClass_H_
#define _SKYBOXSkyboxShaderClass_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <d3dcompiler.h>
#include <fstream>
using namespace std;


class SkyboxShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float lightPadding;
		XMFLOAT3 cameraPosi;
		float cameraPadding;
	};

	struct PixelBufferType
	{
		XMFLOAT4 pixelColor;
	};

public:
	SkyboxShaderClass();
	SkyboxShaderClass(const SkyboxShaderClass&) = delete;
    SkyboxShaderClass& operator=(const SkyboxShaderClass&) = delete;
	~SkyboxShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT3, XMFLOAT4, ID3D11RasterizerState*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int, ID3D11RasterizerState*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_pixelBuffer;
};

#endif