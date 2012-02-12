	.file	"forloop.c"
	.text
	.p2align 4,,15
	.globl	ResultInit
	.type	ResultInit, @function
ResultInit:
.LFB11:
	.cfi_startproc
	movl	4(%esp), %eax
	movl	$0, (%eax)
	movl	$2147483647, 4(%eax)
	movl	$-2147483648, 8(%eax)
	ret
	.cfi_endproc
.LFE11:
	.size	ResultInit, .-ResultInit
	.p2align 4,,15
	.globl	for_classic
	.type	for_classic, @function
for_classic:
.LFB12:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	movl	28(%esp), %eax
	movl	20(%esp), %ebx
	testl	%eax, %eax
	movl	$0, (%ebx)
	movl	$2147483647, 4(%ebx)
	movl	$-2147483648, 8(%ebx)
	je	.L2
	movl	24(%esp), %ecx
	movl	$-2147483648, %ebp
	movl	$2147483647, %edi
	xorl	%esi, %esi
	xorl	%edx, %edx
	jmp	.L7
	.p2align 4,,7
	.p2align 3
.L9:
	movl	4(%ebx), %edi
.L7:
	movl	28(%ecx), %eax
	addl	%eax, %esi
	cmpl	%edi, %eax
	jge	.L4
	movl	%eax, 4(%ebx)
.L4:
	cmpl	%ebp, %eax
	jle	.L5
	movl	%eax, 8(%ebx)
	movl	%eax, %ebp
.L5:
	addl	$1, %edx
	addl	$32, %ecx
	cmpl	28(%esp), %edx
	jne	.L9
	movl	%esi, (%ebx)
.L2:
	popl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popl	%esi
	.cfi_def_cfa_offset 12
	.cfi_restore 6
	popl	%edi
	.cfi_def_cfa_offset 8
	.cfi_restore 7
	popl	%ebp
	.cfi_def_cfa_offset 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE12:
	.size	for_classic, .-for_classic
	.p2align 4,,15
	.globl	for_optimized
	.type	for_optimized, @function
for_optimized:
.LFB13:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	movl	28(%esp), %edx
	movl	20(%esp), %ebx
	movl	24(%esp), %ecx
	testl	%edx, %edx
	movl	$0, (%ebx)
	movl	$2147483647, 4(%ebx)
	movl	$-2147483648, 8(%ebx)
	je	.L10
	movl	$-2147483648, %ebp
	movl	$2147483647, %edi
	xorl	%esi, %esi
	jmp	.L15
	.p2align 4,,7
	.p2align 3
.L16:
	movl	4(%ebx), %edi
	addl	$32, %ecx
.L15:
	movl	28(%ecx), %eax
	addl	%eax, %esi
	cmpl	%edi, %eax
	jge	.L12
	movl	%eax, 4(%ebx)
.L12:
	cmpl	%ebp, %eax
	jle	.L13
	movl	%eax, 8(%ebx)
	movl	%eax, %ebp
.L13:
	subl	$1, %edx
	jne	.L16
	movl	%esi, (%ebx)
.L10:
	popl	%ebx
	.cfi_def_cfa_offset 16
	.cfi_restore 3
	popl	%esi
	.cfi_def_cfa_offset 12
	.cfi_restore 6
	popl	%edi
	.cfi_def_cfa_offset 8
	.cfi_restore 7
	popl	%ebp
	.cfi_def_cfa_offset 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE13:
	.size	for_optimized, .-for_optimized
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 4
.LC0:
	.string	"for %u trials, classicTime == %lu, optimizedTime == %lu\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%esi
	pushl	%ebx
	andl	$-16, %esp
	subl	$16, %esp
	movl	$0, (%esp)
	.cfi_offset 3, -16
	.cfi_offset 6, -12
	call	time
	movl	$0, (%esp)
	movl	%eax, %esi
	call	time
	movl	$0, (%esp)
	movl	%eax, %ebx
	call	time
	subl	%esi, %ebx
	movl	$0, (%esp)
	movl	%eax, %esi
	call	time
	movl	%ebx, 8(%esp)
	movl	$100000000, 4(%esp)
	movl	$.LC0, (%esp)
	subl	%esi, %eax
	movl	%eax, 12(%esp)
	call	printf
	leal	-8(%ebp), %esp
	xorl	%eax, %eax
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%ebp
	.cfi_def_cfa 4, 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE14:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.6.2 20111027 (Red Hat 4.6.2-1)"
	.section	.note.GNU-stack,"",@progbits
