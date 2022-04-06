#pragma once

/* Assembly instruction format:          rd, csr_num */
/* Data types in instruction templates:  unsigned int, uimm14_32 */
#define __csrrd(/*uimm14_32*/ _1)	((unsigned int)__builtin_loongarch_csrrd(_1))

/* Assembly instruction format:          rd, csr_num */
/* Data types in instruction templates:  unsigned int, uimm14_32 */
#define __csrwr(/*unsigned int*/ _1, /*uimm14_32*/ _2)	((unsigned int)__builtin_loongarch_csrwr((unsigned int)(_1), (_2)))

/* Assembly instruction format:          rd, rj, csr_num */
/* Data types in instruction templates:  unsigned int, unsigned int, uimm14_32 */
#define __csrxchg(/*unsigned int*/ _1, /*unsigned int*/ _2, /*uimm14_32*/ _3) ((unsigned int)__builtin_loongarch_csrxchg((unsigned int)(_1), (unsigned int)(_2), (_3)))

/* Assembly instruction format:          rd, csr_num */
/* Data types in instruction templates:  unsigned long int, uimm14 */
#define __dcsrrd(/*uimm14*/ _1)	((unsigned long int)__builtin_loongarch_dcsrrd(_1))

/* Assembly instruction format:          rd, csr_num */
/* Data types in instruction templates:  unsigned long int, uimm14 */
#define __dcsrwr(/*unsigned long int*/ _1, /*uimm14*/ _2)	((unsigned long int)__builtin_loongarch_dcsrwr((unsigned long int)(_1), (_2)))

/* Assembly instruction format:          rd, rj, csr_num */
/* Data types in instruction templates:  unsigned long int, unsigned long int, uimm14 */
#define __dcsrxchg(/*unsigned long int*/ _1, /*unsigned long int*/ _2, /*uimm14*/ _3) ((unsigned long int)__builtin_loongarch_dcsrxchg((unsigned long int)(_1), (unsigned long int)(_2), (_3)))
