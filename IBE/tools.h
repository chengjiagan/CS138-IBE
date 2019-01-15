/* tools set
*  some helper function for reusing
*  function:
*    randomArray: generate a random array with giving size and upper and lower bound
*  by Jiagan Cheng
*/

#ifndef _TOOLS_H
#define _TOOLS_H

// generate a random array, each element in [min, max]
void randomArray(int array[], int size, int min, int max);
bool randomBoolean(double posibility = 0.5);
void randomString(char string[], int size);

#endif