#pragma once
#include <thread>
#include <functional>
#include <vector>

namespace std
{
	class thread;
}

void doParallel(std::function<void(int)> toDo, int n)
{
	int numberOfThreads = 4;
	std::vector<std::thread> threads;
	for (int i = 0; i < n; i++)
	{
		int threadIndex = (i - numberOfThreads) % numberOfThreads;
		auto fx = [i, &toDo] { toDo(i); };
		if (threads.size() > threadIndex)
		{
			if (threads[threadIndex].joinable())
			{
				threads[threadIndex].join();
			}
			threads[threadIndex] = std::thread(fx);
		}
		else
		{
			threads.emplace_back(fx);
		}
	}
	for (int i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
		{
			threads[i].join();
		}
	}
}
