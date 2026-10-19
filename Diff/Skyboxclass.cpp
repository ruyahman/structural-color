#include "SkyboxClass.h"


SkyboxClass::SkyboxClass()
{
    m_skyboxTexture = nullptr;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	NumSphereVertices = 0;
	NumSphereFaces = 0;
}




SkyboxClass::~SkyboxClass()
{
}

bool SkyboxClass::Initialize(ID3D11Device* device, int latLines, int longLines, WCHAR* textureFilename, HWND hwnd)
{
	bool result;

	result = CreateSphere(latLines, longLines, device, hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the 28.", L"Error", MB_OK);
		return false;
	}
	
	result = LoadCubeTexture(device, textureFilename, hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}

void SkyboxClass::Shutdown()
{

	ReleaseTexture();
	ShutdownBuffers();

	return;
}

void SkyboxClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int SkyboxClass::GetIndexCount()
{
	return NumSphereFaces * 3;
}


ID3D11ShaderResourceView* SkyboxClass::GetTexture()
{
	return m_skyboxTexture->GetTexture();
}


bool SkyboxClass::CreateSphere(int LatLines, int LongLines, ID3D11Device* device, HWND hwnd )
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	VertexType* vertices;
	UINT* indices;
	XMMATRIX mRotX,mRotY,mWor;//回転行列用
	//球頂点、面の数
	NumSphereVertices = ((LatLines-2) * LongLines) + 2;
	NumSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

	vertices = new VertexType[NumSphereVertices]{};
	if(!vertices)
	{
		MessageBox(hwnd, L"Could not initialize the 85.", L"Error", MB_OK);
		return false;
	}

	indices = new UINT[NumSphereFaces * 3];
	if(!indices)
	{
		MessageBox(hwnd, L"Could not initialize the 92.", L"Error", MB_OK);
		return false;
	}



	////////////////////////vertex///////////////////////////////////////////////
	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	XMFLOAT3 curVertPos = XMFLOAT3(0.0f,0.0f,1.0f);
	XMFLOAT3 norma = XMFLOAT3(0.0f, 0.0f, 1.0f);

	vertices[0].position = XMFLOAT3(0.0f,0.0f,1.0f);

	for(int i = 0; i < LatLines-2; ++i)
	{
		spherePitch = (i+1) * (XM_PI/(LatLines-1));
		mRotX = XMMatrixRotationX(spherePitch);
		for(int j = 0; j < LongLines; ++j)
		{
		sphereYaw = j * (XM_2PI/(LongLines));
		mRotY = XMMatrixRotationZ(sphereYaw);
		mWor = mRotX * mRotY;
		XMStoreFloat3(&curVertPos, XMVector3TransformNormal(XMLoadFloat3(&norma), mWor));//ここがおかしいのか？
		XMStoreFloat3(&curVertPos, XMVector3Normalize(XMLoadFloat3(&curVertPos)));
		vertices[i*LongLines+j+1].position.x = curVertPos.x;
		vertices[i*LongLines+j+1].position.y = curVertPos.y;
		vertices[i*LongLines+j+1].position.z = curVertPos.z;

		}
	}

	//normal = vertices[2].position;
	vertices[NumSphereVertices-1].position.x =  0.0f;
	vertices[NumSphereVertices-1].position.y =  0.0f;
	vertices[NumSphereVertices-1].position.z = -1.0f;

	////////////////////////////index///////////////////////////////////////////////
	// The polar caps must use the same inward winding as the latitude rings.
	int k = 0;
	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = 0;
		indices[k+1] = l+2;
		indices[k+2] = l+1;
		k += 3;
	}

	indices[k] = 0;
	indices[k+1] = 1;
	indices[k+2] = LongLines;
	k += 3;

	for(int i = 0; i < LatLines-3; ++i)
	{
		for(int j = 0; j < LongLines-1; ++j)
		{
			indices[k]   = i*LongLines+j+1;
			indices[k+1] = i*LongLines+j+2;
			indices[k+2] = (i+1)*LongLines+j+1;

			indices[k+3] = (i+1)*LongLines+j+1;
			indices[k+4] = i*LongLines+j+2;
			indices[k+5] = (i+1)*LongLines+j+2;

			k += 6; // next quad
		}

		indices[k]   = (i*LongLines)+LongLines;
		indices[k+1] = (i*LongLines)+1;
		indices[k+2] = ((i+1)*LongLines)+LongLines;

		indices[k+3] = ((i+1)*LongLines)+LongLines;
		indices[k+4] = (i*LongLines)+1;
		indices[k+5] = ((i+1)*LongLines)+1;

		k += 6;
	}

	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = NumSphereVertices-1;
		indices[k+1] = (NumSphereVertices-1)-(l+2);
		indices[k+2] = (NumSphereVertices-1)-(l+1);
		k += 3;
	}

	indices[k] = NumSphereVertices-1;
	indices[k+1] = NumSphereVertices-2;
	indices[k+2] = (NumSphereVertices-1)-LongLines;
	
	/////////vertexbufferdesc and create////////////////////////////////////////////////////////
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( VertexType ) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	ZeroMemory( &vertexData, sizeof(vertexData) );
	vertexData.pSysMem = vertices;

	 result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the 214.", L"Error", MB_OK);
		return false;
	}

	//////////indexbufferdesc and create/////////////////////////
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;


	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the 235.", L"Error", MB_OK);
		return false;
	}



	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}




void SkyboxClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType); 
	offset = 0;
 
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}



void SkyboxClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

bool SkyboxClass::LoadCubeTexture(ID3D11Device* device, WCHAR* filename, HWND hwnd)
{
	bool result;

	m_skyboxTexture = new TextureClass;
	if(!m_skyboxTexture)
	{
		MessageBox(hwnd, L"Could not initialize the 294.", L"Error", MB_OK);
		return false;
	}
	result = m_skyboxTexture->InitializeCubedds(device, filename);
	if(!result)
	{	
		MessageBox(hwnd, L"Could not initialize the 300.", L"Error", MB_OK);
		return false;
	}	

	return true;
}

void SkyboxClass::ReleaseTexture()
{

	if(m_skyboxTexture)
	{
		m_skyboxTexture->Shutdown();
		delete m_skyboxTexture;
		m_skyboxTexture = 0;
	}

	return;
}




