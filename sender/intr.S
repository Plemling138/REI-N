#include "intr.h"
	.h8300h
	.section .text

	.global _intr_softerr
#	.type   _intr_softerr,@function

_intr_softerr:
/*Save value to register for stack*/
	mov.l   er6,@-er7
	mov.l   er5,@-er7
	mov.l   er4,@-er7
	mov.l   er3,@-er7
	mov.l   er2,@-er7
	mov.l   er1,@-er7
	mov.l   er0,@-er7
	mov.l   er7,er1
	mov.l   #_intrstack,sp /*Use interrupt stack*/
	mov.l   er1,@-er7 /*Save old value of stackpointer*/
	mov.w   #SOFTVEC_TYPE_SOFTERR,r0
	jsr     @_interrupt
	mov.l   @er7+,er1 /*recovery old value of stackpointer from interrupt stack*/
	mov.l   er1,er7
/*recovery value for register to stack*/
	mov.l   @er7+,er0
	mov.l   @er7+,er1
	mov.l   @er7+,er2
	mov.l   @er7+,er3
	mov.l   @er7+,er4
	mov.l   @er7+,er5
	mov.l   @er7+,er6
	rte /*return interrupt*/

	.global _intr_syscall
#	.type   _intr_syscall,@function

_intr_syscall:
/*Save value to register for stack*/
	mov.l   er6,@-er7
	mov.l   er5,@-er7
	mov.l   er4,@-er7
	mov.l   er3,@-er7
	mov.l   er2,@-er7
	mov.l   er1,@-er7
	mov.l   er0,@-er7
	mov.l   er7,er1
	mov.l   #_intrstack,sp /*Use interrupt stack*/
	mov.l   er1,@-er7 /*Save old value of stackpointer*/
	mov.w   #SOFTVEC_TYPE_SYSCALL,r0
	jsr     @_interrupt
	mov.l   @er7+,er1 /*recovery old value of stackpointer from interrupt stack*/
	mov.l   er1,er7
/*recovery value for register to stack*/
	mov.l   @er7+,er0
	mov.l   @er7+,er1
	mov.l   @er7+,er2
	mov.l   @er7+,er3
	mov.l   @er7+,er4
	mov.l   @er7+,er5
	mov.l   @er7+,er6
	rte /*return interrupt*/

	.global _intr_serintr
#	.type   _intr_serintr,@function

_intr_serintr:	
/*Save value to register for stack*/
	mov.l   er6,@-er7
	mov.l   er5,@-er7
	mov.l   er4,@-er7
	mov.l   er3,@-er7
	mov.l   er2,@-er7
	mov.l   er1,@-er7
	mov.l   er0,@-er7
	mov.l   er7,er1
	mov.l   #_intrstack,sp /*Use interrupt stack*/
	mov.l   er1,@-er7 /*Save old value of stackpointer*/
	mov.w   #SOFTVEC_TYPE_SERINTR,r0
	jsr     @_interrupt
	mov.l   @er7+,er1 /*recovery old value of stackpointer from interrupt stack*/
	mov.l   er1,er7
/*recovery value for register to stack*/
	mov.l   @er7+,er0
	mov.l   @er7+,er1
	mov.l   @er7+,er2
	mov.l   @er7+,er3
	mov.l   @er7+,er4
	mov.l   @er7+,er5
	mov.l   @er7+,er6
	rte /*return interrupt*/

	.global _intr_timintr
#	.type   _intr_timintr,@function

_intr_timintr:	
/*Save value to register for stack*/
	mov.l   er6,@-er7
	mov.l   er5,@-er7
	mov.l   er4,@-er7
	mov.l   er3,@-er7
	mov.l   er2,@-er7
	mov.l   er1,@-er7
	mov.l   er0,@-er7
	mov.l   er7,er1
	mov.l   #_intrstack,sp /*Use interrupt stack*/
	mov.l   er1,@-er7 /*Save old value of stackpointer*/
	mov.w   #SOFTVEC_TYPE_TIMINTR,r0
	jsr     @_interrupt
	mov.l   @er7+,er1 /*recovery old value of stackpointer from interrupt stack*/
	mov.l   er1,er7
/*recovery value for register to stack*/
	mov.l   @er7+,er0
	mov.l   @er7+,er1
	mov.l   @er7+,er2
	mov.l   @er7+,er3
	mov.l   @er7+,er4
	mov.l   @er7+,er5
	mov.l   @er7+,er6
	rte /*return interrupt*/

	.global	_intr_ethintr
#	.type		_intr_ethintr,@function

_intr_ethintr:	/*Save value to register for stack*/
	mov.l   er6,@-er7
	mov.l   er5,@-er7
	mov.l   er4,@-er7
	mov.l   er3,@-er7
	mov.l   er2,@-er7
	mov.l   er1,@-er7
	mov.l   er0,@-er7
	mov.l   er7,er1
	mov.l   #_intrstack,sp /*Use interrupt stack*/
	mov.l   er1,@-er7 /*Save old value of stackpointer*/
	mov.w   #SOFTVEC_TYPE_ETHINTR,r0
	jsr     @_interrupt
	mov.l   @er7+,er1 /*recovery old value of stackpointer from interrupt stack*/
	mov.l   er1,er7
/*recovery value for register to stack*/
	mov.l   @er7+,er0
	mov.l   @er7+,er1
	mov.l   @er7+,er2
	mov.l   @er7+,er3
	mov.l   @er7+,er4
	mov.l   @er7+,er5
	mov.l   @er7+,er6
	rte /*return interrupt*/

