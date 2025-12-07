
.ifndef CONVERSIONS
CONVERSIONS:

.include "Division.inc"

;Expects: R22=Input - Returns: R9->100s, R8->10s, R9->1s
toBCD:
	ldi r23, 100
	rcall divide
	mov r9, r25
	mov r22, r24
	ldi r23, 10
	rcall divide
	mov r8, r25
	mov r7, r24
	ret

;Expects: R16=Input - Returns: R16->Result
numToAscii:
	ldi r19, 48
	add r16, r19
	ret

.endif