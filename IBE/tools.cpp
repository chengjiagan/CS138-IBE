#include "tools.h"
#include <ctime>
#include <random>

using namespace std;

default_random_engine e((unsigned)time(NULL));

void randomArray(int array[], int size, int min, int max)
{
    uniform_int_distribution<int> u(min, max);
    for (int i = 0; i < size; i++) {
        array[i] = u(e);
    }
}

bool randomBoolean(double posibility)
{
    bernoulli_distribution b(posibility);
    return b(e);
}

void randomString(char string[], int size)
{
    uniform_int_distribution<int> u('a', 'z');
    for (int i = 0; i < size; i++) {
        string[i] = u(e);
    }
    string[size] = '\0';
}