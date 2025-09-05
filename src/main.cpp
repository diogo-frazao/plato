#include <iostream>
#include "log.h"

int main()
{
    D_LOG(MINI, "Test");
    D_LOG(LOG, "Test");
    D_LOG(WARNING, "Test");
    D_LOG(ERROR, "Test");
    return 0;
}