/*
 * exec.c
 * 
 * Copyright 2014 user <kha@ynnn>
 * 
 */


#include <stdio.h>
#include "error.h"
#include "exec.h"

//!Effectue un ILLOP
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l intruction à exécuter
 * \return retourne rien car error ne retourne rien
 */
bool fonction_illop(Machine *pmach, Instruction instr){
		error(ERR_ILLEGAL, pmach->_pc-1);
}
//! Effectue un NOP sur la machine
 /*!
 * \param pmach la machine en cours d execution
 * \param instr l'instruction a exécuter
 * \return true
 */
bool fonction_nop(Machine *pmach, Instruction instr){
		return true;
}
//! Mets à jour CC
/*!
 * \param pmach la machine en cours d'éxécution
 * \param résultat le dernier résultat
 */ 
 
void set_cc(Machine *pmach, int resultat){
	if(resultat<0)
		pmach->_cc=CC_N;
	else if (resultat>0)
		pmach->_cc=CC_P;
	else
		pmach->_cc=CC_Z;
}

//! Appelle error si nous sommes en dehors du segment de données
/*!
 * \param pmach la machine en cours d'exécution
 * \param addr l'adresse de la donnée
 */
 void si_segdata_erreur(Machine *pmach, unsigned address){
	 if(address>= pmach->_datasize)
		error(ERR_SEGDATA, pmach->_pc-1);
 } 
 //! Appelle error si nous sommes en dehors du segment de texte
/*!
 * \param pmach la machine en cours d'exécution
 * \param addr l'adresse de la donnée
 */
 void si_segtext_erreur(Machine *pmach, unsigned address){
	 if(address>= pmach->_textsize)
		error(ERR_SEGTEXT, pmach->_pc-1);
 } 
 
 //! Calcul l'addresse d'une instruction
 /*!
  * \param pmach la machine en cours d'éxécution
  * \param instr l'instruction à exécuter
  * \return l'adresse absolu en absolu ou l'adresse indexée 
  */
unsigned get_address(Machine *pmach, Instruction instr){
	if(instr.instr_generic._indexed)
		return pmach->_registers[instr.instr_indexed._rindex]
			+instr.instr_indexed._offset;
	else
		return instr.instr_absolute._address;
}

//! Effectue un LOAD
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si le chargement a été fait.
 */
bool fonction_load(Machine *pmach, Instruction instr){
	unsigned registre_condition = instr.instr_generic._regcond;
	
	if(instr.instr_generic._immediate){
		pmach->_registers[registre_condition] = instr.instr_immediate._value;}
	else{
		unsigned address = get_address(pmach,instr);
		si_segdata_erreur(pmach,address);
		pmach->_registers[registre_condition] = pmach->_data[address];}
	
	set_cc(pmach,pmach->_registers[registre_condition]);
	return true;	
}

void si_immediat_erreur(Machine *pmach, Instruction instr){
	if(instr.instr_generic._immediate)
		error(ERR_IMMEDIATE, pmach->_pc -1);
}

//! Effectue un STORE
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si le store a été effectué
 */
bool fonction_store(Machine *pmach, Instruction instr){
	si_immediat_erreur(pmach,instr);
	unsigned registre_condition = instr.instr_generic._regcond;
	unsigned address = get_address(pmach,instr);
	si_segdata_erreur(pmach,address);
	
	pmach->_data[address] = pmach->_registers[registre_condition];
	return true;
}

//! Effectue un ADD
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si l'ADD a été effectué
 */
bool fonction_add(Machine *pmach, Instruction instr){
	unsigned registre_condition = instr.instr_generic._regcond;
	
	if(instr.instr_generic._immediate)
		pmach->_registers[registre_condition]+= instr.instr_immediate._value;
	else{
		unsigned address= get_address(pmach,instr);
		si_segdata_erreur(pmach,address);
		pmach->_registers[registre_condition] += pmach->_data[address];}
	set_cc(pmach, pmach->_registers[registre_condition]);
	return true;
	
}
//! Effectue un SUB
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si le SUB a été effectué
 */
bool fonction_sub(Machine *pmach, Instruction instr){
		unsigned registre_condition = instr.instr_generic._regcond;
	
	if(instr.instr_generic._immediate)
		pmach->_registers[registre_condition]-= instr.instr_immediate._value;
	else{
		unsigned address= get_address(pmach,instr);
		si_segdata_erreur(pmach,address);
		pmach->_registers[registre_condition] -= pmach->_data[address];}
	
	set_cc(pmach, pmach->_registers[registre_condition]);
	return true;
	
}

//! Retourne vrai, si l'on doit sauter false sinon
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si on doit sauter, false sinon, ne retourne pas si erreur
 */
bool jump(Machine *pmach, Instruction instr)
{
    if((instr.instr_generic._regcond != NC && pmach->_cc == CC_U) ||
     instr.instr_generic._regcond > LAST_CONDITION)
        error(ERR_CONDITION, pmach->_pc - 1);   
      
    switch(instr.instr_generic._regcond){
        case NC:
            return true;
        case EQ:
            return pmach->_cc == CC_Z;
        case NE:
            return pmach->_cc != CC_Z;
        case GT:
            return pmach->_cc == CC_P;
        case GE:
            return pmach->_cc == CC_P || pmach->_cc == CC_Z;
        case LT:
            return pmach->_cc == CC_N;
        case LE:
            return pmach->_cc == CC_N || pmach->_cc == CC_Z;
        default:
            error(ERR_CONDITION, pmach->_pc - 1);
    }
}
//! Effectue un BRANCH
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si le BRANCH a été effectué
 */
bool fonction_branch(Machine *pmach, Instruction instr){
	si_immediat_erreur(pmach,instr);
	if(jump(pmach,instr)){
		unsigned address= get_address(pmach,instr);
		si_segtext_erreur_erreur(pmach,address);
		pmach->_pc = address;
	}
	return true;
}
//! Appelle error lorsqu'on sort de la pile
/*!
 * \param pmach la machine en cours d'exécution
 */
void si_segstack_erreur(Machine *pmach){
	 if(pmach->_sp < 0 || pmach->_sp >= pmach->_datasize)
        error(ERR_SEGSTACK, pmach->_pc - 1);
}
//!Effectue un CALL
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si CALL a été effectué
 */
bool fonction_call(Machine *pmach, Instruction instr){
	si_immediat_erreur(pmach,instr);
	if(jump(pmach,instr)){
		si_segstack_erreur(pmach);
		pmach->_data[pmach->_sp] = pmach-> _pc;
		unsigned address = get_address(pmach,instr);
		si_segtext_erreur_erreur(pmach,address);
		pmach->_pc = address;
		--pmach->_sp;
	}
	return true;
}

//! Effectue un RET
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si RET a été effectué
 */
bool fonction_ret(Machine *pmach, Instruction instr){
	 si_segtext_erreur(pmach,++pmach->_sp);
	 si_segstack_erreur(pmach);
	 pmach->_pc = pmach ->_data[pmach->_sp];
	 return true;
 }

//!Effectue un PUSH
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si PUSH a été effectué
 */
bool fonction_push(Machine *pmach, Instruction instr){
	si_segstack_erreur(pmach);
	
	if(instr.instr_generic._immediate)
		pmach->_data[pmach->_sp] = instr.instr_immediate._value;
	else{
		unsigned address = get_address(pmach,instr);
		si_segdata_erreur(pmach,address);
		pmach->_data[pmach->_sp] = pmach->_data[address];}
	--pmach->_sp;
	return true;
}

//!Effectue un POP
/*!
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si POP a été effectué
 */
bool fonction_pop(Machine *pmach, Instruction instr){
	++pmach->_sp;
	si_immediat_erreur(pmach,instr);
	si_segstack_erreur(pmach);
	unsigned address = get_address(pmach,instr);
	si_segdata_erreur(pmach,address);
	
	if(address< pmach->_datasize)
		pmach->_data[address] = pmach->_data[pmach->_sp];
	return true;
}

bool fonction_halt(Machine *pmach, Instruction instr){
	warning(HALT, pmach->_pc - 1);
	return false;
}
//!
//! Décodage et exécution d'une instruction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux après l'exécution de \c HALT ; vrai sinon
 */
bool decode_execute(Machine *pmach, Instruction instr){
	
	switch (instr.instr_generic._cop){
	case ILLOP:
		return fonction_illop(pmach, instr);
	case NOP:
		return fonction_nop(pmach, instr);
    case LOAD:
		return fonction_load(pmach, instr);
    case STORE:
		return fonction_store(pmach, instr);
    case ADD:
		return fonction_add(pmach, instr);
    case SUB:
		return fonction_sub(pmach, instr);
    case BRANCH:
		return fonction_branch(pmach, instr);
    case CALL:
		return fonction_call(pmach, instr);
    case RET:
		return fonction_ret(pmach, instr);
    case PUSH:
		return fonction_push(pmach, instr);
    case POP:
		return fonction_pop(pmach, instr);
    case HALT:
		return fonction_halt(pmach, instr);
    default:
		error(ERR_UNKNOWN, pmach->_pc-1);
	}
}

//! Trace de l'exécution
/*!
 * On écrit l'adresse et l'instruction sous forme lisible.
 *
 * \param msg le message de trace
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \param addr son adresse
 */
void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr){
	printf("TRACE : %s: 0x%04x: ", msg, addr);
	print_instruction(instr, addr);
	printf("\n");
}
