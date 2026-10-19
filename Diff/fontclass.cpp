#include <memory>
#include <sstream>
#include <string>
#include "fontclass.h"


FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}





FontClass::~FontClass()
{
}


bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;
	
	result = LoadFontData(fontFilename);
	if(!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::Shutdown()
{
	ReleaseTexture();
	ReleaseFontData();
	return;
}


bool FontClass::LoadFontData(char* filename)
{
    std::ifstream file(filename);
    if (!file) return false;
    auto metrics = std::make_unique<FontType[]>(95);
    std::string line;
    for (int i = 0; i < 95; ++i)
    {
        if (!std::getline(file, line)) return false;
        std::istringstream fields(line);
        int code;
        if (!(fields >> code) || code != i + 32) return false;
        if (code != 32)
        {
            char glyph;
            if (!(fields >> glyph)) return false;
        }
        auto& metric = metrics[i];
        if (!(fields >> metric.left >> metric.right >> metric.size) ||
            !(metric.left >= 0 && metric.right >= metric.left && metric.right <= 1) ||
            metric.size < 0)
            return false;
    }
    delete[] m_Font;
    m_Font = metrics.release();
    return true;
}

void FontClass::ReleaseFontData()
{
	
	if(m_Font)
	{
		delete [] m_Font;
		m_Font = 0;
	}

	return;
}


bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::ReleaseTexture()
{

	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}


ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	vertexPtr = (VertexType*)vertices;
	numLetters = (int)strlen(sentence);
	index = 0;
		
	for(i=0; i<numLetters; i++)
	{
		letter = static_cast<unsigned char>(sentence[i]) - 32;
        if (letter < 0 || letter >= 95) letter = '?' - 32;
			
		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

		
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}