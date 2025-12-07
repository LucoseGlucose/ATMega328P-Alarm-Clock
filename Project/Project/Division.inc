
.ifndef DIVISION
DIVISION:

;Expects: R22=Numerator, R23=Denominator - Returns: R24->Remainder, R25->Result
divide:
	mov r24, r22
	clr r25

dLoop:
	inc r25
	sub r24, r23
	brlo dFinish
	rjmp dLoop

dFinish:
	add r24, r23
	dec r25
	ret

;Expects: R21=NumeratorLOW, R22=NumeratorHIGH, R23=Denominator - Returns: R24->Remainder, R26->Result - Modifies: R25
divideByteFromWord:
	mov r24, r21
	mov r25, r22
	clr r26

dwLoop:
	inc r26
	sub r24, r23
	sbci r25, 0
	tst r25
	brne dwContinue

	cp r24, r23
	brlo dwFinish

dwContinue:
	rjmp dwLoop

dwFinish:
	ret

.endif