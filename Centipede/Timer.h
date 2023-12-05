#pragma once
#include <chrono>
#include <iostream>
#include <fstream>

class timer
{
public:

	timer()
	{

	}
	void start()
	{
		startS = std::chrono::high_resolution_clock::now();
	}
	void stop()
	{
		end = std::chrono::high_resolution_clock::now();
	}
	float get_elapsed_s()
	{
		std::chrono::duration<float> duration = end - startS;
		return duration.count();
	}
    void checklimit()
	{
		bool limitReached = false;
		int limit = 5;
		int elapsed = get_elapsed_s();
		
		if (elapsed>=limit)
		{
			limitReached = true;
		}
		
	}
	std::chrono::time_point<std::chrono::high_resolution_clock>startS, end;
};