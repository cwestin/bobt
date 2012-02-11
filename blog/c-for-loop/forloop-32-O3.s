	.file	"forloop.c"
	.text
	.p2align 4,,15
.globl _ResultInit
	.def	_ResultInit;	.scl	2;	.type	32;	.endef
_ResultInit:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movl	$0, (%eax)
	movl	$2147483647, 4(%eax)
	movl	$-2147483648, 8(%eax)
	popl	%ebp
	ret
	.p2align 4,,15
.globl _for_classic
	.def	_for_classic;	.scl	2;	.type	32;	.endef
_for_classic:
	pushl	%ebp		/* save the current stack pointer */
	movl	%esp, %ebp
	movl	8(%ebp), %ecx	/* load pr into %ecx */
	pushl	%edi		/* save whatever's in %edi; n goes here */
	movl	16(%ebp), %edi	/* load n into %edi */
	pushl	%esi		/* save whatever's in %esi; ps goes here */
	movl	12(%ebp), %esi  /* load ps into %esi */
	pushl	%ebx		/* save whatever's in %ebx */
	xorl	%ebx, %ebx	/* i = 0; */
	cmpl	%edi, %ebx      /* compare n to i */

		/* ResultInit got inlined here! */
	movl	$0, (%ecx)    	  	  /* pr->sum = 0; */
	movl	$2147483647, 4(%ecx)	  /* pr->min = INT_MAX; */
	movl	$-2147483648, 8(%ecx)	  /* pr->max = INT_MIN; */

	jae	L11		/* jump above equal: if (i >= n) goto L11; */
	.p2align 4,,15
L16:
	movl	%ebx, %edx	/* take the index i... */
	sall	$5, %edx	/* shift arithmetic left:  scale it by 32 -- pointer arithmetic */
	movl	28(%edx,%esi), %eax	/* copy ps[i].myInt into %eax */
	addl	%eax, (%ecx)		/* add that to pr->sum */
	movl	28(%edx,%esi), %eax	/* copy ps[i].myInt into %eax */
	cmpl	4(%ecx), %eax		/* compare that to pr->min */
	jge	L7			/* if (ps[i].myInt >= pr->min) goto L7; */
	movl	%eax, 4(%ecx)		/* pr->min = ps[i].myInt; */
	movl	28(%edx,%esi), %eax	/* copy ps[i].myInt into %eax */
L7:
	cmpl	8(%ecx), %eax		/* compare that to pr->max */
	jle	L6			/* if (ps[i].myInt <= pr->max) goto L6; */
	movl	%eax, 8(%ecx)		/* pr->max = ps[i].myInt; */
L6:
	incl	%ebx		/* i++; */
	cmpl	%edi, %ebx	/* compare n with i */
	jb	L16   		/* jump below:  if (i < n) goto L16; */
L11:
	popl	%ebx		/* restore %ebx (was i) */
	popl	%esi		/* restore %psi (was ps) */
	popl	%edi		/* restore %edi (was n) */
	popl	%ebp		/* restore the stack pointer */
	ret
	.p2align 4,,15
.globl _for_optimized
	.def	_for_optimized;	.scl	2;	.type	32;	.endef
_for_optimized:
	pushl	%ebp			/* save the stack pointer */
	movl	%esp, %ebp
	movl	16(%ebp), %ecx		/* i = n; */
	pushl	%ebx			/* save %ebx; pr goes here */
	movl	8(%ebp), %ebx		/* load pr into %ebx */
	movl	12(%ebp), %edx		/* load ps into %edx */
	testl	%ecx, %ecx		/* test i */

	    /* ResultInit was inlined! */
	movl	$0, (%ebx)		/* pr->sum = 0; */
	movl	$2147483647, 4(%ebx)	/* pr->min = INT_MAX; */
	movl	$-2147483648, 8(%ebx)	/* pr->max = INT_MIN; */
	
	je	L26			/* if (i == 0) goto L26; */
	.p2align 4,,15
L30:
	movl	28(%edx), %eax		/* copy ps->myInt into %eax */
	addl	%eax, (%ebx)		/* pr->sum += ps->myInt; */
	movl	28(%edx), %eax		/* copy ps->myInt into %eax */
	cmpl	4(%ebx), %eax		/* compare pr->min with that */
	jge	L22			/* if (ps->myInt >= pr->min) goto L22; */
	movl	%eax, 4(%ebx)		/* pr->min = ps->myInt; */
	movl	28(%edx), %eax		/* copy ps->myInt into %eax */
L22:
	cmpl	8(%ebx), %eax		/* compare pr->max with that */
	jle	L21			/* if (ps->myInt <= pr->max) goto L21; */
	movl	%eax, 8(%ebx)		/* pr->max = ps->myInt; */
L21:
	addl	$32, %edx		/* ++ps; scaled pointer arithmetic */
	decl	%ecx			/* --i; */
	jne	L30			/* jump not equal; if (i) goto L30; */
L26:
	popl	%ebx			/* restore %ebx */
	popl	%ebp			/* restore the stack pointer */
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
	.align 4
LC0:
	.ascii "for %u trials, classicTime == %lu, optimizedTime == %lu\12\0"
	.text
	.p2align 4,,15
.globl _main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	pushl	%ebp
	movl	$16, %eax
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1676, %esp
	andl	$-16, %esp
	call	__alloca
	call	___main
	xorl	%eax, %eax
	leal	-1596(%ebp), %edx
L35:
	movl	%eax, (%edx)
	decl	%eax
	subl	$32, %edx
	cmpl	$49, %eax
	jbe	L35
	movl	$0, (%esp)
	movl	$10000000, %edi
	call	_time
	movl	%eax, -1644(%ebp)
	movl	%edi, -1652(%ebp)
L47:
	xorl	%esi, %esi
	movl	$2147483647, %ebx
	movl	$-2147483648, %ecx
	movl	%esi, -1640(%ebp)
	xorl	%edi, %edi
	xorl	%eax, %eax
	movl	%ebx, -1636(%ebp)
	movl	$2147483647, %esi
	movl	$-2147483648, %ebx
	movl	%ecx, -1632(%ebp)
	.p2align 4,,15
L45:
	movl	%edi, %ecx
	sall	$5, %ecx
	movl	-1596(%ecx,%ebp), %edx
	addl	%edx, %eax
	movl	%eax, -1640(%ebp)
	movl	-1596(%ecx,%ebp), %edx
	cmpl	%esi, %edx
	jge	L43
	movl	%edx, -1636(%ebp)
	movl	%edx, %esi
	movl	-1596(%ecx,%ebp), %edx
L43:
	cmpl	%ebx, %edx
	jle	L42
	movl	%edx, -1632(%ebp)
	movl	%edx, %ebx
L42:
	incl	%edi
	cmpl	$50, %edi
	jb	L45
	decl	-1652(%ebp)
	jne	L47
	movl	$0, (%esp)
	call	_time
	movl	%eax, -1648(%ebp)
	movl	-1644(%ebp), %edi
	subl	%edi, -1648(%ebp)
	movl	$0, (%esp)
	call	_time
	movl	%eax, -1660(%ebp)
	movl	$10000000, %eax
	movl	%eax, -1656(%ebp)
L59:
	movl	$2147483647, %ebx
	movl	$-2147483648, %ecx
	xorl	%edx, %edx
	movl	%ebx, -1636(%ebp)
	leal	-1624(%ebp), %esi
	movl	$50, %edi
	movl	%ecx, -1632(%ebp)
	xorl	%eax, %eax
	movl	$2147483647, %ebx
	movl	%edx, -1640(%ebp)
	movl	$-2147483648, %ecx
	.p2align 4,,15
L57:
	movl	28(%esi), %edx
	addl	%edx, %eax
	movl	%eax, -1640(%ebp)
	movl	28(%esi), %edx
	cmpl	%ebx, %edx
	jge	L55
	movl	%edx, -1636(%ebp)
	movl	%edx, %ebx
	movl	28(%esi), %edx
L55:
	cmpl	%ecx, %edx
	jle	L54
	movl	%edx, -1632(%ebp)
	movl	%edx, %ecx
L54:
	addl	$32, %esi
	decl	%edi
	jne	L57
	decl	-1656(%ebp)
	jne	L59
	movl	$0, (%esp)
	call	_time
	movl	$LC0, (%esp)
	movl	-1660(%ebp), %edi
	movl	$10000000, %esi
	movl	%esi, 4(%esp)
	subl	%edi, %eax
	movl	%eax, 12(%esp)
	movl	-1648(%ebp), %eax
	movl	%eax, 8(%esp)
	call	_printf
	leal	-12(%ebp), %esp
	xorl	%eax, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.def	_printf;	.scl	3;	.type	32;	.endef
	.def	_time;	.scl	3;	.type	32;	.endef
