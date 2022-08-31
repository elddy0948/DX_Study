#pragma once

class Matrix
{
public:
	Matrix(int sizeX, int sizeY);
	~Matrix();

	bool Get(int x, int y) const;
	void Set(int x, int y, bool value);

	int GetXSize() const;
	int GetYSize() const;

private:
	int xSize;
	int ySize;
	bool** matrix;
};