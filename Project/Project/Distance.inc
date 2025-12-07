
.ifndef DISTANCE
DISTANCE:

.equ TRIG = pb3
.equ ECHO = pc4

.include "Delay.inc"

initSensor:
	sbi ddrb, TRIG
	cbi ddrc, ECHO
	ret

;Returns: R5->Pulse time in us LOW, R6->Pulse time in us HIGH - Modifies: R15, R20, R21, R28, R29
getPulseUS:
	sbi portb, TRIG
	ldi r28, LOW(15)
	ldi r29, HIGH(15)
	rcall delayUS
	
	cbi portb, TRIG
	
	clr r15
	sts tcnt1h, r15
	sts tcnt1l, r15

	sbi tifr1, tov1
	sts tccr1a, r15
	
	ldi r21, 1 << cs11

waitForEcho:
	in r20, pinc
	sbrs r20, ECHO
	rjmp waitForEcho

	sts tccr1b, r21

waitForOverflow:
	in r20, tifr1
	sbrc r20, tov1
	brne returnZeroPulse

	in r20, pinc
	sbrc r20, ECHO
	rjmp waitForOverflow

returnPulse:
	sts tccr1b, r15
	lds r5, tcnt1l
	lds r6, tcnt1h

	ror r6
	ror r5
	ret

returnZeroPulse:
	sts tccr1b, r15
	clr r5
	clr r6
	ret

;Returns: R26->Distance in inches - Modifies: R5, R6, R15, R20, R21, R22, R23, R24, R25, R28, R29
getDistanceIN:
	rcall getPulseUS
	mov r22, r6
	mov r21, r5
	ldi r23, 148
	rcall divideByteFromWord
	ret

.endif