
.org 0
jmp start

;Interrupt Vector for 1 Hz Clock from DS-1307
.org int0addr
	rcall loop
	reti

;Include utility files
.include "Clock.inc"
.include "Conversions.inc"
.include "LCD.inc"
.include "Analog.inc"

;Initialize interrupts, ports, LCD, clock, timer for buzzer, and delay for a quarter of a second
start:
	sbi portd, pd2

	clr r16
	out ddrd, r16

	rcall lcdInit

	sei
	sbi eimsk, int0

	ldi r16, (1 << isc01) | (1 << isc11)
	sts eicra, r16

	rcall analogBeepInit2
	frq r16, 1400
	sts ocr2a, r16

	rcall clockInit

	ldi r28, 250
	clr r29
	rcall delayMS

;Infinite loop since the main loop happens from the 1 Hz interrupt
here:
	rjmp here

;Main loop
loop:
	rcall lcdClear ;Clear LCD and reset the cursor

	ldi r16, 2
	rcall lcdCommand
	
	rcall clockGetTime
	rcall clockGetAlarm

	;Check if alarm on/off button has been pressed
	in r16, eifr
	sbrs r16, int1
	rjmp testAlarm

;If it has toggle the alarm state
toggleAlarm:
	sbi eifr, int1

	mov r16, r9
	com r16
	bst r16, 0

	clr r26
	bld r26, 0

	mov r27, r10
	mov r28, r11

	rcall clockSetAlarm

testAlarm:
	tst r9
	breq clearAlarm ;If the alarm has been turned off, stop the beeping

	tst r14 ;Check if alarm is currently going off, if it is then beep
	brne loud

	tst r6 ;Check if the current time is the same as the alarm time, if it is then trigger the alarm
	brne display

	cp r8, r11
	brne display

	cp r7, r10
	brne display

setAlarm:
	ser r16
	mov r14, r16
	sbi ddrb, pb3
	rjmp loud

clearAlarm:
	clr r16
	mov r14, r16
	cbi ddrb, pb3
	rjmp display

loud:
	in r17, ddrb ;Toggles the output of the buzzer so it makes a cool beep
	mov r16, r17
	andi r17, ~(1 << pb3)

	com r16
	andi r16, 1 << pb3

	or r17, r16
	out ddrb, r17

display:
	in r13, pinb ;See if the alarm value or regular time should be displayed
	sbrs r13, pb4
	rjmp showTime

	sbis pinb, pb5 ;If the add time button is pressed then add time to the alarm
	rjmp showAlarm
	
addAlarmMinute:
	mov r22, r10 ;Unpack the BCD digits
	mov r23, r10

	andi r22, 0x0f
	andi r23, 0xf0
	swap r23

	mov r24, r11
	mov r25, r11

	andi r24, 0x0f
	andi r25, 0xf0
	swap r25

	ldi r16, 5
	add r22, r16
	cpi r22, 10

	brlo changeAlarmValue ;Check for ones overflow
	clr r22
	inc r23

	cpi r23, 6
	brlo changeAlarmValue ;Check for minutes overflow
	clr r23

	inc r24
	cpi r24, 4
	brlo addAlarmHour ;Check for hours overflow

	cpi r25, 2
	brlo addAlarmHour

	clr r24
	clr r25
	rjmp changeAlarmValue

addAlarmHour:
	cpi r24, 10

	brlo changeAlarmValue
	clr r24
	inc r25
	
changeAlarmValue:
	swap r23 ;Repack the digits and set the value in the clock
	clr r27

	or r27, r22
	or r27, r23

	swap r25
	clr r28

	or r28, r24
	or r28, r25

	rcall clockSetAlarm

showAlarm:
	rcall clockGetAlarm ;Output the alarm time onto the screen, as well as the state

	mov r16, r11
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r11
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite
	
	ldi r16, ':'
	rcall lcdWrite
	
	mov r16, r10
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r10
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite

	ldi r16, 0x4D | (1 << 7) ;Set the position of the cursor to the bottom right corner
	rcall lcdCommand

	ldi r16, 'O'
	rcall lcdWrite

	tst r9
	breq writeOff
	
	ldi r16, 'n'
	rcall lcdWrite
	
	ret

writeOff:
	ldi r16, 'f'
	rcall lcdWrite
	ldi r16, 'f'
	rcall lcdWrite

	ret

showTime:
	sbis pinb, pb5 ;Loop to show the regular time on the screen
	rjmp showShowTime
	
addTimeMinute:
	mov r22, r7 ;Same loop for adding time as for the alarm, but with a lot of different registers and subroutines
	mov r23, r7

	andi r22, 0x0f
	andi r23, 0xf0
	swap r23

	mov r24, r8
	mov r25, r8

	andi r24, 0x0f
	andi r25, 0xf0
	swap r25

	inc r22
	cpi r22, 10

	brlo changeTimeValue
	clr r22
	inc r23

	cpi r23, 6
	brlo changeTimeValue
	clr r23

	inc r24
	cpi r24, 4
	brlo addTimeHour

	cpi r25, 2
	brlo addTimeHour

	clr r24
	clr r25
	rjmp changeTimeValue

addTimeHour:
	cpi r24, 10

	brlo changeTimeValue
	clr r24
	inc r25
	
changeTimeValue:
	swap r23
	clr r27

	or r27, r22
	or r27, r23

	swap r25
	clr r28

	or r28, r24
	or r28, r25

	mov r26, r6

	rcall clockSetTime

showShowTime:
	rcall clockGetTime ;Output the regular time and date onto the screen in the same way as the alarm

	mov r16, r8
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r8
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite
	
	ldi r16, ':'
	rcall lcdWrite
	
	mov r16, r7
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r7
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite
	
	ldi r16, ':'
	rcall lcdWrite
	
	mov r16, r6
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r6
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite

	ldi r16, 0x48 | (1 << 7)
	rcall lcdCommand

	rcall clockGetDate
	
	mov r16, r7
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r7
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite
	
	ldi r16, '/'
	rcall lcdWrite
	
	mov r16, r6
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r6
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite
	
	ldi r16, '/'
	rcall lcdWrite
	
	mov r16, r8
	andi r16, 0xf0
	swap r16
	rcall numToAscii
	rcall lcdWrite
	
	mov r16, r8
	andi r16, 0x0f
	rcall numToAscii
	rcall lcdWrite
	
	ret