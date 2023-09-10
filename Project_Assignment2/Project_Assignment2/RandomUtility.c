#include "RandomUtility.h"
#include <stdlib.h>

/// <summary>
/// random int generator that returns postive or negative value
/// </summary>
/// <param name="n"></param>
/// <returns>int value</returns>
int getRandomRange(int n) {
	return ((rand() % n) * ((rand() % 2 == 0) ? -1 : 1));
}
/// <summary>
/// random float generator that returns postive or negative value
/// </summary>
/// <param name="n"></param>
/// <returns>float value</returns>
float getRandomFloat(float n) {
	return (((float)rand() / (float)(RAND_MAX)) * n) * ((rand() % 2 == 0) ? -1 : 1);
}
/// <summary>
/// random float generator that returns range between min and max.
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <returns>float value</returns>
float getRandomRangeFloat(float min, float max) {
	return (((max - min) * ((float)rand() / RAND_MAX)) + min);
}
/// <summary>
///  random int only returns with postiive value.
/// </summary>
/// <param name="n"></param>
/// <returns>positive integer value</returns>
int getPositiveRandom(int n) {
	return ((rand() % n) + 1);
}