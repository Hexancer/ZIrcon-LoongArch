enum
{
  FE_INEXACT =
#define FE_INEXACT 0x010000
    FE_INEXACT,
  FE_UNDERFLOW =
#define FE_UNDERFLOW 0x020000
    FE_UNDERFLOW,
  FE_OVERFLOW =
#define FE_OVERFLOW 0x040000
    FE_OVERFLOW,
  FE_DIVBYZERO =
#define FE_DIVBYZERO 0x080000
    FE_DIVBYZERO,
  FE_INVALID =
#define FE_INVALID 0x100000
    FE_INVALID,
};

#define FE_ALL_EXCEPT \
  (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

enum
{
  FE_TONEAREST =
#define FE_TONEAREST 0x000
    FE_TONEAREST,
  FE_TOWARDZERO =
#define FE_TOWARDZERO 0x100
    FE_TOWARDZERO,
  FE_UPWARD =
#define FE_UPWARD 0x200
    FE_UPWARD,
  FE_DOWNWARD =
#define FE_DOWNWARD 0x300
    FE_DOWNWARD
};


// #define FE_INVALID 1
// #define FE_DIVBYZERO 2
// #define FE_OVERFLOW 4
// #define FE_UNDERFLOW 8
// #define FE_INEXACT 16
// #define FE_ALL_EXCEPT 31
// #define FE_TONEAREST 0
// #define FE_DOWNWARD 0x800000
// #define FE_UPWARD 0x400000
// #define FE_TOWARDZERO 0xc00000

typedef unsigned int fexcept_t;

typedef struct {
    unsigned int __fcsr;
} fenv_t;

// #define FE_DFL_ENV ((const fenv_t*)-1)
