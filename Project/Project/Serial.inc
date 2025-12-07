
.ifndef SERIAL
SERIAL:

.equ BAUD = 9600
.equ BAUDCLK = 16000000 / (16 * BAUD) - 1

;Modifies: R16
serialInit:
	ldi r16, LOW(BAUDCLK)
	sts ubrr0l, r16
	ldi r16, HIGH(BAUDCLK)
	sts ubrr0h, r16

	ldi r16, (1 << txen0) | (1 << rxen0)
	sts ucsr0b, r16

	ldi r16, (1 << ucsz01) | (1 << ucsz00)
	sts ucsr0c, r16
	ret

;Expects: R16=Data - Modifies: R15
serialWrite:
	lds r15, ucsr0a
	sbrs r15, udre0
	rjmp serialWrite

	sts udr0, r16
	ret

;Returns: R16->Result - Modifies: R15
serialRead:
	lds r15, ucsr0a
	sbrs r15, rxc0
	rjmp serialRead

	lds r16, udr0
	ret

.endif