	.data
	.align 4
.LLC0:	.word	1
	.align 4
	.global factorial
factorial:
	save	%sp,-112,%sp
	st	%i0,[%fp+68]
	ld	[%fp+68],%o0
	set	.LLC0,%o1
	ld	[%o1],%o1
	cmp	%o0,%o1
	bg,a	.LL3
	mov	1,%o0
	mov	0,%o0
.LL3:
	st	%o0,[%fp -4]
	ld	[%fp -4],%o0
	subcc	%o0,%g0,%g0
	bne	.LL0
	nop
	ba	.LL1
	nop
.LL0:
	ld	[%fp+68],%o0
	set	.LLC0,%o1
	ld	[%o1],%o1
	st	%o0,[%fp+68]
	sub	%o0,%o1,%o0
	st	%o0,[%fp -8]
	ld	[%fp -8],%o0
	st	%o0,[%fp -8]
	call	factorial
	nop
	st	%o0,[%fp -12]
	ld	[%fp -12],%o1
	ld	[%fp+68],%o0
	st	%o0,[%fp+68]
	st	%o1,[%fp -12]
	call	.mul
	nop
	st	%o0,[%fp -20]
	ld	[%fp -20],%o0
	mov	%o0,%i0
	ba	.LLR
	nop
	st	%o0,[%fp -20]
	ba	.LL2
	nop
.LL1:
	set	.LLC0,%o0
	ld	[%o0],%o0
	mov	%o0,%i0
	ba	.LLR
	nop
.LL2:
.LLR:
	ret
	restore
