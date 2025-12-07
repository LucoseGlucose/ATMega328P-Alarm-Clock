
.ifndef DELAY

;Expects: R28=Delay length in us LOW, R29=Delay length in us HIGH - Modifies: R15, R21, R20, R27
delayUS:
	ldi r27, 2
	ldi r21, 1 << cs11

	rcall delay
	ret

;Expects: R28=Delay length in ms LOW, R29=Delay length in ms HIGH - Modifies: R15, R21, R20, R27
delayMS:
	ldi r27, 250
	ldi r21, (1 << cs11) | (1 << cs10)

	rcall delay
	ret

;Expects: R28=Cycle count LOW, R29=Cycle count HIGH, R21=Clock prescaler, R27=Iteration count - Modifies: R15, R20
delay:
	clr r15
	sts tcnt1h, r15
	sts tcnt1l, r15

	sbi tifr1, ocf1a

	sts ocr1ah, r29
	sts ocr1al, r28

	ldi r20, 1 << com1a0
	sts tccr1a, r20

	sts tccr1b, r21

waitForMatch:
	in r20, tifr1
	sbrs r20, ocf1a
	rjmp waitForMatch

	dec r27
	brne delay

	sts tccr1b, r15
	ret

.endif