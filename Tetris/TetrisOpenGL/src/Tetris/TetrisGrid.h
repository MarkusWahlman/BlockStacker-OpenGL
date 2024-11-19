#pragma once

#include <iostream>
#include <array>
#include <vector>

#include "../debug.h"

constexpr int g_GridXSize = 10;
constexpr int g_GridYSize = 20;

class GridBlock
{
public:
	GridBlock()
		: BlockVertices({}), activeBlock(false), blockXPos(0), blockYPos(0)
	{};

	GridBlock(float BlockXPos, float BlockYPos, float BlockZPos, 
		float ColorR, float ColorG, float ColorB, float ColorA,
		float BlockXSize, float BlockYSize,
		bool active)
		: 
		BlockVertices({
		/*Position*/ BlockXSize*BlockXPos,				BlockYSize*BlockYPos,				BlockZPos, /*Color*/ColorR, ColorG, ColorB, ColorA,
					 BlockXSize*BlockXPos+BlockXSize,	BlockYSize*BlockYPos,				BlockZPos,			ColorR, ColorG, ColorB, ColorA,
					 BlockXSize*BlockXPos+BlockXSize,	BlockYSize*BlockYPos+BlockYSize,	BlockZPos,			ColorR, ColorG, ColorB, ColorA,
					 BlockXSize*BlockXPos,				BlockYSize*BlockYPos+BlockYSize,	BlockZPos,			ColorR, ColorG, ColorB, ColorA,
					}),
		activeBlock(active),
		blockXPos(BlockXPos),
		blockYPos(BlockYPos)
	{};
	bool activeBlock;

	float blockXPos; //These should be const, the GridBlock class SHOULDN'T be done like this, but it will do for this project.
					 //...
	float blockYPos;

	std::array<float, 7 * 4> BlockVertices;
};

class TetrisGrid
{
	/*20x10 Grid for tetris*/
public:
	TetrisGrid(float WindowX, float WindowY, float r, float g, float b)
		: m_GridBlocks({}), m_Indices({})
	{
		float blockXSize = WindowX / g_GridXSize;
		float blockYSize = WindowY / g_GridYSize;

		for (int i = 0; i < g_GridXSize; ++i)
		{
			for (int j = 0; j < g_GridYSize; ++j)
			{
				m_GridBlocks[i][j] = GridBlock{
					/*Position*/	(float)i, (float)j, 0.0f,
					/*Color*/		r, g, b, 0.0f,
					/*Size*/		blockXSize, blockYSize,
					/*Active*/		false
				};

				int BlockCount = j + i * g_GridYSize;
				int CurrentPos = BlockCount * 6;

				m_Indices[CurrentPos] = BlockCount*4;
				m_Indices[CurrentPos +1] = BlockCount*4 + 1;
				m_Indices[CurrentPos + 2] = BlockCount*4 + 2;
				m_Indices[CurrentPos + 3] = BlockCount*4 + 2;
				m_Indices[CurrentPos + 4] = BlockCount*4 + 3;
				m_Indices[CurrentPos + 5] = BlockCount*4;
			}
		}

		//Spawn area *Hardcoded to blue, the grid class is overly compilated already and just wrong, so lets stick to the theme
		for (int i = g_GridXSize / 2 - 2; i < g_GridXSize / 2+2; ++i)
		{
			for (int j = g_GridYSize-1; j > g_GridYSize-3; --j)
			{
				m_GridBlocks[i][j] = GridBlock{
					/*Position*/	(float)i, (float)j, 0.0f,
					/*Color*/		0.0f, 0.0f, 1.0f, 0.0f,
					/*Size*/		blockXSize, blockYSize,
					/*Active*/		false
				};
			}
		}
	};

	const std::array<uint32_t, 6 * g_GridXSize * g_GridYSize>& GetIndices()
	{
		return m_Indices;
	}

	const std::vector<float> GetActiveVerticies()
	{
		std::vector<float> blockVertices;
		/*SpawnArea is always active*/
		for (int i = g_GridXSize / 2 - 2; i < g_GridXSize / 2 + 2; ++i)
		{
			for (int j = g_GridYSize - 1; j > g_GridYSize - 3; --j)
			{
				for (int k = 0; k < (int)m_GridBlocks[i][j].BlockVertices.size(); ++k)
				{
					blockVertices.push_back(m_GridBlocks[i][j].BlockVertices[k]);
				}
			}
		}

		/*Active blocks*/
		for (int i = 0; i < (int)m_GridBlocks.size(); ++i)
		{
			for (int j = 0; j < (int)m_GridBlocks[i].size(); ++j)
			{
				if (m_GridBlocks[i][j].activeBlock)
				{
					for (int k = 0; k < (int)m_GridBlocks[i][j].BlockVertices.size(); ++k)
					{
						blockVertices.push_back(m_GridBlocks[i][j].BlockVertices[k]);
					}
				}
			}
		}
		return blockVertices;
	}

	const std::array<std::array<GridBlock, g_GridYSize>, g_GridXSize>& GetGridBlocks()
	{
		return m_GridBlocks;
	}

	void DisableGridRow(int yRow)
	{
		for (int i = 0; i < g_GridXSize; ++i)
		{
			m_GridBlocks[i][yRow].activeBlock = false;
		}
	}

	void DebugSingleBlock(int x, int y, bool active)
	{
		m_GridBlocks[x][y].activeBlock = active;
	}

	friend class TetrisShape;

protected:
	std::array<uint32_t, 6*g_GridXSize*g_GridYSize> m_Indices;
	std::array<std::array<GridBlock, g_GridYSize>, g_GridXSize> m_GridBlocks;
};


static std::vector<std::pair<int, int>> Get3x3RotationCoordinates(int middleX, int middleY)
/*
	Returns the coordinates of a given position
	0- 1- 2-
	7- M- 3-
	6- 5- 4-
*/
{
	std::vector<std::pair<int, int>> coordinates;

	coordinates.push_back(std::pair<int, int>(middleX - 1, middleY + 1));	//0
	coordinates.push_back(std::pair<int, int>(middleX, middleY + 1));		//1
	coordinates.push_back(std::pair<int, int>(middleX + 1, middleY + 1));	//2
	coordinates.push_back(std::pair<int, int>(middleX + 1, middleY));		//3
	coordinates.push_back(std::pair<int, int>(middleX + 1, middleY - 1));	//4
	coordinates.push_back(std::pair<int, int>(middleX, middleY - 1));		//5
	coordinates.push_back(std::pair<int, int>(middleX - 1, middleY - 1));	//6
	coordinates.push_back(std::pair<int, int>(middleX - 1, middleY));		//7

	return coordinates;
}

static std::vector<std::pair<int, int>> Get5x5RotationMiddleCoordinates(int middleX, int middleY)
/*
	Returns the coordinates of a given position
	- - 1 - -
	- - - - -
	0 - M - 2
	- - - - -
	- - 3 - -
*/
{
	std::vector<std::pair<int, int>> coordinates;

	coordinates.push_back(std::pair<int, int>(middleX - 2, middleY));	//0
	coordinates.push_back(std::pair<int, int>(middleX, middleY +2));	//1
	coordinates.push_back(std::pair<int, int>(middleX + 2, middleY));	//2
	coordinates.push_back(std::pair<int, int>(middleX, middleY - 2));	//2

	return coordinates;
}


class TetrisShape
	/*Tetris shapes are bound to a grid, and they alter the grid*/
{
public:
	enum class STATUS
	{
		DISABLED,
		DESTROYED,
		BLOCKED,
		MOVING
	};

	enum class NUMBER
	{
		EMPTY,
		LBLOCK,
		RLBLOCK,
		BOXBLOCK,
		LONGBLOCK,
		ZBLOCK,
		RZBLOCK,
		TBLOCK
	};

	static void EnablePhysics(bool shouldEnable)
	{
		sm_EnablePhysics = shouldEnable;
	}

	bool CollisionOnSpawn()
	{
		return m_CollisionOnSpawn;
	}

	STATUS Update()
	{
		if (!sm_EnablePhysics)
			return STATUS::DISABLED;


 		if (m_ShapeBlocks.empty())
		{
			Debug::Message("A shape returned STATUS::DESTROYED");
			return STATUS::DESTROYED;
		}
				
		/*Loop through the blocks to see if parts are removed or if it's blocked*/
		for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
		{
			if (m_ShapeBlocks[i]->blockYPos < 1)	/*The block hit the bottom layer 0*/
				return STATUS::BLOCKED;

			GridBlock* nextBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos][(size_t)m_ShapeBlocks[i]->blockYPos - 1];

			if (!m_ShapeBlocks[i]->activeBlock)	/*The block was destroyed*/
			{
				//@todo THIS DOESN'T WORK AS INTENDED
				m_ShapeBlocks.erase(m_ShapeBlocks.begin() + i);
				--i;
				continue;
			}
			if (nextBlock->activeBlock)
			{
				for (int y = 0; y < (int)m_ShapeBlocks.size(); ++y)
				{
					if (m_ShapeBlocks[y] == nextBlock)
							break;
					if(y == (int)m_ShapeBlocks.size()-1)
						return STATUS::BLOCKED;
				}
			}
		}
			
		/*Move the whole block down*/
		std::vector<GridBlock> shapeBlocksCopy;

		for (GridBlock* block : m_ShapeBlocks)
			shapeBlocksCopy.push_back(*block);

		for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
		{
			bool disableCurrent = true;

			if (m_ShapeBlocks[i]->blockYPos + 1 < g_GridYSize) /*The block is under the top layer*/
			{
				GridBlock* prevBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos][(size_t)m_ShapeBlocks[i]->blockYPos + 1];
				for (int y = 0; y < (int)m_ShapeBlocks.size(); ++y)
				{
					if (shapeBlocksCopy[y].blockXPos == prevBlock->blockXPos && shapeBlocksCopy[y].blockYPos == prevBlock->blockYPos) /*The previous block is a part of the shape, we don't want to disable it.*/
					{
						disableCurrent = false;
						break;
					}
				}
			}

			if (disableCurrent)
				m_ShapeBlocks[i]->activeBlock = false;

			GridBlock* nextBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos][(size_t)m_ShapeBlocks[i]->blockYPos - 1];
			nextBlock->activeBlock = true;
			m_ShapeBlocks[i] = nextBlock;
		}
			
		return STATUS::MOVING;
	}

	enum class MOVEDIR
	{
		NONE,
		RIGHT,
		LEFT
	};

	STATUS Move(MOVEDIR direction)
	{
		if (!sm_EnablePhysics)
			return STATUS::DISABLED;

		if (direction == MOVEDIR::RIGHT)
		{
			/*Loop through the blocks to see if parts are removed or if it's blocked*/
			for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
			{
				if (m_ShapeBlocks[i]->blockXPos >= g_GridXSize-1)	/*The block hit the right corner*/
					return STATUS::BLOCKED;

				GridBlock* nextBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos+1][(size_t)m_ShapeBlocks[i]->blockYPos];

				/*Check if the block belongs to the shape*/
				if (nextBlock->activeBlock)
				{
					for (int y = 0; y < (int)m_ShapeBlocks.size(); ++y)
					{
						if (m_ShapeBlocks[y] == nextBlock)
							break;
						if (y == (int)m_ShapeBlocks.size() - 1)
							return STATUS::BLOCKED;
					}
				}
			}

			/*Move to the right!*/
			std::vector<GridBlock> shapeBlocksCopy;

			for (GridBlock* block : m_ShapeBlocks)
				shapeBlocksCopy.push_back(*block);

			for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
			{
				bool disableCurrent = true;

				if (m_ShapeBlocks[i]->blockXPos > 0) /*The block is not on the far left*/
				{
					GridBlock* prevBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos-1][(size_t)m_ShapeBlocks[i]->blockYPos];
					for (int y = 0; y < (int)m_ShapeBlocks.size(); ++y)
					{
						if (shapeBlocksCopy[y].blockXPos == prevBlock->blockXPos && shapeBlocksCopy[y].blockYPos == prevBlock->blockYPos) 
							/*The previous block is a part of the shape, we don't want to disable it.*/
						{
							disableCurrent = false;
							break;
						}
					}
				}
				else
					disableCurrent = true; //@todo remove
					

				if (disableCurrent)
					m_ShapeBlocks[i]->activeBlock = false;

				GridBlock* nextBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos+1][(size_t)m_ShapeBlocks[i]->blockYPos];
				nextBlock->activeBlock = true;
				m_ShapeBlocks[i] = nextBlock;
			}
			return STATUS::MOVING;

		}
		else if (direction == MOVEDIR::LEFT)
		{
			/*Loop through the blocks to see if parts are removed or if it's blocked*/
			for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
			{
				if (m_ShapeBlocks[i]->blockXPos < 1)	/*The block hit the left corner*/
					return STATUS::BLOCKED;

				GridBlock* nextBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos - 1][(size_t)m_ShapeBlocks[i]->blockYPos];

				/*Check if the block belongs to the shape*/
				if (nextBlock->activeBlock)
				{
					for (int y = 0; y < (int)m_ShapeBlocks.size(); ++y)
					{
						if (m_ShapeBlocks[y] == nextBlock)
							break;
						if (y == (int)m_ShapeBlocks.size() - 1)
							return STATUS::BLOCKED;
					}
				}
			}

			/*Move to the left!*/
			std::vector<GridBlock> shapeBlocksCopy;

			for (GridBlock* block : m_ShapeBlocks)
				shapeBlocksCopy.push_back(*block);

			for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
			{
				bool disableCurrent = true;

				if (m_ShapeBlocks[i]->blockXPos + 1 < g_GridXSize) /*The block is not on the far right*/
				{
					GridBlock* prevBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos + 1][(size_t)m_ShapeBlocks[i]->blockYPos];
					for (int y = 0; y < (int)m_ShapeBlocks.size(); ++y)
					{
						if (shapeBlocksCopy[y].blockXPos == prevBlock->blockXPos && shapeBlocksCopy[y].blockYPos == prevBlock->blockYPos)
							/*The previous block is a part of the shape, we don't want to disable it.*/
						{
							disableCurrent = false;
							break;
						}
					}
				}

				if (disableCurrent)
					m_ShapeBlocks[i]->activeBlock = false;

				GridBlock* nextBlock = &m_Grid->m_GridBlocks[(size_t)m_ShapeBlocks[i]->blockXPos - 1][(size_t)m_ShapeBlocks[i]->blockYPos];
				nextBlock->activeBlock = true;
				m_ShapeBlocks[i] = nextBlock;
			}
			return STATUS::MOVING;
		}
		return STATUS::DESTROYED; //unreachable
	}


	std::pair<int, int> GetMiddleCoordinates()
	{
		return std::pair<int,int>(m_ShapeBlocks[0]->blockXPos, m_ShapeBlocks[0]->blockYPos);
	}

	virtual STATUS Rotate()	/*Defined for a 3x3 shape*/
	{
		if (!sm_EnablePhysics)
			return STATUS::DISABLED;

		std::pair<int, int> midCoords = GetMiddleCoordinates();

		std::vector<std::pair<int, int>> rotCoordinates = Get3x3RotationCoordinates(midCoords.first, midCoords.second);

		std::vector<GridBlock*> nextBlocks;
		for (GridBlock* block : m_ShapeBlocks)	//Copy current blocks
			nextBlocks.push_back(block);

		/*calculate nextBlocks values (and check that we don't go over the grid)*/
		for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
		{
			/*	0- 1- 2-
				7- M- 3-
				6- 5- 4-
			*/
			for (int j = 0; j < (int)rotCoordinates.size(); ++j)
			{
				if (rotCoordinates[j].first > g_GridXSize-1 || rotCoordinates[i].first < 0)
					return STATUS::BLOCKED;

				if (rotCoordinates[j].second < 0)
					return STATUS::BLOCKED;

				if (m_ShapeBlocks[i]->blockXPos == rotCoordinates[j].first && m_ShapeBlocks[i]->blockYPos == rotCoordinates[j].second)
				{
					int nextRotCoordIndex = j;
					nextRotCoordIndex += 2;
					if (nextRotCoordIndex > 7)
						nextRotCoordIndex -= 8;

					if (rotCoordinates[nextRotCoordIndex].first > g_GridXSize-1 || rotCoordinates[nextRotCoordIndex].first < 0)
						return STATUS::BLOCKED;

					if (rotCoordinates[nextRotCoordIndex].second < 0)
						return STATUS::BLOCKED;

					GridBlock* nextBlock = &GetGridBlocks()[rotCoordinates[nextRotCoordIndex].first][rotCoordinates[nextRotCoordIndex].second];

					nextBlocks[i] = nextBlock;
				}
			}
		}

		/*	Check if we can rotate (check if the rotation gets blocked) 
			@todo IF the rotation gets blocked, we should go up and find a position where it doesn't.
		*/
		for (int i = 0; i < (int)nextBlocks.size(); ++i)
		{
			if (std::find(m_ShapeBlocks.begin(), m_ShapeBlocks.end(), nextBlocks[i]) == m_ShapeBlocks.end())
				if (nextBlocks[i]->activeBlock)
					return STATUS::BLOCKED;
		}

		/*Do the rotation*/
		for (int i = 0; i < (int)nextBlocks.size(); ++i)
		{
			/*nextBlocks doesn't contain m_ShapeBlocks[i]*/
			if (std::find(nextBlocks.begin(), nextBlocks.end(), m_ShapeBlocks[i]) == nextBlocks.end()) 
				m_ShapeBlocks[i]->activeBlock = false;

			m_ShapeBlocks[i] = nextBlocks[i];
			m_ShapeBlocks[i]->activeBlock = true;
		}
		return STATUS::MOVING;
	}

	~TetrisShape()
	{
		for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
		{
			m_ShapeBlocks[i]->activeBlock = false;
		}
	}

protected:
	TetrisShape(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: m_Grid(grid), m_ShapeBlocks({}), m_CollisionOnSpawn(false)
	{}

	void ActivateShapeBlocks()
	{
		for (GridBlock* block : m_ShapeBlocks)
		{
			//@todo If they are already activated here, it means collision on spawn / start again
			if(block->activeBlock)
			 {
				m_CollisionOnSpawn = true;
			 }
			block->activeBlock = true;
		}
			
	}

	std::array<std::array<GridBlock, g_GridYSize>, g_GridXSize>& GetGridBlocks()
	{
		return m_Grid->m_GridBlocks;
	}

	bool m_CollisionOnSpawn;
	TetrisGrid* m_Grid;
	std::vector<GridBlock*> m_ShapeBlocks;

	inline static bool sm_EnablePhysics = true;	//@todo Use for pausing
};


///
/// YOU COULD FOR SURE SOMEHOW ABUSE TEMPLATES TO MAKE THE BLOCK CLASSES. I'll try that next time.
/// The LBlock class should be an abstract class. We'd have a "NormalLBlock" class with that so we could inherit the rotate() buttttttttttttt
///

class LBlock : public TetrisShape
{
public:
	LBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)
		{
			Debug::Message("LBlock spawn out-of-bounds");
			return;
		}

		/*The L block 
			    ->X
			X<-X->X
			
		*/
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY]);

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY+1]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY]);

		ActivateShapeBlocks();
	}

};

class ReverseLBlock : public TetrisShape
{
public:
	ReverseLBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)
		{
			Debug::Message("LBlock spawn out-of-bounds");
			return;
		}

		/*The L block
			X<-
			X<-X->X

		*/
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY+1]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY]);

		ActivateShapeBlocks();
	}
};


class BoxBlock : public TetrisShape
{
public:
	BoxBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)	
		{
			Debug::Message("BoxBlock spawn out-of-bounds");
			return;
		}

		/*The Box block
			X -> X
			X -> X
		*/
		for (int y = 0; y < 2; ++y)
		{
			m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY - y]);
			m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX + 1][spawnPointY - y]);
		}
		ActivateShapeBlocks();
	}
	STATUS Rotate() override
	{/*Doesn't rotate*/
		return STATUS::BLOCKED;
	}
};

class LongBlock : public TetrisShape
{
public:
	LongBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)
		{
			Debug::Message("LongBlock spawn out-of-bounds");
			return;
		}

		/*The Long block
			X <- X -> X -> X
		*/

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-2][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY]);

		ActivateShapeBlocks();
	}

	STATUS Rotate() override
	{/*@todo*/
		if (!sm_EnablePhysics)
			return STATUS::DISABLED;

		std::pair<int, int> midCoords = GetMiddleCoordinates();

		std::vector<std::pair<int, int>> rotCoordinates = Get5x5RotationMiddleCoordinates(midCoords.first, midCoords.second);

		std::vector<GridBlock*> nextBlocks;
		for (GridBlock* block : m_ShapeBlocks)	//Copy current blocks
			nextBlocks.push_back(block);

		/*calculate nextBlocks values (and check that we don't go over the grid)*/
		for (int i = 0; i < (int)m_ShapeBlocks.size(); ++i)
		{
			/*	
			*	- - 1 - -
				- - - - -
				0 - M - 2
				- - - - -
				- - 3 - -
			*/
			for (int j = 0; j < (int)rotCoordinates.size(); ++j)
			{
				if (rotCoordinates[j].first > g_GridXSize - 1 || rotCoordinates[i].first < 0)
					return STATUS::BLOCKED;

				if (rotCoordinates[j].second < 0)
					return STATUS::BLOCKED;

				if (m_ShapeBlocks[i]->blockXPos == rotCoordinates[j].first && m_ShapeBlocks[i]->blockYPos == rotCoordinates[j].second)
				{
					int nextRotCoordIndex = j;
					nextRotCoordIndex += 1;
					if (nextRotCoordIndex > 3)
						nextRotCoordIndex -= 4;

					if (rotCoordinates[nextRotCoordIndex].first > g_GridXSize - 1 || rotCoordinates[nextRotCoordIndex].first < 0)
						return STATUS::BLOCKED;

					if (rotCoordinates[nextRotCoordIndex].second < 0 || rotCoordinates[nextRotCoordIndex].second > g_GridYSize-1)
						return STATUS::BLOCKED;

					GridBlock* nextBlock = &GetGridBlocks()[rotCoordinates[nextRotCoordIndex].first][rotCoordinates[nextRotCoordIndex].second];

					nextBlocks[i] = nextBlock;
				}
			}
		}

		/*	Check if we can rotate (check if the rotation gets blocked)
			@todo IF the rotation gets blocked, we should go up and find a position where it doesn't.
		*/
		for (int i = 0; i < (int)nextBlocks.size(); ++i)
		{
			if (std::find(m_ShapeBlocks.begin(), m_ShapeBlocks.end(), nextBlocks[i]) == m_ShapeBlocks.end())
				if (nextBlocks[i]->activeBlock)
					return STATUS::BLOCKED;
		}

		if(TetrisShape::Rotate() == STATUS::BLOCKED)
			return STATUS::BLOCKED;	/*Couldn't rotate*/

		/*Do the rotation*/
		for (int i = 0; i < (int)nextBlocks.size(); ++i)
		{
			/*nextBlocks doesn't contain m_ShapeBlocks[i]*/
			if (std::find(nextBlocks.begin(), nextBlocks.end(), m_ShapeBlocks[i]) == nextBlocks.end())
				m_ShapeBlocks[i]->activeBlock = false;

			m_ShapeBlocks[i] = nextBlocks[i];
			m_ShapeBlocks[i]->activeBlock = true;
		}
		/*We still need to rotate the rest of the blocks again, as we used the old positions for this rotation and it set the other blocks back
		(I'm lazy it's awful already, and it works why fix it)*/
		TetrisShape::Rotate();

		return STATUS::MOVING;
	}
};

class ZBlock : public TetrisShape
{
public:
	ZBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)
		{
			Debug::Message("ZBlock spawn out-of-bounds");
			return;
		}

		/*The ZBlock block
				X -> X
			X<- X
			-	-	 -
		*/

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY]);

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY+1]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY+1]);

		ActivateShapeBlocks();
	}
};

class ReverseZBlock : public TetrisShape
{
public:
	ReverseZBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)
		{
			Debug::Message("ReverseZBlock spawn out-of-bounds");
			return;
		}

		/*The ReverseZBlock block
			X<-X
			   X->X
		*/

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY]);

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY+1]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY+1]);

		ActivateShapeBlocks();
	}
};

class TBlock : public TetrisShape
{
public:
	TBlock(TetrisGrid* grid, int spawnPointX, int spawnPointY)
		: TetrisShape(grid, spawnPointX, spawnPointY)
	{
		if (spawnPointY < 2 || spawnPointX > g_GridXSize - 3)
		{
			Debug::Message("TBlock spawn out-of-bounds");
			return;
		}

		/*The TBlock block
			-  X  -
			X<-X->X
		*/

		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX][spawnPointY+1]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX-1][spawnPointY]);
		m_ShapeBlocks.push_back(&GetGridBlocks()[spawnPointX+1][spawnPointY]);

		ActivateShapeBlocks();
	}
};
