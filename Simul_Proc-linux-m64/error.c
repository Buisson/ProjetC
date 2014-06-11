#include <stdlib.h>
#include <stdio.h>
#include "error.h"

//liste de errors
const char *error_names[] =
{
    "NOERRROR",
    "UNKNOWN",
    "ILLEGAL",
    "CONDITION",
    "IMMEDIATE",
    "SEGTEXT",
    "SEGDATA",
    "SEGSTACK",
};

//nom de warning
const char *warning_names[] =
{
    "HALT",
}; 

//fonction warning
void warning(Warning warn, unsigned addr){
	 fprintf(stderr, "WARNING: %s reached at address 0x%x\n",
          warning_names[warn], addr);
}

//fonction error
void error(Error err, unsigned addr){
    fprintf(stderr, "ERROR: %s at address 0x%x\n",
            error_names[err], addr);
     exit(1);
}
