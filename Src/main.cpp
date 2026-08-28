#include"Core/Core.hpp"
#include<iostream>
#include<stdexcept>

int main()
{
	Core core;

	try
	{
		core.Init();

		while (!core.WindowShouldClose())
		{
			glfwPollEvents();
			core.drawFrame();
		}

		core.deviceWaitIdle();

		core.CleanUp();

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_FAILURE;
}