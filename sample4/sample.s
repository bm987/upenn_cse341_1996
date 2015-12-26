	.data
	.align 4
.LLC0:	.word	2
	.align 4
.LLC1:	.word	0
	.align 4
.LLC2:	.word	100
	.align 4
.LLC3:	.word	50
	.align 4
	.global sample
sample:
	save	%sp,-144,%sp
	st	%i0,[%fp+68]
	st	%i1,[%fp+72]
	ld	[%fp+68],%o0
	mov	%o0,%o1
	st	%o1,[%fp -8]
	st	%o0,[%fp+68]
.LL0:
	ld	[%fp -8],%o0
	ld	[%fp+72],%o1
	cmp	%o0,%o1
	bl,a	.LL2
	mov	1,%o0
	mov	0,%o0
.LL2:
	st	%o0,[%fp -32]
	st	%o1,[%fp+72]
	ld	[%fp -32],%o0
	subcc	%o0,%g0,%g0
	be	.LL1
	nop
	ld	[%fp -8],%o0
	mov	%o0,%o1
	st	%o1,[%fp -12]
	st	%o0,[%fp -8]
.LL3:
	ld	[%fp -12],%o0
	inc	%o0
	st	%o0,[%fp -12]
	ld	[%fp -12],%o0
	set	.LLC1,%o1
	ld	[%o1],%o1
	cmp	%o0,%o1
	bge,a	.LL4
	mov	1,%o0
	mov	0,%o0
.LL4:
	st	%o0,[%fp -28]
	ld	[%fp -28],%o0
	subcc	%o0,%g0,%g0
	bne	.LL3
	nop
	ld	[%fp -8],%o0
	set	.LLC2,%o1
	ld	[%o1],%o1
	cmp	%o0,%o1
	bg,a	.LL8
	mov	1,%o0
	mov	0,%o0
.LL8:
	st	%o0,[%fp -28]
	ld	[%fp -8],%o0
	set	.LLC3,%o1
	ld	[%o1],%o1
	cmp	%o0,%o1
	bl,a	.LL9
	mov	1,%o0
	mov	0,%o0
.LL9:
	ld	[%fp -28],%o2
	st	%o0,[%fp -32]
	or	%o0,%o2,%o0
	st	%o0,[%fp -36]
	st	%o2,[%fp -28]
	ld	[%fp -36],%o0
	subcc	%o0,%g0,%g0
	bne	.LL5
	nop
	ba	.LL6
	nop
.LL5:
	ld	[%fp -8],%o0
	mov	%o0,%i0
	ba	.LLR
	nop
	st	%o0,[%fp -8]
	ba	.LL7
	nop
.LL6:
.LL7:
.LL10:
	set	.LLC1,%o0
	ld	[%o0],%o0
	ld	[%fp -8],%o1
	st	%o1,[%fp -8]
	xor	%o0,%o1,%o1
	subcc	%g0,%o1,%g0
	addx	%g0,-1,%o0
	st	%o0,[%fp -28]
	ld	[%fp -28],%o0
	subcc	%o0,%g0,%g0
	be	.LL12
	nop
.LL11:
	ld	[%fp -8],%o0
	dec	%o0
	st	%o0,[%fp -8]
	ba	.LL10
	nop
.LL12:
	ld	[%fp -8],%o0
	ld	[%fp -12],%o1
	st	%o0,[%fp -8]
	add	%o0,%o1,%o0
	fitos	%f0,%o0
	ld	[%fp -24],%f1
	st	%f1,[%fp -24]
	fadds	%f1,%f0,%f1
	ld	[%fp -4],%o2
	fitos	%f2,%o2
	st	%f1,[%fp -36]
	fsubs	%f1,%f2,%f1
	st	%f1,[%fp -44]
	ld	[%fp -44],%o3
	mov	%o3,%o4
	st	%o4,[%fp -20]
	ld	[%fp -16],%o4
	ld	[%o4],%o5
	st	%o0,[%fp -28]
	st	%o1,[%fp -12]
	st	%o2,[%fp -4]
	st	%o3,[%fp -44]
	st	%o4,[%fp -16]
	st	%o5,[%fp -28]
	add	%fp,68,%o0
	ld	[%fp -16],%o1
	st	%o0,[%o1]
	ld	[%fp -16],%o2
	ld	[%o2],%o3
	set	.LLC2,%o4
	ld	[%o4],%o4
	st	%o3,[%fp -28]
	xor	%o4,%o3,%o3
	subcc	%g0,%o3,%g0
	subx	%g0,-1,%o4
	st	%o0,[%fp -32]
	st	%o1,[%fp -16]
	st	%o2,[%fp -16]
	st	%o4,[%fp -32]
	ld	[%fp -32],%o0
	subcc	%o0,%g0,%g0
	bne	.LL13
	nop
	ba	.LL14
	nop
.LL13:
	ld	[%fp -16],%o0
	ld	[%o0],%o1
	mov	%o1,%i0
	ba	.LLR
	nop
	st	%o0,[%fp -16]
	st	%o1,[%fp -36]
	ba	.LL15
	nop
.LL14:
.LL15:
	set	.LLC0,%o1
	ld	[%o1],%o1
	ld	[%fp -8],%o0
	st	%o0,[%fp -8]
	call	.mul
	nop
	st	%o0,[%fp -40]
	ld	[%fp -40],%o0
	mov	%o0,%o1
	st	%o1,[%fp -8]
	st	%o0,[%fp -40]
	ba	.LL0
	nop
.LL1:
	st	%f0,[%fp -32]
	st	%f2,[%fp -40]
.LLR:
	ret
	restore
