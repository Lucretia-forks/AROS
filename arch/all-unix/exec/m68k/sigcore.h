#ifndef _SIGCORE_H
#define _SIGCORE_H

#include <signal.h>
#include "etask.h"

/* Put a value of type SP_TYPE on the stack or get it off the stack. */
#define _PUSH(sp,val)       (*--sp = (SP_TYPE)(val))
#define _POP(sp)            (*sp++)

/*
#define _GETSR_ASM         __asm__ __volatile__          \
                              ("subq.l #4,-4(%a6)  \n\t" \
                               "move.l -4(%a6),%a0 \n\t" \
                               "move.l %a6,(%a0)   \n\t")
*/
			    	
#ifdef __linux__
#   include <asm/sigcontext.h>
#   include <linux/version.h> 
    /* sigcontext_t is the type of the signals' context. Linux offers no way
	to get this context in a legal way, so I have to use tricks. */
/*
#   ifdef i386
#     if LINUX_VERSION_CODE > 131102
        typedef struct sigcontext sigcontext_t;
#     else     
        typedef struct sigcontext_struct sigcontext_t;
#     endif
#   else  
	typedef struct sigcontext sigcontext_t;
#   endif
*/

	typedef struct sigcontext_struct sigcontext_t;
 
    /* name and type of the signal handler */
#   define SIGHANDLER	    linux_sighandler
#   define SIGHANDLER_T     SignalHandler

    /*
	This macro contains some magic necessary to make it work.
	The problem is that Linux offers no official way to obtain the
	signals' context. Linux stores the signals' context on the
	process' stack. It looks like this:

		    |			       |
		    +--------------------------+
		    | last entry before signal |
		    +--------------------------+
		    |	    empty space        | <--- SP
		    +--------------------------+
		    |	   signal context      |
		    +--------------------------+
		    |	   signal number       |
		    +--------------------------+
		    |	   return address      |
		    +--------------------------+
		    |			       |

	so the address of the signal context is &sig+1.
    */
#   define GLOBAL_SIGNAL_INIT \
	static void sighandler (int sig, sigcontext_t * sc);    \
								\
	static void SIGHANDLER (int sig, long sigcode, sigcontext_t * scp)  \
	{							\
	    sighandler (sig, (sigcontext_t *)scp );         \
	}



    /* Type of the values which can be stored on the stack. A variable
	which is to be used as a stack pointer must be declared as
	"SP_TYPE *". */
#   define SP_TYPE	    long

    /* How many general purpose registers are to be saved on the stack
	when a task switch happens */
#   define CPU_NUMREGS	6

    /* Use this structure to save/restore registers if the stack is too
	small */
    struct AROS_cpu_context
    {
	ULONG regs[17]; /* d0-d7,a0-a6,sr,formatvec */
	//struct _fpstate fpu; /* FPU registers */
    };

#   define SIZEOF_ALL_REGISTERS     (sizeof (struct AROS_cpu_context))
#   define GetCpuContext(task)      ((struct AROS_cpu_context *)\
				    (GetIntETask(task)->iet_Context))
#   define GetSP(task)              ((SP_TYPE*)(task->tc_SPReg))


    /*
	Macros to access the stack pointer, frame pointer and program
	counter. The FP is the base address for accesses to arguments
	and local variables of a function and PC is the current address
	in the program code.
    */

#   define SP(sc)           (sc->sc_usp) 
#   define PC(sc)           (sc->sc_pc)

    /*
	Macros to enable or disable all signals after the signal handler
	has returned and the normal execution commences.
    */

#   define SC_DISABLE(sc)   (sc->sc_mask = ~0L)
#   define SC_ENABLE(sc)    (sc->sc_mask = 0L)

    /*
	The names of the general purpose registers which are to be saved.
	Use R and a number as name, no matter what the real name is.
	General purpose registers (GPRs) are registers which can be
	modified by the task (ie. data and address registers) and which are
	not saved by the CPU when an interrupt happens.
    */

/* take a look at /usr/include/asm/sigcore.h and you will find those 
   m68k-registers
*/

#   define R0(sc)       (sc->sc_d0)
#   define R1(sc)       (sc->sc_d1)
#   define R2(sc)       (sc->sc_a0)
#   define R3(sc)       (sc->sc_a1)
#   define R4(sc)	(sc->sc_sr)
#   define R5(sc)	(sc->sc_formatvec)

#   define R6(sc)	(sc->sc_ext_d2)
#   define R7(sc)       (sc->sc_ext_d3)
#   define R8(sc)	(sc->sc_ext_d4)
#   define R9(sc)       (sc->sc_ext_d5)
#   define R10(sc)	(sc->sc_ext_d6)
#   define R11(sc)	(sc->sc_ext_d7)
#   define R12(sc)	(sc->sc_ext_a2)
#   define R13(sc)	(sc->sc_ext_a3)
#   define R14(sc)	(sc->sc_ext_a4)
#   define R15(sc)	(sc->sc_ext_a5)
#   define R16(sc)	(sc->sc_ext_a6)

    /*
	Save and restore the CPU GPRs.
    */
#   define SAVE_CPU(task,sc) \
	(GetCpuContext(task)->regs[0] = R0(sc)), \
	(GetCpuContext(task)->regs[1] = R1(sc)), \
	(GetCpuContext(task)->regs[2] = R2(sc)), \
	(GetCpuContext(task)->regs[3] = R3(sc)), \
	(GetCpuContext(task)->regs[4] = R4(sc)), \
	(GetCpuContext(task)->regs[5] = R5(sc)), \
	(GetCpuContext(task)->regs[6] = R6(sc)), \
	(GetCpuContext(task)->regs[7] = R7(sc)), \
	(GetCpuContext(task)->regs[8] = R8(sc)), \
	(GetCpuContext(task)->regs[9] = R9(sc)), \
	(GetCpuContext(task)->regs[10] = R10(sc)), \
	(GetCpuContext(task)->regs[11] = R11(sc)), \
	(GetCpuContext(task)->regs[12] = R12(sc)), \
	(GetCpuContext(task)->regs[13] = R13(sc)), \
	(GetCpuContext(task)->regs[14] = R14(sc)), \
	(GetCpuContext(task)->regs[15] = R15(sc)), \
	(GetCpuContext(task)->regs[16] = R16(sc))

#   define RESTORE_CPU(task,sc) \
	(R0(sc) = GetCpuContext(task)->regs[0]), \
	(R1(sc) = GetCpuContext(task)->regs[1]), \
	(R2(sc) = GetCpuContext(task)->regs[2]), \
	(R3(sc) = GetCpuContext(task)->regs[3]), \
	(R4(sc) = GetCpuContext(task)->regs[4]), \
	(R5(sc) = GetCpuContext(task)->regs[5]), \
	(R6(sc) = GetCpuContext(task)->regs[6]), \
	(R7(sc) = GetCpuContext(task)->regs[7]), \
	(R8(sc) = GetCpuContext(task)->regs[8]), \
	(R9(sc) = GetCpuContext(task)->regs[9]), \
	(R10(sc) = GetCpuContext(task)->regs[10]), \
	(R11(sc) = GetCpuContext(task)->regs[11]), \
	(R12(sc) = GetCpuContext(task)->regs[12]), \
	(R13(sc) = GetCpuContext(task)->regs[13]), \
	(R14(sc) = GetCpuContext(task)->regs[14]), \
	(R15(sc) = GetCpuContext(task)->regs[15]), \
	(R16(sc) = GetCpuContext(task)->regs[16])	

    /*
	It's not possible to do save the FPU under linux because linux
	uses the tasks stack to save the signal context. The signal
	context conatins the SP *before* the sigcontext was pushed on
	this stack, so it looks like this:

		    |			       |
		    +--------------------------+
		    | last entry before signal |
		    +--------------------------+
		    |	    empty space        | <--- SP
		    +--------------------------+
		    |	   signal context      |
		    +--------------------------+
		    |			       |


	As you can see, SP points to the empty space. Now this empty space
	is not very big. It's big enough that one can save the CPU
	registers but not big enough for the FPU. *sigh*.
    */
#   define NO_FPU

    /*
	Size of the FPU stackframe in stack units (one stack unit is
	sizeof(SP_TYPE) bytes).
    */
#   ifndef NO_FPU
#	define FPU_FRAMESIZE	(sizeof (struct _fpstate) / sizeof (SP_TYPE))
#   else
#	define FPU_FRAMESIZE	0
#   endif

    /*
	This macro return 1 if a FPU is available.
    */
#   ifndef NO_FPU
#	define HAS_FPU(sc)      (sc->fpstate)
#   else
#	define HAS_FPU(sc)      0
#   endif

    /*
	Save and restore the FPU on/from the stack.
    */
#   ifndef NO_FPU
#	define SAVE_FPU(sp,sc) \
	(sp -= FPU_FRAMESIZE), \
	    HAS_FPU(sc) && \
		((*((struct _fpstate *)sp) = *(sc->fpstate)), 1)

#	define RESTORE_FPU(sp,sc) \
	    HAS_FPU(sc) && \
		((*(sc->fpstate) = *((struct _fpstate *)sp)), 1), \
	    (sp += FPU_FRAMESIZE)
#   else
#	define SAVE_FPU(sp,sc)          (sp -= 0)
#	define RESTORE_FPU(sp,sc)       (sp += 0)
#   endif

    /*
	Prepare the stack. This macro is used on the stack before a new
	task is run for the first time. To create such a macro, you must
	know how the system uses the stack. On Linux/i386, every stack
	frame looks like this:

						 high adresses
		    |	       ...	     |
		    +------------------------+
		    |	    arguments	     |
		    +------------------------+
		    |	  return address     |
		    +------------------------+
		    |	old frame pointer    |
		    +------------------------+
		    |	 local variables     |
		    +------------------------+
		    |	 saved registers     |
		    +------------------------+
		    |	       ...	     |
						low addresses
						stack grows from high to
						low addresses.

	The first routine gets no arguments, but if you want to pass
	some to it, then you must push them on the stack before you
	call this macro. Note that the arguments must be pushed in
	reverse order, ie. if you want to call a function like this:

	    func (a,b,c);

	then you must prepare the stack like this:

	    _PUSH(sp,c);
	    _PUSH(sp,b);
	    _PUSH(sp,a);
	    PREPARE_INITIAL_FRAME(sp,func);

	This is because the arguments are fetched relative to the FP
	(ie. FP[0] is the old frame pointer, FP[1] is the return
	address, FP[2] is the first argument, FP[3] is the second
	and so on).

    */

#   define PREPARE_INITIAL_FRAME(sp,pc) \
	( \
	_PUSH(sp,pc) \
	)

    /*
	Prepare the cpu context
    */
#   define PREPARE_INITIAL_CONTEXT(task)    /* nop */


    /*
	This macro is similar to PREPARE_INITIAL_FRAME() but also saves
	all general purpose registers. Use this macro when you want to
	leave the current tasks' context to save the registers. Note that
	the argument "sp" of the macro is just the name of the stack
	pointer. The macro will load it from the sigcontext "sc". You
	must store the value of "sp" after the macro and hand it to
	RESTOREREGS() below to restore this context.
    */
    

#   define SAVEREGS(task,sc) \
	( \
	GetSP(task) = (SP_TYPE *)SP(sc), \
	_PUSH(GetSP(task),PC(sc)) ,\
	SAVE_CPU(task,sc) \
	)

    /*
	This macro does the opposite to SAVEREGS(). It restores all
	general purpose registers. After that, you can enter the new
	tasks' context. Both "sp" and "sc" must be initialized.
	The macro will save the new SP into the sigcontext "sc".
    */

#   define RESTOREREGS(task,sc) \
	( \
	RESTORE_CPU(task,sc), \
	PC(sc) = _POP(GetSP(task)), \
	SP(sc) = GetSP(task) \
	)


    /* This macro prints the current signals' context */
#   define PRINT_SC(sc) \
	printf ("SC: SP=%08lx  PC=%08lx  FPU=%s\n" \
		"    R0=%08lx  R1=%08lx  R2=%08lx  R3=%08lx\n" \
		"    R4=%08lx  R5=%08lx \n" \
	    , SP(sc), PC(sc) \
	    , "no" \
	    , R0(sc), R1(sc), R2(sc), R3(sc) \
	    , (long)R4(sc), (long)R5(sc) \
	)

    /* This macro prints the current stack (after SAVEREGS()) */
#   define PRINT_STACK(sp) \
	printf ("    SP=%08lx  PC=%08lx\n" \
		"    R0=%08lx  R1=%08lx  R2=%08lx  R3=%08lx\n" \
		"    R4=%08lx  R5=%08lx\n" \
	    , (ULONG)(sp+(FPU_FRAMESIZE+CPU_NUMREGS+2)) \
	    , sp[FPU_FRAMESIZE+CPU_NUMREGS] \
	    , sp[FPU_FRAMESIZE+CPU_NUMREGS+1] \
	    , sp[5], sp[4], sp[3], sp[2] \
	    , sp[1], sp[0] \
	)

#endif /* __linux__ */

#endif /* _SIGCORE_H */
