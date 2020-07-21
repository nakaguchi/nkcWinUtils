#include <iostream>
#include "../HPTimer.h"

int main()
{
    nkc::HPTimer t1;
    for (int i = 0; i < 10000; i ++) ;
    std::cout << t1.Interval() << "ms" << std::endl;

    return 0;
}
