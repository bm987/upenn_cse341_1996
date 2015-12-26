	.data
	.align 4
.LLC0:	.word	2
	.align 4
.LLC1:	.word	0
	.align 4
.LLC2:	.word	1
	.align 4
	.global sort
sort:
	save	%sp,-208,%sp
	st	%i0,[%fp+68]
	st	%i1,[%fp+72]
	st	%i2,[%fp+76]
	set	.LLC0,%o1
	ld	[%o1],%o1
	ld	[%fp+68],%o0
	st	%o0,[%fp+68]
	call	.div
	nop
	st	%o0,[%fp -24]
	ld	[%fp -24],%o0
	mov	%o0,%o1
	st	%o1,[%fp -4]
	st	%o0,[%fp -24]
.LL0:
	ld	[%fp -4],%o0
	set	.LLC1,%o1
	ld	[%o1],%o1
	cmp	%o0,%o1
	bg,a	.LL2
	mov	1,%o0
	mov	0,%o0
.LL2:
	st	%o0,[%fp -32]
	ld	[%fp -32],%o0
	subcc	%o0,%g0,%g0
	be	.LL1
	nop
	ld	[%fp -4],%o0
	mov	%o0,%o1
	st	%o1,[%fp -8]
	st	%o0,[%fp -4]
.LL3:
	ld	[%fp -8],%o0
	ld	[%fp+68],%o1
	cmp	%o0,%o1
	bl,a	.LL5
	mov	1,%o0
	mov	0,%o0
.LL5:
	st	%o0,[%fp -52]
	st	%o1,[%fp+68]
	ld	[%fp -52],%o0
	subcc	%o0,%g0,%g0
	be	.LL4
	nop
	ld	[%fp -8],%o0
	ld	[%fp -4],%o1
	st	%o0,[%fp -8]
	sub	%o0,%o1,%o0
	mov	%o0,%o2
	st	%o2,[%fp -12]
	st	%o0,[%fp -56]
	st	%o1,[%fp -4]
.LL6:
	ld	[%fp -12],%o0
	set	.LLC1,%o1
	ld	[%o1],%o1
	cmp	%o0,%o1
	bge,a	.LL8
	mov	1,%o0
	mov	0,%o0
.LL8:
	ld	[%fp -12],%o2
	ld	[%fp+72],%o3
	st	%o2,[%fp -12]
	sll	%o2,2,%o2
	add	%o3,%o2,%o2
	ld	[%o2],%o3
	ld	[%fp -12],%o4
	ld	[%fp -4],%o5
	st	%o4,[%fp -12]
	add	%o4,%o5,%o4
	st	%o0,[%fp -64]
	st	%o2,[%fp -68]
	st	%o3,[%fp -72]
	st	%o4,[%fp -76]
	st	%o5,[%fp -4]
	ld	[%fp -76],%o0
	ld	[%fp+72],%o1
	st	%o0,[%fp -76]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	st	%o0,[%fp -80]
	st	%o1,[%fp -84]
	ld	[%fp -72],%o0
	ld	[%fp -84],%o1
	cmp	%o0,%o1
	bg,a	.LL9
	mov	1,%o0
	mov	0,%o0
.LL9:
	ld	[%fp -64],%o2
	st	%o0,[%fp -88]
	and	%o0,%o2,%o0
	st	%o0,[%fp -92]
	st	%o1,[%fp -84]
	st	%o2,[%fp -64]
	ld	[%fp -92],%o0
	subcc	%o0,%g0,%g0
	be	.LL7
	nop
	ld	[%fp -12],%o0
	ld	[%fp+72],%o1
	st	%o0,[%fp -12]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	mov	%o1,%o2
	st	%o2,[%fp -16]
	ld	[%fp -12],%o2
	ld	[%fp+72],%o3
	st	%o2,[%fp -12]
	sll	%o2,2,%o2
	add	%o3,%o2,%o2
	ld	[%o2],%o3
	ld	[%fp -12],%o4
	ld	[%fp -4],%o5
	st	%o4,[%fp -12]
	add	%o4,%o5,%o4
	st	%o0,[%fp -104]
	st	%o1,[%fp -108]
	st	%o2,[%fp -20]
	st	%o3,[%fp -24]
	st	%o4,[%fp -28]
	st	%o5,[%fp -4]
	ld	[%fp -28],%o0
	ld	[%fp+72],%o1
	st	%o0,[%fp -28]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	ld	[%fp -20],%o2
	st	%o1,[%o2]
	ld	[%fp -12],%o3
	ld	[%fp -4],%o4
	st	%o3,[%fp -12]
	add	%o3,%o4,%o3
	ld	[%fp+72],%o5
	st	%o3,[%fp -20]
	sll	%o3,2,%o3
	add	%o5,%o3,%o3
	ld	[%o3],%o5
	st	%o0,[%fp -32]
	st	%o1,[%fp -36]
	st	%o2,[%fp -20]
	st	%o3,[%fp -24]
	st	%o4,[%fp -4]
	st	%o5,[%fp -28]
	ld	[%fp -16],%o0
	ld	[%fp -24],%o1
	st	%o0,[%o1]
	ld	[%fp+76],%o2
	ld	[%o2],%o3
	ld	[%fp+76],%o4
	ld	[%o4],%o5
	st	%o0,[%fp -16]
	st	%o1,[%fp -24]
	st	%o2,[%fp+76]
	st	%o3,[%fp -20]
	st	%o4,[%fp+76]
	st	%o5,[%fp -24]
	ld	[%fp -24],%o0
	set	.LLC2,%o1
	ld	[%o1],%o1
	st	%o0,[%fp -24]
	add	%o0,%o1,%o0
	ld	[%fp+76],%o2
	st	%o0,[%o2]
	ld	[%fp -12],%o3
	ld	[%fp -4],%o4
	st	%o3,[%fp -12]
	sub	%o3,%o4,%o3
	mov	%o3,%o5
	st	%o5,[%fp -12]
	st	%o0,[%fp -28]
	st	%o2,[%fp+76]
	st	%o3,[%fp -96]
	st	%o4,[%fp -4]
	ba	.LL6
	nop
.LL7:
	ld	[%fp -8],%o0
	inc	%o0
	st	%o0,[%fp -8]
	ba	.LL3
	nop
.LL4:
	set	.LLC0,%o1
	ld	[%o1],%o1
	ld	[%fp -4],%o0
	st	%o0,[%fp -4]
	call	.div
	nop
	st	%o0,[%fp -40]
	ld	[%fp -40],%o0
	mov	%o0,%o1
	st	%o1,[%fp -4]
	st	%o0,[%fp -40]
	ba	.LL0
	nop
.LL1:
.LLR:
	ret
	restore
