#pragma once
#define GLEW_STATIC
#include "GL/glew.h"
#include "../debug.h"
#include "GLFW/glfw3.h"

#include <chrono>

#include "TetrisGrid.h"

constexpr int g_WindowX = 250;
constexpr int g_WindowY = 500;

/* https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time */
template <class DT = std::chrono::milliseconds,
	class ClockT = std::chrono::steady_clock>
	class Timer
{
	using timep_t = typename ClockT::time_point;
	timep_t _start = ClockT::now(), _end = {};

public:
	void tick() {
		_end = timep_t{};
		_start = ClockT::now();
	}

	void tock() { _end = ClockT::now(); }

	template <class T = DT>
	auto duration() const {
		return std::chrono::duration_cast<T>(_end - _start);
	}
};

class TetrisGame
{
public:
	enum class STATUS
	{
		EXIT,
		RERUN
	};

	TetrisGame() :
		m_StopGame(false),

		m_ActiveShape(nullptr),
		m_CurrentShapes({}),
		m_CurBlock(0),
		m_SlownessTimer({}),
		m_BlockTimer({}),
		m_ArrowHoldTimer({}),

		m_Grid(g_WindowX, g_WindowY, 1.0f, 0.0f, 0.0f),
		m_Slowness(100),

		m_Shader(0),
		m_GridVA(0),
		m_GridVB(0),
		m_GridIB(0),

		m_WindowX(250), 
		m_WindowY(500), 

		m_GameWindow(nullptr)
	{};
	~TetrisGame();

	bool initGame();
	STATUS run();

private:
	void UpdateGameLogic();	/*Handles block creation, deletion and updates*/

	bool m_StopGame;

	TetrisShape* m_ActiveShape;
	std::vector<TetrisShape*> m_CurrentShapes;
	int m_CurBlock;
	Timer<std::chrono::milliseconds, std::chrono::steady_clock> m_SlownessTimer;
	Timer<std::chrono::milliseconds, std::chrono::steady_clock> m_BlockTimer;
	Timer<std::chrono::milliseconds, std::chrono::steady_clock> m_ArrowHoldTimer;

	TetrisGrid m_Grid;
	std::vector<int> m_BlockNumbersToSpawn;
	int m_Slowness;

	unsigned int m_Shader;
	unsigned int m_GridVA;
	unsigned int m_GridVB;
	unsigned int m_GridIB;

	int m_WindowX;
	int m_WindowY;

	GLFWwindow* m_GameWindow;
};
