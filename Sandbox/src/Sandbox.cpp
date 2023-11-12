#include "Profiler.h"
#include "TestFunctions.h"

#include <Windows.h>

void ProcessInput()
{
    SAMPLE();
    Fibonacci(12);
}

void Update()
{
    SAMPLE();
    Fibonacci(18);
}

void Render()
{
    SAMPLE();
    Fibonacci(15);
}

void GameLoop()
{
    SAMPLE();

    ProcessInput();
    Update();
    Render();
}

int main()
{
    for (int i = 0; i < 5; i++)
    {
        BEGIN_PROFILE_FRAME();
        GameLoop();
        END_PROFILE_FRAME();
    }

    PROFILE_OUTPUT();
    return 0;
}
