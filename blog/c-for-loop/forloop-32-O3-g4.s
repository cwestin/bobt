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
	pushl	%ebp			/* save %ebp; current max kept here */
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi			/* save %edi; currrent min kept here */
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi			/* save %esi; current sum kept here */
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx			/* save %ebx; pr goes here */
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	movl	28(%esp), %eax
	movl	20(%esp), %ebx
	testl	%eax, %eax

	    /* ResultInit() got inlined */
	movl	$0, (%ebx)
	movl	$2147483647, 4(%ebx)
	movl	$-2147483648, 8(%ebx)
	je	.L2
	movl	24(%esp), %ecx		/* %ecx is used to point to the array! */
	movl	$-2147483648, %ebp	/* current pr->max in a register */
	movl	$2147483647, %edi	/* current pr->min in a register */
	xorl	%esi, %esi		/* current sum in a register */
	xorl	%edx, %edx		/* i = 0; */
	jmp	.L7
	.p2align 4,,7
	.p2align 3
.L9:
	movl	4(%ebx), %edi		/* load the current min to register */
.L7:
	movl	28(%ecx), %eax		/* load ps[i].myInt into %eax */
	addl	%eax, %esi
	cmpl	%edi, %eax		/* compare pr->min with %eax */
	jge	.L4			/* if (ps[i].myInt >= pr->min) goto L4; */
	movl	%eax, 4(%ebx)		/* pr->min = ps[i].myInt; */
.L4:
	cmpl	%ebp, %eax		/* compare pr->max with %eax */
	jle	.L5			/* if (ps[i].myInt <= pr->max) goto L5; */
	movl	%eax, 8(%ebx)		/* pr->max = ps[i].myInt; */
	movl	%eax, %ebp		/* save to current pr->max */
.L5:
	addl	$1, %edx		/* i++; */
	addl	$32, %ecx		/* move along the array pointer */
	cmpl	28(%esp), %edx		/* compare n with i */
	jne	.L9			/* if (i != n) goto L9; */
	movl	%esi, (%ebx)		/* move register total to pr->sum */
.L2:
	/* restore saved registers */
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
	pushl	%ebp			/* save %ebp; max will go here */
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	pushl	%edi			/* save %edi; min will go here */
	.cfi_def_cfa_offset 12
	.cfi_offset 7, -12
	pushl	%esi			/* save %esi; sum will go here */
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushl	%ebx			/* save %ebx; pr will go here */
	.cfi_def_cfa_offset 20
	.cfi_offset 3, -20
	movl	28(%esp), %edx		/* move n to %edx */
	movl	20(%esp), %ebx		/* move pr to %ebx */
	movl	24(%esp), %ecx		/* move ps to %ecx */
	testl	%edx, %edx		/* test n */

	/* ResultInit() inlined here */
	movl	$0, (%ebx)
	movl	$2147483647, 4(%ebx)
	movl	$-2147483648, 8(%ebx)
	
	je	.L10			/* if (n == 0) goto L10 */
	movl	$-2147483648, %ebp	/* current max in a register */
	movl	$2147483647, %edi	/* current min in a register */
	xorl	%esi, %esi		/* zero sum in a register */
	jmp	.L15
	.p2align 4,,7
	.p2align 3
.L16:
	movl	4(%ebx), %edi		/* move the current min to %edi */
	addl	$32, %ecx		/* ++ps; */
.L15:
	movl	28(%ecx), %eax		/* copy ps[i].myInt into %eax */
	addl	%eax, %esi		/* add to sum */
	cmpl	%edi, %eax		/* compare to current min */
	jge	.L12			/* if (ps[i].myInt >= pr->min) goto L12; */
	movl	%eax, 4(%ebx)		/* pr->min = ps[i].myInt; */
.L12:
	cmpl	%ebp, %eax		/* compare to current max */
	jle	.L13			/* if (ps[i].myInt <= pr->max) goto L13; */
	movl	%eax, 8(%ebx)		/* pr->max = ps[i].myInt; */
	movl	%eax, %ebp		/* also save to register max */
.L13:
	subl	$1, %edx		/* --i; */
	jne	.L16			/* if (i != 0) goto L16; */
	movl	%esi, (%ebx)		/* move sum to pr->sum */
.L10:
	/* restore saved registers */
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
