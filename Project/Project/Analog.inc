
.ifndef ANALOG
ANALOG:

;Modifies: R16
analogReadInit:
	ldi r16, 0b01100000
	sts admux, r16

	ldi r16, 0b10000111
	sts adcsra, r16
	ret

;Modifies: R16
analogWriteInit0:
	ldi r16, (1 << com0a1) | (1 << wgm00)
	out tccr0a, r16

	ldi r16, 1 << cs02
	out tccr0b, r16
	ret

;Modifies: R16
analogWriteInit2:
	ldi r16, (1 << com2b1) | (1 << com2a1) | (1 << wgm20)
	sts tccr2a, r16

	ldi r16, (1 << cs22) | (1 << cs21)
	sts tccr2b, r16
	ret

;Returns: R16->Result
.macro frq
	ldi @0, 16000000 / (2 * 64 * @1) - 1
.endmacro

;Modifies: R16
analogBeepInit0:
	ldi r16, (1 << com0a0) | (1 << wgm01)
	out tccr0a, r16

	ldi r16, (1 << cs00) | (1 << cs01)
	out tccr0b, r16
	ret

;Modifies: R16
analogBeepInit2:
	ldi r16, (1 << com2b0) | (1 << com2a0) | (1 << wgm21)
	sts tccr2a, r16

	ldi r16, 1 << cs22
	sts tccr2b, r16
	ret

;Expects: R17=Analog pin index - Returns: R11->8 bit digital value - Modifies: R16, R18
analogRead:
	lds r18, admux
	andi r18, 0xf0
	or r18, r17
	sts admux, r18

	lds r16, adcsra
	ori r16, 1 << adsc
	sts adcsra, r16

analogReadWait:
	lds r16, adcsra
	sbrc r16, adsc
	rjmp analogReadWait

	lds r11, adch
	ret

;Expects: X=OCRnx address, R16=Data
analogWrite:
	st X, r16
	ret

.endif