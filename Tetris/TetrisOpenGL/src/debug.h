#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>

#ifdef _DEBUG
	constexpr bool IS_DEBUG_ENABLED = 1;
#else 
	constexpr bool IS_DEBUG_ENABLED = 0;
#endif

namespace Debug
{
	void Message();

	template<typename First, typename ...Rest>
	void Message(First&& first, Rest && ...rest)
	{
		#ifdef _DEBUG
			std::cout << std::forward<First>(first);
			Message(std::forward<Rest>(rest)...);
		#endif
	}

	void InitConsole();
	void StopAndFreeConsole();
};


	


