	.data
	.align 4
.LLC0:	.word	1
	.align 4
	.global quicksort
quicksort:
	save	%sp,-128,%sp
	st	%i0,[%fp+68]
	st	%i1,[%fp+72]
	st	%i2,[%fp+76]
	ld	[%fp+72],%o0
	ld	[%fp+68],%o1
	cmp	%o0,%o1
	ble,a	.LL3
	mov	1,%o0
	mov	0,%o0
.LL3:
	st	%o0,[%fp -20]
	st	%o1,[%fp+68]
	ld	[%fp -20],%o0
	subcc	%o0,%g0,%g0
	bne	.LL0
	nop
	ba	.LL1
	nop
.LL0:
	ba	.LLR
	nop
	ba	.LL2
	nop
.LL1:
.LL2:
	ld	[%fp+68],%o0
	set	.LLC0,%o1
	ld	[%o1],%o1
	st	%o0,[%fp+68]
	sub	%o0,%o1,%o0
	mov	%o0,%o2
	st	%o2,[%fp -4]
	ld	[%fp+72],%o2
	mov	%o2,%o3
	st	%o3,[%fp -8]
	ld	[%fp+72],%o3
	ld	[%fp+76],%o4
	st	%o3,[%fp+72]
	sll	%o3,2,%o3
	add	%o4,%o3,%o3
	ld	[%o3],%o4
	mov	%o4,%o5
	st	%o5,[%fp -12]
	st	%o0,[%fp -20]
	st	%o2,[%fp+72]
	st	%o3,[%fp -20]
	st	%o4,[%fp -24]
.LL4:
.LL5:
	ld	[%fp -4],%o0
	set	.LLC0,%o1
	ld	[%o1],%o1
	st	%o0,[%fp -4]
	add	%o0,%o1,%o0
	mov	%o0,%o2
	st	%o2,[%fp -4]
	ld	[%fp -4],%o2
	ld	[%fp+76],%o3
	st	%o2,[%fp -4]
	sll	%o2,2,%o2
	add	%o3,%o2,%o2
	ld	[%o2],%o3
	st	%o0,[%fp -20]
	st	%o2,[%fp -28]
	st	%o3,[%fp -32]
	ld	[%fp -32],%o0
	ld	[%fp -12],%o1
	cmp	%o0,%o1
	bl,a	.LL6
	mov	1,%o0
	mov	0,%o0
.LL6:
	st	%o0,[%fp -36]
	st	%o1,[%fp -12]
	ld	[%fp -36],%o0
	subcc	%o0,%g0,%g0
	bne	.LL5
	nop
.LL7:
	ld	[%fp -8],%o0
	set	.LLC0,%o1
	ld	[%o1],%o1
	st	%o0,[%fp -8]
	sub	%o0,%o1,%o0
	mov	%o0,%o2
	st	%o2,[%fp -8]
	ld	[%fp -8],%o2
	ld	[%fp+76],%o3
	st	%o2,[%fp -8]
	sll	%o2,2,%o2
	add	%o3,%o2,%o2
	ld	[%o2],%o3
	st	%o0,[%fp -20]
	st	%o2,[%fp -28]
	st	%o3,[%fp -32]
	ld	[%fp -32],%o0
	ld	[%fp -12],%o1
	cmp	%o0,%o1
	bg,a	.LL8
	mov	1,%o0
	mov	0,%o0
.LL8:
	st	%o0,[%fp -36]
	st	%o1,[%fp -12]
	ld	[%fp -36],%o0
	subcc	%o0,%g0,%g0
	bne	.LL7
	nop
	ld	[%fp -4],%o0
	ld	[%fp -8],%o1
	cmp	%o0,%o1
	bl,a	.LL12
	mov	1,%o0
	mov	0,%o0
.LL12:
	st	%o0,[%fp -20]
	st	%o1,[%fp -8]
	ld	[%fp -20],%o0
	subcc	%o0,%g0,%g0
	bne	.LL9
	nop
	ba	.LL10
	nop
.LL9:
	ld	[%fp -4],%o0
	ld	[%fp+76],%o1
	st	%o0,[%fp -4]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	mov	%o1,%o2
	st	%o2,[%fp -16]
	ld	[%fp -4],%o2
	ld	[%fp+76],%o3
	st	%o2,[%fp -4]
	sll	%o2,2,%o2
	add	%o3,%o2,%o2
	ld	[%o2],%o3
	ld	[%fp -8],%o4
	ld	[%fp+76],%o5
	st	%o4,[%fp -8]
	sll	%o4,2,%o4
	add	%o5,%o4,%o4
	ld	[%o4],%o5
	st	%o5,[%o2]
	st	%o0,[%fp -24]
	st	%o1,[%fp -28]
	st	%o2,[%fp -20]
	st	%o3,[%fp -24]
	st	%o4,[%fp -28]
	st	%o5,[%fp -32]
	ld	[%fp -8],%o0
	ld	[%fp+76],%o1
	st	%o0,[%fp -8]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	ld	[%fp -16],%o2
	st	%o2,[%o0]
	st	%o0,[%fp -20]
	st	%o1,[%fp -24]
	st	%o2,[%fp -16]
	ba	.LL11
	nop
.LL10:
	ld	[%fp -4],%o0
	ld	[%fp+76],%o1
	st	%o0,[%fp -4]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	mov	%o1,%o2
	st	%o2,[%fp -16]
	ld	[%fp -4],%o2
	ld	[%fp+76],%o3
	st	%o2,[%fp -4]
	sll	%o2,2,%o2
	add	%o3,%o2,%o2
	ld	[%o2],%o3
	ld	[%fp+72],%o4
	ld	[%fp+76],%o5
	st	%o4,[%fp+72]
	sll	%o4,2,%o4
	add	%o5,%o4,%o4
	ld	[%o4],%o5
	st	%o5,[%o2]
	st	%o0,[%fp -20]
	st	%o1,[%fp -24]
	st	%o2,[%fp -20]
	st	%o3,[%fp -24]
	st	%o4,[%fp -28]
	st	%o5,[%fp -32]
	ld	[%fp+72],%o0
	ld	[%fp+76],%o1
	st	%o0,[%fp+72]
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	ld	[%fp -16],%o2
	st	%o2,[%o0]
	st	%o0,[%fp -20]
	st	%o1,[%fp -24]
	st	%o2,[%fp -16]
.LL11:
	ld	[%fp -4],%o0
	ld	[%fp -8],%o1
	cmp	%o0,%o1
	bl,a	.LL13
	mov	1,%o0
	mov	0,%o0
.LL13:
	st	%o0,[%fp -20]
	st	%o1,[%fp -8]
	ld	[%fp -20],%o0
	subcc	%o0,%g0,%g0
	bne	.LL4
	nop
	ld	[%fp+76],%o2
	ld	[%fp -8],%o1
	ld	[%fp+68],%o0
	st	%o0,[%fp+68]
	st	%o1,[%fp -8]
	st	%o2,[%fp+76]
	call	quicksort
	nop
	st	%o0,[%fp -20]
	ld	[%fp -4],%o0
	set	.LLC0,%o1
	ld	[%o1],%o1
	st	%o0,[%fp -4]
	add	%o0,%o1,%o0
	st	%o0,[%fp -20]
	ld	[%fp+76],%o2
	ld	[%fp+72],%o1
	ld	[%fp -20],%o0
	st	%o0,[%fp -20]
	st	%o1,[%fp+72]
	st	%o2,[%fp+76]
	call	quicksort
	nop
	st	%o0,[%fp -24]
.LLR:
	ret
	restore
