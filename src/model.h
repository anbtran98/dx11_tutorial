////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "texture.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(int);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void ReleaseTextures();

	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
    void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture* m_Textures;
	ModelType* m_model;
};

#endif

// #ifndef _MODEL_H
// #define _MODEL_H

// #include <d3d11.h>
// #include <directxmath.h>
// #include <fstream>

// #include "texture.h"

// class Model {
//     struct VertexType {
//         DirectX::XMFLOAT3 position;
//         DirectX::XMFLOAT2 texture;
//         DirectX::XMFLOAT3 normal;
//         DirectX::XMFLOAT3 tangent;
//         DirectX::XMFLOAT3 binormal;
//     };

//     struct ModelType {
//         float x, y, z;
//         float tu, tv;
//         float nx, ny, nz;
//         float tx, ty, tz;
//         float bx, by, bz;
//     };

//     struct TempVertexType {
//         float x, y, z;
//         float tu, tv;
//         float nx, ny, nz;
//     };

//     struct VectorType {
//         float x, y, z;
//     };

//     ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
//     Texture* mTextures;
//     int mVertexCount, mIndexCount;
//     ModelType* mModel;
 
//     bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
//     void ReleaseTextures();
//     bool InitializeBuffers(ID3D11Device*);
//     void ShutdownBuffers();
//     void RenderBuffers(ID3D11DeviceContext*);
//     bool LoadModel(char*);
//     void ReleaseModel();
//     void CalculateModelVectors();
//     void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

//  public:
//     Model();
//     Model(const Model&);
//     ~Model();

//     bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*, char*);
//     void Shutdown();
//     void Render(ID3D11DeviceContext*);
//     int GetIndexCount();
//     ID3D11ShaderResourceView* GetTexture(int);
// };

// #endif _MODEL_H
