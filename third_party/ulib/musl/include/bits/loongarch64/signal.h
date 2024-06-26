//TODO: complete contexts

#include <stdint.h>

#define FPU_REG_WIDTH	256
#define FPU_ALIGN	__attribute__((__aligned__(32)))

union fpureg {
	uint32_t	val32[FPU_REG_WIDTH / 32];
	uint64_t	val64[FPU_REG_WIDTH / 64];
};

typedef struct sigaltstack {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
} stack_t;


struct sigcontext {
	uint64_t	pc;
	uint64_t	regs[32];
	uint32_t	flags;

	uint32_t	fcsr;
	uint32_t	vcsr;
	uint64_t	fcc;
	uint64_t	scr[4];

	union fpureg sc_fpregs[32] FPU_ALIGN;
	uint8_t	sc_reserved[4096] __attribute__((__aligned__(16)));
};

/**
 * struct ucontext - user context structure
 * @uc_flags:
 * @uc_link:
 * @uc_stack:
 * @uc_mcontext:	holds basic processor state
 * @uc_sigmask:
 * @uc_extcontext:	holds extended processor state
 */
typedef struct __ucontext {
	/* Historic fields matching asm-generic */
	unsigned long		uc_flags;
	struct ucontext		*uc_link;
	stack_t			uc_stack;
	struct sigcontext	uc_mcontext;
	sigset_t		uc_sigmask;

	/* Extended context structures may follow ucontext */
	unsigned long long	uc_extcontext[0];
} ucontext_t;

#define SA_NOCLDSTOP	0x00000001
#define SA_NOCLDWAIT	0x00000002
#define SA_SIGINFO	0x00000004
#define SA_ONSTACK	0x08000000
#define SA_RESTART	0x10000000
#define SA_NODEFER	0x40000000
#define SA_RESETHAND	0x80000000

#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

#define _NSIG 65
