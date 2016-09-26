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

    int *n = Singleton<int>::Instance();
    *n = 3;
    int *n1 = Singleton<int>::Instance();
    fprintf(stderr, "n:%d; n1:%d\n", *n, *n1);

    Singleton<std::string>::Instance()->assign("This is right ~_~ * ~_~ ");
    std::string *str = Singleton<std::string>::Instance();
    fprintf(stderr, "str:%s\n", str->c_str());

    return 0;
}
#endif
