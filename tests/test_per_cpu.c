#include <stdio.h>
#include <ucsan/per_cpu.h>

int main(void)
{
    int a, b;
    test_per_cpu();

    return 0;
}
