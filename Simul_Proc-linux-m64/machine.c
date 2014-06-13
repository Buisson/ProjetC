/*! 
 *  \brief     machine.c
 *  \details   Ce fichier implemente les fonctions de machine.h
 *  \author    Colombet Aurelien
 *  \date      2014
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "machine.h"
#include "debug.h"
#include "exec.h"
//!
//! Chargement d'un programme
/*!
 * La machine est réinitialisée et ses segments de texte et de données sont
 * remplacés par ceux fournis en paramètre.
 *
 * \param pmach la machine en cours d'exécution
 * \param textsize taille utile du segment de texte
 * \param text le contenu du segment de texte
 * \param datasize taille utile du segment de données
 * \param data le contenu initial du segment de texte
 */
void load_program(Machine *pmach,
unsigned textsize, Instruction text[textsize],
unsigned datasize, Word data[datasize], unsigned dataend) {
  /*Init de la machine*/
    pmach->_datasize=datasize;
    pmach->_dataend=dataend;
    pmach->_pc=0;
    pmach->_textsize=textsize;
    pmach->_text=text;
    pmach->_data=data;
    pmach->_cc=CC_U;
    
    int i;
    /*On remet les registres à 0*/
    for(i=0;i<NREGISTERS;i++){
        pmach->_registers[i]=0;
    }
    //(bon datasize-1 car SP a 19 dans l'execution)
    pmach->_sp=datasize - 1;   //On met le pointeur sp à datasize.(contient les données statiques dans les adresses hautes de la pile.)
}

//! Lecture d'un programme depuis un fichier binaire
/*!
 * Le fichier binaire a le format suivant :
 * 
 *    - 3 entiers non signés, la taille du segment de texte (\c textsize),
 *    celle du segment de données (\c datasize) et la première adresse libre de
 *    données (\c dataend) ;
 *
 *    - une suite de \c textsize entiers non signés représentant le contenu du
 *    segment de texte (les instructions) ;
 *
 *    - une suite de \c datasize entiers non signés représentant le contenu initial du
 *    segment de données.
 *
 * Tous les entiers font 32 bits et les adresses de chaque segment commencent à
 * 0. La fonction initialise complétement la machine.
 *
 * \param pmach la machine à simuler
 * \param programfile le nom du fichier binaire
 *
 */
void read_program(Machine *mach, const char *programfile) {
    unsigned int sizeText,sizeData,endData;
    int fd = open(programfile,O_RDONLY);
    /*Lecture des 3 premieres données*/
    read(fd,&sizeText,sizeof(unsigned int));
    read(fd,&sizeData,sizeof(unsigned int));
    read(fd,&endData,sizeof(unsigned int));
    
    int i;
    Word *data = malloc(sizeof(Word)*sizeData);
    Instruction *text = malloc(sizeof(Instruction)*sizeText);
    for(i=0;i<sizeText;i++) 
        read(fd,text+i,sizeof(int));
    for(i=0;i<endData;i++)
        read(fd,data+i,sizeof(int)); 
    close(fd);

    load_program(mach,sizeText,text,sizeData,data,endData);
}

//! Affichage du programme et des données
/*!
 * On affiche les instruction et les données en format hexadécimal, sous une
 * forme prête à être coupée-collée dans le simulateur.
 *
 * Pendant qu'on y est, on produit aussi un dump binaire dans le fichier
 * dump.prog. Le format de ce fichier est compatible avec l'option -b de
 * test_simul.
 *
 * \param pmach la machine en cours d'exécution
 */
void dump_memory(Machine *pmach) {
    
    int fd = open("dump.bin",O_CREAT|O_RDWR,0777);
    
    write(fd,&pmach->_textsize,sizeof(unsigned int));
    write(fd,&pmach->_datasize,sizeof(unsigned int));
    write(fd,&pmach->_dataend,sizeof(unsigned int));
    
    printf("Instruction text[] = {\n    ");
    int i,cnt=0;
    for(i=0;i<pmach->_textsize;i++){
        write(fd,&pmach->_text[i],sizeof(unsigned int));
        if(i!=pmach->_textsize-1){
            printf("0x%08x, ",pmach->_text[i]._raw);
        }else{
            printf("0x%08x, \n};",pmach->_text[i]._raw);
        }
        cnt++;
        if(cnt%4==0)
            printf("\n    ");
    }
    printf("\nunsigned textsize = %d;\n\n",pmach->_textsize);
    
    printf("Word data[] = {\n    ");
    cnt=0;
    for(i=0;i<pmach->_datasize;i++){
        write(fd,&pmach->_data[i],sizeof(unsigned int));
        if(i!=pmach->_datasize-1){
            printf("0x%08x, ",pmach->_data[i]);
        }else{
            printf("0x%08x, \n};",pmach->_data[i]);
        }
        cnt++;
        if(cnt%4==0)
            printf("\n    ");
    }
    printf("\nunsigned datasize = %d;\nunsigned dataend = %d;\n",pmach->_datasize,pmach->_dataend);   
}

//! Affichage des instructions du programme
/*!
 * Les instructions sont affichées sous forme symbolique, précédées de leur adresse.
.* 
 * \param pmach la machine en cours d'exécution
 */
void print_program(Machine *pmach) {
    int i;
    printf("\n*** PROGRAM (size: %d) ***\n", pmach->_textsize);
    for(i = 0; i < pmach->_textsize; i++) {
        printf("0x%04x: 0x%08x \t ", i, pmach->_text[i]._raw);  //%04x print in 4 width and pad with 0 before
        print_instruction(pmach->_text[i], i);
        printf("\n");
    }
}

//! Affichage des données du programme
/*!
 * Les valeurs sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_data(Machine *pmach) {
    int i,cnt=0;
    printf("\n*** DATA (size: %d, end = 0x%08x (%d)) ***\n",pmach->_datasize,pmach->_dataend,pmach->_dataend);
    for(i=0;i<pmach->_datasize;i++){
        printf("0x%04x: 0x%08x %d \t",i,pmach->_data[i],pmach->_data[i]);
        cnt++;       
        if(cnt%3==0 && cnt!=0)
            printf("\n");
        
    }
    printf("\n\n");
}

//
//! Affichage des registres du CPU
/*!
 * Les registres généraux sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_cpu(Machine *pmach) {
    printf("\n*** CPU ***\n");
    printf("PC:  0x%08x   CC: %c\n\n",pmach->_pc,(pmach->_cc == CC_U)?'U':(pmach->_cc == CC_Z)?'Z':(pmach->_cc == CC_P)?'P':'N');
    int i,cnt=0;
    for(i=0;i<NREGISTERS;i++){
        printf("R%02d: 0x%08x %d      ",i,pmach->_registers[i],pmach->_registers[i]);
        cnt++;
        if(cnt%3==0){
            printf("\n");
        }
    }
    printf("\n");    
}

//! Simulation
/*!
 * La boucle de simualtion est très simple : recherche de l'instruction
 * suivante (pointée par le compteur ordinal \c _pc) puis décodage et exécution
 * de l'instruction.
 *
 * \param pmach la machine en cours d'exécution
 * \param debug mode de mise au point (pas à apas) ?
 */
void simul(Machine *pmach, bool debug) {
    trace("Executing",pmach,pmach->_text[pmach->_pc],pmach->_pc);
    
    while(decode_execute(pmach, pmach->_text[pmach->_pc++]) && pmach->_pc<pmach->_textsize){
        if(debug) debug = debug_ask(pmach);
        trace("Executing",pmach,pmach->_text[pmach->_pc],pmach->_pc);
    }
}
