DSEG	SEGMENT
a	DW	0
b	DW	0
c	DW	0
d	DW	0
e	DW	0
p	DW	0
q	DW	0
t	DW	0
r	DW	0
arr	DW	10 DUP(0)
bb	DW	20 DUP(0)
T2	DW	0
T12	DW	0
DSEG	ENDS
CSEG	SEGMENT
	ASSUME	CS:CSEG,DS:DSEG
START:	MOV	AX,DSEG
	MOV	DS,AX 	MOV	AX,9
 	MOV	a,AX
 	MOV	AX,39
 	MOV	arr+0,AX
 	MOV	AX,0
 	MOV	bb+0,AX
 	MOV	AX,0
 	ADD	AX,39
 	MOV	bb+2,AX
 	MOV	AX,a
 	CMP	AX,3
 	JAE	ABC
 	MOV	AX,a
 	ADD	AX,2
 	MOV	b,AX
 	MOV	AX,8
 	MOV	c,AX
 	MOV	AX,b
 	MOV	BX,8
 	MUL	BX
 	MOV	d,AX
 	ADD	AX,3
 	MOV	e,AX
 	MOV	AX,t
 	SUB	AX,r
 	MOV	p,AX
 	MOV	BX,9
 	MUL	BX
 	MOV	q,AX
 	JMP	DEF
ABC:	MOV	AX,a
 	ADD	AX,2
 	MOV	b,AX
DEF:	MOV	AX,b
 	CMP	AX,6
 	JBE	GHI
 	MOV	AX,b
 	ADD	AX,a
 	MOV	c,AX
 	MOV	AX,b
 	SUB	AX,1
 	MOV	b,AX
 	JMP	DEF
GHI:	MOV	AX,4C00H
 	INT	21H
CSEG	ENDS
	END	START
