.text
	.globl go
go:
	pushq %rbx
	pushq %rbp
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	call _go
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbp
	popq %rbx
	retq

_encode: 
subq $0, %rsp
salq $1, %rdi
addq $1, %rdi
movq %rdi, %rax
addq $0, %rsp
retq
_go: 
subq $0, %rsp
movq $7, %rdi
movq $5, %rsi
call allocate
movq 0(%rax), %rdi
movq $_encode_ret, -8(%rsp)
subq $8, %rsp
jmp _encode
_encode_ret:
movq %rax, %rdi
call print
addq $0, %rsp
retq
