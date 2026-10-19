#ifndef _common_H_
#define _common_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

struct VertexType
	{
		/*D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;*/
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
	};

#endif