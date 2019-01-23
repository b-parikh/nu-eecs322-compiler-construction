   .text
   .globl go

go:
   # save callee-saved registers
   pushq %rbx
   pushq %rbp
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15

   call _go

   # restore callee-saved registers and return
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %rbp
   popq %rbx
   retq


_go:
   movq $5, %rdi
   movq $3, %rsi
   call allocate  # runtime system call
   movq %rax, %rdi
   call print  # runtime system call
   ret

