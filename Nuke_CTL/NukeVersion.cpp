#include <iostream>
#include "DDImage/ddImageVersionNumbers.h"
int main(int, char*[])
{
    std::cout << kDDImageVersionMajorNum << "." << kDDImageVersionMinorNum;
    return 0;
}