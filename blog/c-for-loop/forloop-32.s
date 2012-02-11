	.file	"forloop.c"
	.text
.globl _ResultInit
	.def	_ResultInit;	.scl	2;	.type	32;	.endef
_ResultInit:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	$0, (%eax)
	movl	8(%ebp), %eax
	movl	$2147483647, 4(%eax)
	movl	8(%ebp), %eax
	movl	$-2147483648, 8(%eax)
	popl	%ebp
	ret
.globl _for_classic
	.def	_for_classic;	.scl	2;	.type	32;	.endef
_for_classic:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ResultInit
	movl	$0, -8(%ebp)	/* i = 0; */
	
L3:
	movl	-8(%ebp), %eax	/* copy i to %eax */
	cmpl	16(%ebp), %eax	/* compare n (in 16(%ebp)) to i (in %eax) */
	jae	L2		/* if (i >= n) goto L2; */
	movl	8(%ebp), %ecx
	movl	8(%ebp), %ebx
	movl	-8(%ebp), %eax
	movl	%eax, %edx
	sall	$5, %edx
	movl	12(%ebp), %eax
	movl	28(%edx,%eax), %eax
	addl	(%ebx), %eax
	movl	%eax, (%ecx)
	movl	-8(%ebp), %eax
	movl	%eax, %ecx
	sall	$5, %ecx
	movl	12(%ebp), %eax
	movl	8(%ebp), %edx
	movl	28(%ecx,%eax), %eax
	cmpl	4(%edx), %eax
	jge	L6
	movl	8(%ebp), %ecx
	movl	-8(%ebp), %eax
	movl	%eax, %edx
	sall	$5, %edx
	movl	12(%ebp), %eax
	movl	28(%edx,%eax), %eax
	movl	%eax, 4(%ecx)
L6:
	movl	-8(%ebp), %eax
	movl	%eax, %ecx
	sall	$5, %ecx
	movl	12(%ebp), %eax
	movl	8(%ebp), %edx
	movl	28(%ecx,%eax), %eax
	cmpl	8(%edx), %eax
	jle	L5
	movl	8(%ebp), %ecx
	movl	-8(%ebp), %eax
	movl	%eax, %edx
	sall	$5, %edx
	movl	12(%ebp), %eax
	movl	28(%edx,%eax), %eax
	movl	%eax, 8(%ecx)
L5:
	leal	-8(%ebp), %eax
	incl	(%eax)
	jmp	L3
L2:
	addl	$8, %esp
	popl	%ebx
	popl	%ebp
	ret
.globl _for_optimized
	.def	_for_optimized;	.scl	2;	.type	32;	.endef
_for_optimized:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ResultInit
	movl	16(%ebp), %eax
	movl	%eax, -4(%ebp)
L9:
	cmpl	$0, -4(%ebp)
	je	L8
	movl	8(%ebp), %ecx
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movl	28(%eax), %eax
	addl	(%edx), %eax
	movl	%eax, (%ecx)
	movl	12(%ebp), %eax
	movl	8(%ebp), %edx
	movl	28(%eax), %eax
	cmpl	4(%edx), %eax
	jge	L12
	movl	8(%ebp), %eax
	movl	12(%ebp), %edx
	movl	28(%edx), %edx
	movl	%edx, 4(%eax)
L12:
	movl	12(%ebp), %eax
	movl	8(%ebp), %edx
	movl	28(%eax), %eax
	cmpl	8(%edx), %eax
	jle	L11
	movl	8(%ebp), %eax
	movl	12(%ebp), %edx
	movl	28(%edx), %edx
	movl	%edx, 8(%eax)
L11:
	leal	12(%ebp), %eax
	addl	$32, (%eax)
	leal	-4(%ebp), %eax
	decl	(%eax)
	jmp	L9
L8:
	leave
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
	.align 4
LC0:
	.ascii "for %u trials, classicTime == %lu, optimizedTime == %lu\12\0"
	.text
.globl _main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1672, %esp
	andl	$-16, %esp
	movl	$0, %eax
	addl	$15, %eax
	addl	$15, %eax
	shrl	$4, %eax
	sall	$4, %eax
	movl	%eax, -1644(%ebp)
	movl	-1644(%ebp), %eax
	call	__alloca
	call	___main
	movl	$0, -24(%ebp)
L15:
	cmpl	$49, -24(%ebp)
	ja	L16
	movl	-24(%ebp), %eax
	sall	$5, %eax
	leal	-8(%ebp), %edx
	addl	%edx, %eax
	leal	-1600(%eax), %edx
	movl	-24(%ebp), %eax
	movl	%eax, 12(%edx)
	leal	-24(%ebp), %eax
	decl	(%eax)
	jmp	L15
L16:
	movl	$0, (%esp)
	call	_time
	movl	%eax, -12(%ebp)
	movl	$10000000, -24(%ebp)
L18:
	cmpl	$0, -24(%ebp)
	je	L19
	movl	$50, 8(%esp)
	leal	-1624(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1640(%ebp), %eax
	movl	%eax, (%esp)
	call	_for_classic
	leal	-24(%ebp), %eax
	decl	(%eax)
	jmp	L18
L19:
	movl	$0, (%esp)
	call	_time
	subl	-12(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	$0, (%esp)
	call	_time
	movl	%eax, -12(%ebp)
	movl	$10000000, -24(%ebp)
L21:
	cmpl	$0, -24(%ebp)
	je	L22
	movl	$50, 8(%esp)
	leal	-1624(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1640(%ebp), %eax
	movl	%eax, (%esp)
	call	_for_optimized
	leal	-24(%ebp), %eax
	decl	(%eax)
	jmp	L21
L22:
	movl	$0, (%esp)
	call	_time
	subl	-12(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$10000000, 4(%esp)
	movl	$LC0, (%esp)
	call	_printf
	movl	$0, %eax
	leave
	ret
	.def	_printf;	.scl	3;	.type	32;	.endef
	.def	_time;	.scl	3;	.type	32;	.endef
