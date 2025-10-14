
#pragma once

#include <string>
#include <vector>

class Terrain
{
public:
	Terrain(
		IDirect3DDevice9* device,
		//std::string heightmapFileName, 
		int numVertsPerRow,  
		int numVertsPerCol, 
		int cellSpacing,    // space between cells
		float heightScale);   

	~Terrain();

	//int  getHeightmapEntry(int row, int col);
	//void setHeightmapEntry(int row, int col, int value);

	//float getHeight(float x, float z);

	bool  loadTexture(char* fileName);
	//bool  genTexture(D3DXVECTOR3* directionToLight);
	bool  Render(D3DXMATRIX* world);
	void  Resize(int numVertsPerRow, int numVertsPerCol, int cellSpacing);
	void  Save(CFileIO &File);
	void  Load(CFileIO &File);
private:
	IDirect3DDevice9*       m_pDevice;
	IDirect3DTexture9*      _tex;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib;

	int _numVertsPerRow;
	int _numVertsPerCol;
	int _cellSpacing;

	int _numCellsPerRow;
	int _numCellsPerCol;
	int _width;
	int _depth;
	int _numVertices;
	int _numTriangles;

	float _heightScale;

	char m_FileName[128];

	std::vector<int> _heightmap;

	// helper methods
	//bool  readRawFile(std::string fileName);
	bool  computeVertices();
	bool  computeIndices();
	bool  lightTerrain(D3DXVECTOR3* directionToLight);
	float computeShade(int cellRow, int cellCol, D3DXVECTOR3* directionToLight);

	struct TerrainVertex
	{
		TerrainVertex(){}
		TerrainVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		{
			_x = x; _y = y; _z = z; _u = u; _v = v; _nx = nx; _ny = ny; _nz = nz;
		}
		
		float _x, _y, _z;
		float _nx, _ny, _nz;
		float _u, _v;

		static const DWORD FVF;
	};

	
};

