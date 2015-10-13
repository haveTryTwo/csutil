#include "singleton.h"

namespace strategy
{
}

#ifdef _SINGLETON_MAIN_TEST_
#include <string>

#include <stdio.h>

int main(int argc, char *argv[])
{
    using namespace strategy;

    int *n = Singleton<int>::instance();
    *n = 3;
    int *n1 = Singleton<int>::instance();
    fprintf(stderr, "n:%d; n1:%d\n", *n, *n1);

    Singleton<std::string>::instance()->assign("This is right ~_~ * ~_~ ");
    std::string *str = Singleton<std::string>::instance();
    fprintf(stderr, "str:%s\n", str->c_str());

    return 0;
}
#endif
