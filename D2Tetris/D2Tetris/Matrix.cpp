#include "Matrix.h"

Matrix::Matrix(int sizeX, int sizeY)
	: xSize(sizeX),
	ySize(sizeY),
	matrix(new bool* [sizeY])
{
	for (int i = 0; i < ySize; ++i)
	{
		matrix[i] = new bool[xSize];
		for (int j = 0; j < xSize; ++j)
		{
			matrix[i][j] = false;
		}
	}
}

Matrix::~Matrix()
{
	for (int i = 0; i < ySize; ++i)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}

bool Matrix::Get(int x, int y) const
{
	return matrix[y][x];
}

void Matrix::Set(int x, int y, bool value)
{
	matrix[y][x] = value;
}

int Matrix::GetXSize() const
{
	return xSize;
}

int Matrix::GetYSize() const
{
	return ySize;
}