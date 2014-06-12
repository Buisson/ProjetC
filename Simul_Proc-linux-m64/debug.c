#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "machine.h"

bool debug_ask(Machine *pmach){
	printf("DEBUG? ");
	char commande = '\n';
	int i = 0;
	char rep;
	while( (rep=getchar()) != '\n' && rep != EOF){
		if(i==0) commande = rep;
		i++;
	};
	//printf("%c",rep);
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
