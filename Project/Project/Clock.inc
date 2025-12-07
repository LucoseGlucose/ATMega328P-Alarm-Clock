
.ifndef clock
clock:

.include "TWI.inc"
.equ clockSLA = 0b11010000

.equ seconds = 45
.equ minutes = 23
.equ hours = 12

;Modifies: R16, R17, R31
clockInit:
	ldi r16, 72
	sts twbr, r16

	clr r16
	sts twsr, r16

	sbi portc, pc4
	sbi portc, pc5

	rcall clockGetTime

	clr r31
	rcall clockSetRegister

	mov r16, r6
	andi r16, 0b01111111
	rcall twiTransmitData

	ldi r31, 0x7
	rcall clockSetRegister

	ldi r16, 0b10010000
	rcall twiTransmitData

	rcall twiTransmitStop
	ret
	
;Expects: R31=Register Address - Modifies: R16
clockSetRegister:
	rcall twiTransmitStart

	ldi r16, clockSLA | twiW
	rcall twiTransmitSLA
	
	mov r16, r31
	rcall twiTransmitData
	ret
	
;Returns: R6->SecondsBCD, R7->MinutesBCD, R8->HoursBCD - Modifies: R16, R17, R31
clockGetTime:
	clr r31
	rcall clockSetRegister

	rcall twiTransmitStart

	ldi r16, clockSLA | twiR
	rcall twiTransmitSLA

	ldi r17, 1 << twea
	rcall twiReceiveData
	mov r6, r16

	rcall twiReceiveData
	mov r7, r16

	clr r17
	rcall twiReceiveData
	mov r8, r16

	rcall twiTransmitStop
	ret
	
;Returns: R6->DateBCD, R7->MonthBCD, R8->YearsSince2000BCD - Modifies: R16, R17, R31
clockGetDate:
	ldi r31, 0x4
	rcall clockSetRegister

	rcall twiTransmitStart

	ldi r16, clockSLA | twiR
	rcall twiTransmitSLA

	ldi r17, 1 << twea
	rcall twiReceiveData
	mov r6, r16

	rcall twiReceiveData
	mov r7, r16

	clr r17
	rcall twiReceiveData
	mov r8, r16

	rcall twiTransmitStop
	ret
	
;Expects: R26->SecondsBCD, R27->MinutesBCD, R28->HoursBCD - Modifies: R16, R17, R31
clockSetTime:
	clr r31
	rcall clockSetRegister

	ori r26, 0b10000000
	mov r16, r26
	rcall twiTransmitData

	mov r16, r27
	rcall twiTransmitData

	mov r16, r28
	rcall twiTransmitData

	rcall twiTransmitStop
	ret
	
;Expects: R26->DateBCD, R27->MonthBCD, R28->YearsSince2000BCD - Modifies: R16, R17, R31
clockSetDate:
	ldi r31, 0x4
	rcall clockSetRegister

	mov r16, r26
	rcall twiTransmitData

	mov r16, r27
	rcall twiTransmitData

	mov r16, r28
	rcall twiTransmitData

	rcall twiTransmitStop
	ret
	
;Returns: R9->SecondsBCD, R10->MinutesBCD, R11->HoursBCD - Modifies: R16, R17, R31
clockGetAlarm:
	ldi r31, 0x8
	rcall clockSetRegister

	rcall twiTransmitStart

	ldi r16, clockSLA | twiR
	rcall twiTransmitSLA

	ldi r17, 1 << twea
	rcall twiReceiveData
	mov r9, r16

	rcall twiReceiveData
	mov r10, r16

	clr r17
	rcall twiReceiveData
	mov r11, r16

	rcall twiTransmitStop
	ret
	
;Expects: R26->AlarmOnOff, R27->MinuteBCD, R28->HourBCD - Modifies: R16, R17, R31
clockSetAlarm:
	ldi r31, 0x8
	rcall clockSetRegister

	mov r16, r26
	rcall twiTransmitData

	mov r16, r27
	rcall twiTransmitData

	mov r16, r28
	rcall twiTransmitData

	rcall twiTransmitStop
	ret

.endif