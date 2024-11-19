#include "TetrisGame.h"
#include "../shaders.h"

#include <fstream>
#include <string>
#include <sstream>

static bool s_UpArrowPressed = false;
static bool s_LeftArrowPressed = false;
static bool s_RightArrowPressed = false;
static bool s_SpacePressed = false;
static bool s_DownArrowDown = false;

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP)
	{
		if (action == GLFW_PRESS)
			s_UpArrowPressed = true;
	}
	if (key == GLFW_KEY_LEFT)
	{
		if (action == GLFW_PRESS)
			s_LeftArrowPressed = true;
	}
	if (key == GLFW_KEY_RIGHT)
	{
		if (action == GLFW_PRESS)
			s_RightArrowPressed = true;
		
	}
	if (key == GLFW_KEY_SPACE)
	{
		if (action == GLFW_PRESS)
			s_SpacePressed = true;
	}
	if (key == GLFW_KEY_DOWN)
	{
		if (action == GLFW_PRESS)
			s_DownArrowDown = true;
		if (action == GLFW_RELEASE)
			s_DownArrowDown = false;
	}
}

static int findFirstIntFromString(std::string str)
{
	std::stringstream ss(str);

	std::string temp;
	int found;
	while (!ss.eof()) 
	{
		ss >> temp;
		if (std::stringstream(temp) >> found)
			return found;
		temp = "";
	}
	return 0;
}

TetrisGame::~TetrisGame()
{
	glfwDestroyWindow(m_GameWindow);
}

bool TetrisGame::initGame()
{
	/*Create GLFW window*/
	if (!glfwInit())
	{
		Debug::Message("Couldn't initialize GLFW library!");
		return false;
	}

	m_GameWindow = glfwCreateWindow(m_WindowX, m_WindowY, "TETRIS", nullptr, nullptr);
	glfwMakeContextCurrent(m_GameWindow);

	if (m_GameWindow == NULL)
	{
		Debug::Message("Couldn't create window!");
		return false;
	}

	if (glewInit() != GLEW_OK)
	{
		Debug::Message("Couldn't initialize GLEW!");
		return false;
	}

	glfwSetKeyCallback(m_GameWindow, glfwKeyCallback);

	/*Create shaders*/
	m_Shader = Shader::CreateShader();
	if (m_Shader == 0)
		Debug::Message("Couldn't create OpenGLShader");

	glUseProgram(m_Shader);

	/*If there's no config file, generate one.
	The spawn file contains the info on what blocks to spawn(the order), and the game slowness.*/

	std::string fileName = "tetris.cfg"; //@todo name?
	std::ifstream ifs{ fileName };	

	if (!ifs)
	{
		Debug::Message("No existing .cfg file found, creating a new one");

		std::ofstream ofs{ fileName };
		if (!ofs)
		{
			Debug::Message("Couldn't open ", fileName, " for writing");
			return false; //@todo Could just continue but not create a file..?
		}

		int totalBlocks = 0;

		if (ofs)
		{
			ofs << "Blocks: ";
		}

		int extraSpace = rand() % (0 - 12 + 1) + 12; //Extra space/"difficulty" (0-12) (meaning 0-4 block extra space)
		while (totalBlocks+4 <= g_GridXSize * g_GridYSize-8-extraSpace)/*-8 spawn size*/
		{
			int blockNumber = rand() % (7 - 1 + 1) + 1; //Get new random block (Number 1-7)

			if (ofs)
			{
				ofs << blockNumber;
			}
			m_BlockNumbersToSpawn.push_back(blockNumber);

			totalBlocks += 4;	//Each block has a size of 4.
		}

		int slowness = rand() % (150 - 75 + 1) + 75; //Get new random slowness (75-150)
		m_Slowness = slowness;
		if (ofs)
		{
			ofs << '\n' << "Slowness: " << slowness;
		}
			
	}
	else
	{
		std::string curLine;
		while (std::getline(ifs, curLine))
		{
			if (curLine.find("Blocks:") != std::string::npos) 
			{
				int blockNumsFound = 0;
				for (char c : curLine)
				{
					if (blockNumsFound >= 48) //Maximum of 48 blocks can fit
						break;

					if (isdigit(c))
					{
						++blockNumsFound;
						m_BlockNumbersToSpawn.push_back(int(c - '0'));
					}
				}
			}
			else if (curLine.find("Slowness:") != std::string::npos)
			{
				int slowness = findFirstIntFromString(curLine);
				if (slowness)
					m_Slowness = slowness;
			}
		}
	}
	

	/* The player will be rendered on a square map that's originally 1024x1024
	on our map (0, 0) = bottom left AND (1024, 1024) = top right*/

	glm::mat4 gameProj{};
	glm::mat4 gameView{};
	glm::mat4 gameModel{};

	gameProj = glm::ortho(0.0f, (float)m_WindowX, 0.0f, (float)m_WindowY, -1.0f, 1.0f);
	gameView = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	gameModel = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 tetrisMvp = gameProj * gameView * gameModel;

	Shader::SetUniformMat4f("u_MVP", m_Shader, tetrisMvp);

	/*@todo Create OpenGL grid with 25x25 pixel blocks (10x20 grid)*/
	//@todo Could use a matrix to alter the blocks (200 blocks)

	float vertices[]
	{
	/*Position*/  0.f,	  0.f, 0.0f, /*Color*/ 0.0f, 1.0f, 0.0f, 1.0f,
				 25.f,	  0.f, 0.0f,			0.0f, 1.0f, 0.0f, 1.0f,
				 25.f,	 25.f,	0.0f,			0.0f, 1.0f, 0.0f, 1.0f,
				  0.f,	 25.f,	0.0f,			0.0f, 1.0f, 0.0f, 1.0f,
	};

	glCreateVertexArrays(1, &m_GridVA);
	glBindVertexArray(m_GridVA);

	glCreateBuffers(1, &m_GridVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridVB);
	glBufferData(GL_ARRAY_BUFFER, g_GridXSize* g_GridYSize * 7*4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);	//@todo verify that this is correct?

	glEnableVertexArrayAttrib(m_GridVB, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

	glEnableVertexArrayAttrib(m_GridVB, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void*)12);

	glCreateBuffers(1, &m_GridIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GridIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Grid.GetIndices()), m_Grid.GetIndices().data(), GL_STATIC_DRAW);

	return true;
}

TetrisGame::STATUS TetrisGame::run()
{
	if (m_GameWindow == NULL)
	{
		Debug::Message("Tried to run tetris without a valid window");
		return STATUS::EXIT;
	}

	while (!glfwWindowShouldClose(m_GameWindow))
	{
		UpdateGameLogic();

		if (m_StopGame)
			return STATUS::RERUN;

		std::vector<float> verticiesToDraw = m_Grid.GetActiveVerticies();
		
		glBindBuffer(GL_ARRAY_BUFFER, m_GridVB);
		glBufferSubData(GL_ARRAY_BUFFER, 0, verticiesToDraw.size() * sizeof(float), verticiesToDraw.data());

		/*Render*/
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(m_Shader);	//@todo Play with the shader

		glBindVertexArray(m_GridVA);
		glDrawElements(GL_TRIANGLES, (uint32_t)verticiesToDraw.size(), GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(m_GameWindow);
		/* Poll for and process events */
		glfwPollEvents();
	}
	return STATUS::EXIT;
}

void TetrisGame::UpdateGameLogic()
{
	if (m_ActiveShape == nullptr)
	{
		if (m_CurBlock > (int)m_BlockNumbersToSpawn.size()-1)
			return;	//We don't have an active shape, and we have spawned all of the shapes.

		TetrisShape::NUMBER blockNumber = (TetrisShape::NUMBER)m_BlockNumbersToSpawn[m_CurBlock];
		++m_CurBlock;
		
		switch (blockNumber)
		{
			case TetrisShape::NUMBER::LBLOCK:
			{
				m_ActiveShape = new LBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			case TetrisShape::NUMBER::RLBLOCK:
			{
				m_ActiveShape = new ReverseLBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			case TetrisShape::NUMBER::BOXBLOCK:
			{
				m_ActiveShape = new BoxBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			case TetrisShape::NUMBER::LONGBLOCK:
			{
				m_ActiveShape = new LongBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			case TetrisShape::NUMBER::ZBLOCK:
			{
				m_ActiveShape = new ZBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			case TetrisShape::NUMBER::RZBLOCK:
			{
				m_ActiveShape = new ReverseZBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			case TetrisShape::NUMBER::TBLOCK:
			{
				m_ActiveShape = new TBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
				break;
			}
			default:
			{
				Debug::Message("out-of-bounds block number, spawning a box.");
				m_ActiveShape = new BoxBlock(&m_Grid, g_GridXSize / 2, g_GridYSize - 2);
			}
		}

		if (m_ActiveShape->CollisionOnSpawn() == true)
		{
			m_StopGame = true;
			return;
		}
		m_CurrentShapes.push_back(m_ActiveShape);
	}

	/*Update all shapes but the active shape*/
	for (int i = 0; i < (int)m_CurrentShapes.size(); ++i)
	{
		if (m_CurrentShapes[i] == m_ActiveShape)
			continue;
		TetrisShape::STATUS curShapeStatus = m_CurrentShapes[i]->Update();
	}

	m_SlownessTimer.tock();
		
	if (s_UpArrowPressed)
	{
		m_ActiveShape->Rotate();
		s_UpArrowPressed = false;
	}
	if (s_LeftArrowPressed)
	{
		m_ArrowHoldTimer.tick();
		m_ActiveShape->Move(TetrisShape::MOVEDIR::LEFT);
		s_LeftArrowPressed = false;
	}
	if (s_RightArrowPressed)
	{
		m_ArrowHoldTimer.tick();
		m_ActiveShape->Move(TetrisShape::MOVEDIR::RIGHT);
		s_RightArrowPressed = false;
	}

	m_ArrowHoldTimer.tock();
	if (m_ArrowHoldTimer.duration().count() > 100)
	{
		int rightArrowState = glfwGetKey(m_GameWindow, GLFW_KEY_RIGHT);
		if (rightArrowState == GLFW_PRESS)
		{
			m_ActiveShape->Move(TetrisShape::MOVEDIR::RIGHT);
			m_ArrowHoldTimer.tick();
		}
		int leftArrowState = glfwGetKey(m_GameWindow, GLFW_KEY_LEFT);
		if (leftArrowState == GLFW_PRESS)
		{
			m_ActiveShape->Move(TetrisShape::MOVEDIR::LEFT);
			m_ArrowHoldTimer.tick();
		}
	}
		
	if (s_SpacePressed)
	{
		while (true)
		{
			TetrisShape::STATUS status = m_ActiveShape->Update();
			if (status == TetrisShape::STATUS::BLOCKED || status == TetrisShape::STATUS::DISABLED)
			{
				m_ActiveShape = nullptr;
				break;
			}
		}
		s_SpacePressed = false;
		return;
	}

	int slowness = m_Slowness;
	if (s_DownArrowDown)
	{
		slowness = m_Slowness / 4;
	}
	if (m_SlownessTimer.duration().count() < slowness)
		return;
	
	m_SlownessTimer.tick();

	TetrisShape::STATUS activeShapeStatus = m_ActiveShape->Update();

	if (activeShapeStatus == TetrisShape::STATUS::BLOCKED)
	{
		m_BlockTimer.tock();

		if (m_BlockTimer.duration().count() > 2200)	/*If the block has been blocked for 2.2s @todo this should scale slowly with the slowness?*/
			m_ActiveShape = nullptr;
	}
	else
	{
		m_BlockTimer.tick();
	}
	
}