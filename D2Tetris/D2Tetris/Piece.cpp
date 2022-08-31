#include "Piece.h"
#include "Stack.h"
#include "Engine.h"

Piece::Piece()
	: m_pRedBrush(NULL)
{
	position.x = (STACK_WIDTH / 2) - 2;
	position.y = 0;

	waiting = true;

	cells = new Matrix(4, 4);

	int pieceType = rand() % 7;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cells->Set(j, i, cellsTemplates[pieceType][i][j]);
		}
	}
}

Piece::~Piece()
{
	delete cells;
	if (m_pRedBrush != NULL)
	{
		m_pRedBrush->Release();
		m_pRedBrush = NULL;
	}
}

void Piece::InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pRedBrush
	);
}

void Piece::Activate()
{
	waiting = false;
}

bool Piece::Advance(Matrix* stackCells)
{
	position.y += 1;
	if (StackCollision(stackCells))
	{
		position.y -= 1;
		return true;
	}
	return false;
}

void Piece::GoLeft(Matrix* stackCells)
{
	int initialPositionX = position.x;
	position.x -= 1;

	if (LeftWallCollision())
	{
		position.x = initialPositionX;
		return;
	}

	if (StackCollision(stackCells))
	{
		position.x = initialPositionX;
		return;
	}
}

void Piece::GoRight(Matrix* stackCells)
{
	int initialPositionX = position.x;
	position.x += 1;

	if (RightWallCollision())
	{
		position.x = initialPositionX;
		return;
	}

	if (StackCollision(stackCells))
	{
		position.x = initialPositionX;
		return;
	}
}

void Piece::Rotate(Matrix* stackCells)
{
	// Store initial values
	Matrix* temp = new Matrix(4, 4);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp->Set(j, i, cells->Get(j, i));
		}
	}

	int initialPositionX = position.x;

	//Rotate
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cells->Set(j, i, temp->Get(i, 3 - j));
		}
	}

	while (LeftWallCollision())
	{
		position.x += 1;
	}

	while (RightWallCollision())
	{
		position.x -= 1;
	}

	if (StackCollision(stackCells))
	{
		//Revert
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				cells->Set(j, i, temp->Get(j, i));
			}
		}
		position.x = initialPositionX;
		return;
	}
}

void Piece::Draw(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	int padding = (RESOLUTION_Y - (STACK_HEIGHT + 1) * CELL_SIZE) / 2;
	int centerRight = RESOLUTION_X - (RESOLUTION_Y - padding - (STACK_WIDTH + 2) * CELL_SIZE) / 2;

	int centerX = centerRight;
	int centerY = padding + 2 * CELL_SIZE + 100;

	if (!waiting)
	{
		centerX = padding + (position.x + 1) * CELL_SIZE;
		centerY = padding + position.y * CELL_SIZE;
	}

	// Drawing the cells
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				D2D1_RECT_F rectangle4 = D2D1::RectF(
					centerX + j * CELL_SIZE + 1,
					centerY + i * CELL_SIZE + 1,
					centerX + (j + 1) * CELL_SIZE - 1,
					centerY + (i + 1) * CELL_SIZE - 1
				);

				m_pRenderTarget->FillRectangle(&rectangle4, m_pRedBrush);
			}
		}
	}
}

bool Piece::LeftWallCollision()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				int realX = position.x + j;
				if (realX < 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Piece::RightWallCollision()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				int realX = position.x + j;
				if (realX >= STACK_WIDTH)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Piece::StackCollision(Matrix* stackCells)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				int realX = position.x + j;
				int realY = position.y + i;

				// Check collision with the bottom
				if (realY >= STACK_HEIGHT)
				{
					return true;
				}

				// Check collision with existing stack
				if (stackCells->Get(realX, realY))
				{
					return true;
				}
			}
		}
	}
	return false;
}

Point2D Piece::GetPosition()
{
	return position;
}

Matrix* Piece::GetCells()
{
	return cells;
}