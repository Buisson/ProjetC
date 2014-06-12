//-----------------
// Instructions
//-----------------
TEXT 30
// Programme principal
main EQU *

	CALL EQ, @coucou 

	PUSH @op1
	ADD R00,#10
	coucou SUB R00,#1
	
	
	
	ADD R15, #2
	
	HALT
	//STORE R00,#0
	NOP
	NOP
	NOP
	NOP

	END
//-----------------
// Données et pile
//-----------------
	DATA 30
	WORD 0
	result WORD 0
	op1 WORD 20
	op2 WORD 5
END
