#include <stdio.h>
#include "instruction.h"
#include "error.h"
const char *cop_names[] =
{
    "ILLOP",
    "NOP",
    "LOAD",
    "STORE",
    "ADD",
    "SUB",
    "BRANCH",
    "CALL",
    "RET",
    "PUSH",
    "POP",
    "HALT",
};

const char *condition_names[] =
{
    "NC",
    "EQ",
    "NE",
    "GT",
    "GE",
    "LT",
    "LE",
};

void print_instruction(Instruction instr, unsigned addr){
	Code_Op op = instr.instr_generic._cop;
//Instruction inconnue, error .
	if(op>LAST_COP){
        error(ERR_UNKNOWN,addr);
	}
//Affiche le nom de op tout d'abord
    printf("%s ",cop_names[op]);
//Instruction faut juste afficher le nom
    if(op==ILLOP||op==NOP||op==RET||op==HALT)
    	return;
//Instruction faut juste address absolut
    if(op==PUSH||op==POP){
        printf("@0x%04x", instr.instr_absolute._address);
        return;
    }
//RC est condition
    if(op==BRANCH||op==CALL){
        // Condition inconnue
        if(instr.instr_generic._regcond > LAST_CONDITION)
            error(ERR_CONDITION, addr);
        printf("%s, ", condition_names[instr.instr_generic._regcond]);
    }
//RC est numéro de registre pour ADD,STORE,ADD et SUB
    else{
         printf("R%02u, ", instr.instr_generic._regcond);
    }
//affiche le dernier champ 
    //Valeur immédiate 
     if(instr.instr_generic._immediate)
        printf("#%u", instr.instr_immediate._value);  
    //Adressage indirect
     else if(instr.instr_generic._indexed)
        printf("%d[R%02u]", instr.instr_indexed._offset,
                instr.instr_indexed._rindex);  
    //Adressage direct
        else
             printf("@0x%04x", instr.instr_absolute._address);

}