	.data
	.align 4
.LLC0:	.word	0
	.align 4
.LLC1:	.word	1
	.align 4
	.global allfact
allfact:
	save	%sp,-120,%sp
	st	%i0,[%fp+68]
	set	.LLC0,%o0
	ld	[%o0],%o0
	mov	%o0,%o1
	st	%o1,[%fp -8]
	set	.LLC1,%o1
	ld	[%o1],%o1
	mov	%o1,%o2
	st	%o2,[%fp -4]
.LL0:
	ld	[%fp -4],%o0
	ld	[%fp+68],%o1
	cmp	%o0,%o1
	ble,a	.LL2
	mov	1,%o0
	mov	0,%o0
.LL2:
	st	%o0,[%fp -16]
	st	%o1,[%fp+68]
	ld	[%fp -16],%o0
	subcc	%o0,%g0,%g0
	be	.LL1
	nop
	ld	[%fp -4],%o0
	st	%o0,[%fp -4]
	call	factorial
	nop
	st	%o0,[%fp -20]
	ld	[%fp -8],%o0
	ld	[%fp -20],%o1
	st	%o0,[%fp -8]
	add	%o0,%o1,%o0
	mov	%o0,%o2
	st	%o2,[%fp -8]
	ld	[%fp -4],%o2
	inc	%o2
	st	%o0,[%fp -24]
	st	%o1,[%fp -20]
	st	%o2,[%fp -4]
	ba	.LL0
	nop
.LL1:
	ld	[%fp -8],%o0
	mov	%o0,%i0
	ba	.LLR
	nop
	st	%o0,[%fp -8]
.LLR:
	ret
	restore
