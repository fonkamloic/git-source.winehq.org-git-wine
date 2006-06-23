/*
 * i386 register context support
 *
 * Copyright (C) 1999 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "config.h"

#ifdef __i386__

#include <assert.h>
#include <errno.h>
#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif
#include <stdarg.h>
#include <unistd.h>
#ifdef HAVE_SYS_PTRACE_H
# include <sys/ptrace.h>
#endif
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#include "windef.h"

#include "file.h"
#include "thread.h"
#include "request.h"

#ifndef PTRACE_PEEKUSER
#  ifdef PTRACE_PEEKUSR /* libc5 uses USR not USER */
#    define PTRACE_PEEKUSER PTRACE_PEEKUSR
#  else
#    define PTRACE_PEEKUSER PT_READ_U
#  endif
#endif

#ifndef PTRACE_POKEUSER
#  ifdef PTRACE_POKEUSR /* libc5 uses USR not USER */
#    define PTRACE_POKEUSER PTRACE_POKEUSR
#  else
#    define PTRACE_POKEUSER PT_WRITE_U
#  endif
#endif

#ifdef PT_GETDBREGS
#define PTRACE_GETDBREGS PT_GETDBREGS
#endif

#ifdef PT_SETDBREGS
#define PTRACE_SETDBREGS PT_SETDBREGS
#endif

#ifdef linux
#ifdef HAVE_SYS_USER_H
# include <sys/user.h>
#endif

/* debug register offset in struct user */
#define DR_OFFSET(dr) ((int)((((struct user *)0)->u_debugreg) + (dr)))

/* retrieve a debug register */
static inline int get_debug_reg( int pid, int num, DWORD *data )
{
    int res;
    errno = 0;
    res = ptrace( PTRACE_PEEKUSER, pid, DR_OFFSET(num), 0 );
    if ((res == -1) && errno)
    {
        file_set_error();
        return -1;
    }
    *data = res;
    return 0;
}

/* retrieve the thread x86 registers */
void get_thread_context( struct thread *thread, CONTEXT *context, unsigned int flags )
{
    int pid = get_ptrace_pid(thread);

    /* all other regs are handled on the client side */
    assert( (flags | CONTEXT_i386) == CONTEXT_DEBUG_REGISTERS );

    if (!suspend_for_ptrace( thread )) return;

    if (get_debug_reg( pid, 0, &context->Dr0 ) == -1) goto error;
    if (get_debug_reg( pid, 1, &context->Dr1 ) == -1) goto error;
    if (get_debug_reg( pid, 2, &context->Dr2 ) == -1) goto error;
    if (get_debug_reg( pid, 3, &context->Dr3 ) == -1) goto error;
    if (get_debug_reg( pid, 6, &context->Dr6 ) == -1) goto error;
    if (get_debug_reg( pid, 7, &context->Dr7 ) == -1) goto error;
    context->ContextFlags |= CONTEXT_DEBUG_REGISTERS;
    resume_after_ptrace( thread );
    return;
 error:
    file_set_error();
    resume_after_ptrace( thread );
}

/* set the thread x86 registers */
void set_thread_context( struct thread *thread, const CONTEXT *context, unsigned int flags )
{
    int pid = get_ptrace_pid( thread );

    /* all other regs are handled on the client side */
    assert( (flags | CONTEXT_i386) == CONTEXT_DEBUG_REGISTERS );

    if (!suspend_for_ptrace( thread )) return;

    if (ptrace( PTRACE_POKEUSER, pid, DR_OFFSET(0), context->Dr0 ) == -1) goto error;
    if (thread->context) thread->context->Dr0 = context->Dr0;
    if (ptrace( PTRACE_POKEUSER, pid, DR_OFFSET(1), context->Dr1 ) == -1) goto error;
    if (thread->context) thread->context->Dr1 = context->Dr1;
    if (ptrace( PTRACE_POKEUSER, pid, DR_OFFSET(2), context->Dr2 ) == -1) goto error;
    if (thread->context) thread->context->Dr2 = context->Dr2;
    if (ptrace( PTRACE_POKEUSER, pid, DR_OFFSET(3), context->Dr3 ) == -1) goto error;
    if (thread->context) thread->context->Dr3 = context->Dr3;
    if (ptrace( PTRACE_POKEUSER, pid, DR_OFFSET(6), context->Dr6 ) == -1) goto error;
    if (thread->context) thread->context->Dr6 = context->Dr6;
    if (ptrace( PTRACE_POKEUSER, pid, DR_OFFSET(7), context->Dr7 ) == -1) goto error;
    if (thread->context) thread->context->Dr7 = context->Dr7;
    resume_after_ptrace( thread );
    return;
 error:
    file_set_error();
    resume_after_ptrace( thread );
}

#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <machine/reg.h>

/* retrieve the thread x86 registers */
void get_thread_context( struct thread *thread, CONTEXT *context, unsigned int flags )
{
#ifdef PTRACE_GETDBREGS
    int pid = get_ptrace_pid(thread);
    struct dbreg dbregs;

    /* all other regs are handled on the client side */
    assert( (flags | CONTEXT_i386) == CONTEXT_DEBUG_REGISTERS );

    if (!suspend_for_ptrace( thread )) return;

    if (ptrace( PTRACE_GETDBREGS, pid, (caddr_t) &dbregs, 0 ) == -1) file_set_error();
    else
    {
#ifdef DBREG_DRX
        /* needed for FreeBSD, the structure fields have changed under 5.x */
        context->Dr0 = DBREG_DRX((&dbregs), 0);
        context->Dr1 = DBREG_DRX((&dbregs), 1);
        context->Dr2 = DBREG_DRX((&dbregs), 2);
        context->Dr3 = DBREG_DRX((&dbregs), 3);
        context->Dr6 = DBREG_DRX((&dbregs), 6);
        context->Dr7 = DBREG_DRX((&dbregs), 7);
#else
        context->Dr0 = dbregs.dr0;
        context->Dr1 = dbregs.dr1;
        context->Dr2 = dbregs.dr2;
        context->Dr3 = dbregs.dr3;
        context->Dr6 = dbregs.dr6;
        context->Dr7 = dbregs.dr7;
#endif
        context->ContextFlags |= CONTEXT_DEBUG_REGISTERS;
    }
    resume_after_ptrace( thread );
#endif
}

/* set the thread x86 registers */
void set_thread_context( struct thread *thread, const CONTEXT *context, unsigned int flags )
{
#ifdef PTRACE_SETDBREGS
    int pid = get_ptrace_pid(thread);
    struct dbreg dbregs;

    /* all other regs are handled on the client side */
    assert( (flags | CONTEXT_i386) == CONTEXT_DEBUG_REGISTERS );

    if (!suspend_for_ptrace( thread )) return;

#ifdef DBREG_DRX
    /* needed for FreeBSD, the structure fields have changed under 5.x */
    DBREG_DRX((&dbregs), 0) = context->Dr0;
    DBREG_DRX((&dbregs), 1) = context->Dr1;
    DBREG_DRX((&dbregs), 2) = context->Dr2;
    DBREG_DRX((&dbregs), 3) = context->Dr3;
    DBREG_DRX((&dbregs), 4) = 0;
    DBREG_DRX((&dbregs), 5) = 0;
    DBREG_DRX((&dbregs), 6) = context->Dr6;
    DBREG_DRX((&dbregs), 7) = context->Dr7;
#else
    dbregs.dr0 = context->Dr0;
    dbregs.dr1 = context->Dr1;
    dbregs.dr2 = context->Dr2;
    dbregs.dr3 = context->Dr3;
    dbregs.dr4 = 0;
    dbregs.dr5 = 0;
    dbregs.dr6 = context->Dr6;
    dbregs.dr7 = context->Dr7;
#endif
    if (ptrace( PTRACE_SETDBREGS, pid, (caddr_t) &dbregs, 0 ) == -1) file_set_error();
    else if (thread->context)  /* update the cached values */
    {
        thread->context->Dr0 = context->Dr0;
        thread->context->Dr1 = context->Dr1;
        thread->context->Dr2 = context->Dr2;
        thread->context->Dr3 = context->Dr3;
        thread->context->Dr6 = context->Dr6;
        thread->context->Dr7 = context->Dr7;
    }
    resume_after_ptrace( thread );
#endif
}

#else  /* linux || __FreeBSD__ */

/* retrieve the thread x86 registers */
void get_thread_context( struct thread *thread, CONTEXT *context, unsigned int flags )
{
}

/* set the thread x86 debug registers */
void set_thread_context( struct thread *thread, const CONTEXT *context, unsigned int flags )
{
}

#endif  /* linux || __FreeBSD__ */


/* copy a context structure according to the flags */
void copy_context( CONTEXT *to, const CONTEXT *from, unsigned int flags )
{
    flags &= ~CONTEXT_i386;  /* get rid of CPU id */
    if (flags & CONTEXT_CONTROL)
    {
        to->Ebp    = from->Ebp;
        to->Eip    = from->Eip;
        to->Esp    = from->Esp;
        to->SegCs  = from->SegCs;
        to->SegSs  = from->SegSs;
        to->EFlags = from->EFlags;
    }
    if (flags & CONTEXT_INTEGER)
    {
        to->Eax = from->Eax;
        to->Ebx = from->Ebx;
        to->Ecx = from->Ecx;
        to->Edx = from->Edx;
        to->Esi = from->Esi;
        to->Edi = from->Edi;
    }
    if (flags & CONTEXT_SEGMENTS)
    {
        to->SegDs = from->SegDs;
        to->SegEs = from->SegEs;
        to->SegFs = from->SegFs;
        to->SegGs = from->SegGs;
    }
    if (flags & CONTEXT_FLOATING_POINT)
    {
        to->FloatSave = from->FloatSave;
    }
    if (flags & CONTEXT_DEBUG_REGISTERS)
    {
        to->Dr0 = from->Dr0;
        to->Dr1 = from->Dr1;
        to->Dr2 = from->Dr2;
        to->Dr3 = from->Dr3;
        to->Dr6 = from->Dr6;
        to->Dr7 = from->Dr7;
    }
    to->ContextFlags |= flags;
}

/* retrieve the current instruction pointer of a context */
void *get_context_ip( const CONTEXT *context )
{
    return (void *)context->Eip;
}

/* return the context flag that contains the CPU id */
unsigned int get_context_cpu_flag(void)
{
    return CONTEXT_i386;
}

/* return only the context flags that correspond to system regs */
/* (system regs are the ones we can't access on the client side) */
unsigned int get_context_system_regs( unsigned int flags )
{
    return flags & (CONTEXT_DEBUG_REGISTERS & ~CONTEXT_i386);
}

#endif  /* __i386__ */
