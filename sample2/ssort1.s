	.data
	.align 4
.LLC0:	.word	0
	.align 4
.LLC1:	.word	1
	.align 4
.LLC2:	.word	5
	.align 4
.LLC3:	.word	2
	.align 4
.LLC4:	.word	6
	.align 4
.LLC5:	.word	3
	.align 4
.LLC6:	.word	4
	.align 4
.LLC7:	.word	7
	.align 4
.LLC8:	.word	8
	.align 4
.LLC9:	.word	9
	.align 4
	.global init
init:
	save	%sp,-104,%sp
	st	%i0,[%fp+68]
	set	.LLC0,%o0
	ld	[%o0],%o0
	ld	[%fp+68],%o1
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	set	.LLC1,%o2
	ld	[%o2],%o2
	st	%o2,[%o0]
	set	.LLC1,%o3
	ld	[%o3],%o3
	ld	[%fp+68],%o4
	sll	%o3,2,%o3
	add	%o4,%o3,%o3
	ld	[%o3],%o4
	set	.LLC2,%o5
	ld	[%o5],%o5
	st	%o5,[%o3]
	st	%o0,[%fp -4]
	st	%o1,[%fp -8]
	st	%o3,[%fp -4]
	st	%o4,[%fp -8]
	set	.LLC3,%o0
	ld	[%o0],%o0
	ld	[%fp+68],%o1
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	set	.LLC4,%o2
	ld	[%o2],%o2
	st	%o2,[%o0]
	set	.LLC5,%o3
	ld	[%o3],%o3
	ld	[%fp+68],%o4
	sll	%o3,2,%o3
	add	%o4,%o3,%o3
	ld	[%o3],%o4
	set	.LLC6,%o5
	ld	[%o5],%o5
	st	%o5,[%o3]
	st	%o0,[%fp -4]
	st	%o1,[%fp -8]
	st	%o3,[%fp -4]
	st	%o4,[%fp -8]
	set	.LLC6,%o0
	ld	[%o0],%o0
	ld	[%fp+68],%o1
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	set	.LLC7,%o2
	ld	[%o2],%o2
	st	%o2,[%o0]
	set	.LLC2,%o3
	ld	[%o3],%o3
	ld	[%fp+68],%o4
	sll	%o3,2,%o3
	add	%o4,%o3,%o3
	ld	[%o3],%o4
	set	.LLC8,%o5
	ld	[%o5],%o5
	st	%o5,[%o3]
	st	%o0,[%fp -4]
	st	%o1,[%fp -8]
	st	%o3,[%fp -4]
	st	%o4,[%fp -8]
	set	.LLC4,%o0
	ld	[%o0],%o0
	ld	[%fp+68],%o1
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	set	.LLC9,%o2
	ld	[%o2],%o2
	st	%o2,[%o0]
	set	.LLC7,%o3
	ld	[%o3],%o3
	ld	[%fp+68],%o4
	sll	%o3,2,%o3
	add	%o4,%o3,%o3
	ld	[%o3],%o4
	set	.LLC0,%o5
	ld	[%o5],%o5
	st	%o5,[%o3]
	st	%o0,[%fp -4]
	st	%o1,[%fp -8]
	st	%o3,[%fp -4]
	st	%o4,[%fp -8]
	set	.LLC8,%o0
	ld	[%o0],%o0
	ld	[%fp+68],%o1
	sll	%o0,2,%o0
	add	%o1,%o0,%o0
	ld	[%o0],%o1
	set	.LLC3,%o2
	ld	[%o2],%o2
	st	%o2,[%o0]
	set	.LLC9,%o3
	ld	[%o3],%o3
	ld	[%fp+68],%o4
	sll	%o3,2,%o3
	add	%o4,%o3,%o3
	ld	[%o3],%o4
	set	.LLC5,%o5
	ld	[%o5],%o5
	st	%o5,[%o3]
	st	%o0,[%fp -4]
	st	%o1,[%fp -8]
	st	%o3,[%fp -4]
	st	%o4,[%fp -8]
.LLR:
	ret
	restore
