#include <fenv.h>

#include <zircon/assert.h>
#include <zircon/compiler.h>
#include <stdint.h>

int fegetround(void) {
    TODO();
    return 0;
}

__LOCAL int __fesetround(int round) {
    TODO();
    return 0;
}

int fetestexcept(int mask) {
    TODO();
    return 0;
}

int feclearexcept(int mask) {
    TODO();
    return 0;
}

int feraiseexcept(int mask) {
    TODO();
    return 0;
}

int fegetenv(fenv_t* env) {
    TODO();
    return 0;
}

int fesetenv(const fenv_t* env) {
    TODO();
    return 0;
}