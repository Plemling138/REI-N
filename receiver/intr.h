#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

/*Define Software-Intrput-vetor*/

#define SOFTVEC_TYPE_NUM     5 //Number of Software-Intrupt-Vector

#define SOFTVEC_TYPE_SOFTERR 0 //Software error
#define SOFTVEC_TYPE_SYSCALL 1 //SystemCall
#define SOFTVEC_TYPE_SERINTR 2 //Serial-Intrupt
#define SOFTVEC_TYPE_TIMINTR 3
#define SOFTVEC_TYPE_ETHINTR 4

#endif
