#include "TestFunctions.h"
#include "Profiler.h"
#include <iostream>

void Recursive(int n)
{
    std::cout << n << std::endl;

    if (n <= 0)
    {
        return;
    }

    Recursive(n - 1);
}

int Fibonacci(int n)
{
    if (n <= 1)
    {
        return n;
    }

    return Fibonacci(n - 1) + Fibonacci(n - 2);
}

int Simple(int n)
{
    int counter = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            counter++;
        }
    }
    return counter;
}

bool IsPrime(int number)
{
    if (number <= 1)
    {
        return false;
    }

    for (int i = 2; i * i <= number; ++i)
    {
        if (number % i == 0)
        {
            return false;
        }
    }
    return true;
}

int FindNthPrime(int n)
{
    if (n <= 0)
    {
        return -1;
    }

    int count = 0;
    int number = 2;

    while (count < n)
    {
        if (IsPrime(number))
        {
            ++count;
        }
        if (count < n) {
            ++number;
        }
    }

    return number;
}

void Parent()
{
    for (int i = 0; i < 1000; i++)
    {
        Level1();
    }
}

void Level1()
{
    for (int i = 0; i < 1000; i++)
    {
        Level2();
    }
}

void Level2()
{
    for (int i = 0; i < 1000; i++)
    {
    }
}
