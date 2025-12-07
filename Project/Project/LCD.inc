
.ifndef LCD
LCD:

.include "Delay.inc"

.equ E = 1 << pb2
.equ RW = 1 << pb1
.equ RS = 1 << pb0

.equ PORTCdata = 0x0f
.equ PORTDdata = 0xf0

;Modifies: R16, R17, R28, R29, R30, R31, PORTB, PORTC, PORTD, DDRB, DDRC, DDRD
lcdInit:
	in r16, ddrb
	ori r16, RS | RW | E
	out ddrb, r16

	in r16, ddrc
	ori r16, PORTCdata
	out ddrc, r16

	in r16, ddrd
	ori r16, PORTDdata
	out ddrd, r16

	clr r16
	out portb, r16
	out portc, r16
	out portd, r16

	clr r31

	ldi r28, LOW(20)
	ldi r29, HIGH(20)
	rcall delayMS

	ldi r16, 0b00111000
	rcall lcdCommand

	ldi r16, 0b00001100
	rcall lcdCommand
	
	ret

lcdClear:
	ldi r16, 0b00000001
	rcall lcdCommand
	ret

;Expects: R16=Character in ASCII - Modifies: R17, R28, R29, R30, R31
lcdWrite:
	ldi r31, RS
	rcall lcdInstruction
	
	ldi r28, LOW(40)
	ldi r29, HIGH(40)

	rcall delayUS
	ret

;Expects: R16=Command - Modifies: R17, R28, R29, R30, R31
lcdCommand:
	clr r31
	rcall lcdInstruction
	
	ldi r28, LOW(1600)
	ldi r29, HIGH(1600)

	rcall delayUS
	ret

;Expects: R16=Data, R31=0bXXXXX,E,RW,RS - Modifies: R17, R18, R19, R30
lcdInstruction:
	in r30, portb
	andi r30, ~(RS | RW | E)
	or r30, r31
	out portb, r30

	in r18, portd
	andi r18, ~PORTDdata

	in r19, portc
	andi r19, ~PORTCdata

	mov r17, r16
	andi r17, PORTDdata
	or r17, r18

	andi r16, PORTCdata
	or r16, r19

	out portc, r16
	out portd, r17

	ori r30, E
	out portb, r30

	andi r30, ~E
	out portb, r30
	ret

.endif