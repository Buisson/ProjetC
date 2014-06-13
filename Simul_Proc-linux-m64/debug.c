#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "machine.h"

/*!
 * \file debug.c
 * \brief Fonctions de mise au point interactive.
 */

//! Dialogue de mise au point interactive pour l'instruction courante.
/*!
 * Cette fonction gère le dialogue pour l'option \c -d (debug). Dans ce mode,
 * elle est invoquée après l'exécution de chaque instruction.  Elle affiche le
 * menu de mise au point et on exécute le choix de l'utilisateur. Si cette
 * fonction retourne faux, on abandonne le mode de mise au point interactive
 * pour les instructions suivantes et jusqu'à la fin du programme.
 * 
 * \param mach la machine/programme en cours de simulation
 * \return vrai si l'on doit continuer en mode debug, faux sinon
 */
bool debug_ask(Machine *pmach){
	printf("DEBUG? ");
	char commande = '\n';
	bool premier = true;
	char rep;
	while( (rep=getchar()) != '\n' && rep != EOF){
		if(premier) commande = rep;
		premier = false;
	};
	switch(commande){
	  case 'h':
		printf("Available commands :\n\th\thelp\n\tc\tcontinue (exit interactive debug mode)\n\ts\tstep by step (next instruction)\n\tRET\tstep by step (next instruction)\n\tr\tprint registers\n\td\tprint data memory\n\tt\tprint text (program) memory\n\tp\tprint text (program) memory\n\tm\tprint registers and data memory\n");
          break;
	  case 'c':
		return false;
	  break;
          case 's':
		return true;
          break;
	  case 'r':
		print_cpu(pmach);
          break;
          case 'd':
		print_data(pmach);
          break;
          case 't':
		print_program(pmach);
          break;
	  case 'p':
		print_program(pmach);
          break;
	  case 'm':
		print_cpu(pmach);
		print_data(pmach);
          break;
	  case '\n':
		return true;
	  break;
	  default:
          break;
	}
	return debug_ask(pmach);
}
