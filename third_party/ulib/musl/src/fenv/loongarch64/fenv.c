#include <fenv.h>

#include <zircon/compiler.h>
#include <stdint.h>

int fegetround(void) {
    return 0;
}

__LOCAL int __fesetround(int round) {
    return 0;
}

int fetestexcept(int mask) {
    return 0;
}


int feclearexcept(int mask) {
    return 0;
}

int feraiseexcept(int mask) {
    return 0;
}

int fegetenv(fenv_t* env) {
    return 0;
}

int fesetenv(const fenv_t* env) {
    return 0;
}