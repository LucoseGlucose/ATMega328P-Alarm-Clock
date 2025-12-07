
.ifndef TWI
TWI:

.equ twiW = 0
.equ twiR = 1

;Modifies: R17
twiWait:
	lds r17, twcr
	sbrs r17, twint
	rjmp twiWait
	ret

;Modifies: R17
twiTransmitStart:
	ldi r17, (1 << twint) | (1 << twsta) | (1 << twen)
	sts twcr, r17

	rcall twiWait
	ret
	
;Expects: R16=SLA+W/R - Modifies: R17
twiTransmitSLA:
	sts twdr, r16
	ldi r17, (1 << twint) | (1 << twen)
	sts twcr, r17

	rcall twiWait
	ret
	
;Expects: R16=Data - Modifies: R17
twiTransmitData:
	sts twdr, r16
	ldi r17, (1 << twint) | (1 << twen)
	sts twcr, r17

	rcall twiWait

	ret
	
;Modifies: R17
twiTransmitStop:
	ldi r17, (1 << twint) | (1 << twsto) | (1 << twen)
	sts twcr, r17
	ret
	
;Expects: R17=N/ACK - Returns: R16->Data - Modifies: R17
twiReceiveData:
	ori r17, (1 << twint) | (1 << twen)
	sts twcr, r17

	rcall twiWait
	lds r16, twdr
	ret

.endif