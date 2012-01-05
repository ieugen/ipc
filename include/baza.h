/*
 *  ipc - interpretor/compilator			  
 *  copyright (c) 2004-2010 stan ioan-eugen				  
 *  								  
 *  e-mail: stan.ieugen@gmail.com 				  			  
 *     Acest program e gratuit ; il puteti redistribui si / sau modi
 * fica sub conditiile si termenii Licentei Publice Generale GNU 
 * (GNU General Public License) versiunea 2 sau  orice alta versiune 
 * ulterioara.
 *     Acest program e distribuit in speranta ca va fi folositor,
 * dar e distribuit FARA NICI O GARANTIE, implicita sau explicita.
 *     Cititi GNUGPL, General Public License pentru detalii .
 */

#ifdef IPC_WIN32
   #include <process.h>
#else
   #include <unistd.h>
#endif

#ifndef _BAZA_H_

#define _BAZA_H_

#define MARIME_IDENTIFICATOR 32
#define NUMAR_DE_CUVINTE_CHEIE 18
#define NUMAR_DE_OPERATORI     19
#define NUMAR_MAX_DE_VARIABILE 100
#define NUMAR_NIVELE_PRECEDENTA 13

#define TRUE        1
#define FALSE       0
#define ASOCIATIV_LA_DREAPTA 0
#define ASOCIATIV_LA_STANGA  1

#define COD_NUME       100
#define COD_NUMAR      200
#define COD_OPERATOR   300

#define TIP_CARACTER  1000
#define TIP_INTREG    1001
#define TIP_REAL      1002
#define TIP_SUBRUTINA 1003

#define LEXIC const char *lexic[NUMAR_DE_CUVINTE_CHEIE]={"caracter","intreg","real","sub","sfsub","daca","altfel","sfdaca","pentru","sfpentru","ctimp","sfctimp","repeta","pana","citeste","scrie"};

#define COD_CARACTER  1
#define COD_INTREG    2
#define COD_REAL      3
#define COD_SUB       4
#define COD_SFSUB     5
#define COD_DACA      6
#define COD_ALTFEL    7
#define COD_SFDACA    8
#define COD_PENTRU    9
#define COD_SFPENTRU 10
#define COD_CTIMP    11
#define COD_SFCTIMP  12
#define COD_REPETA   13
#define COD_PANA     14
#define COD_CITESTE  15
#define COD_SCRIE    16

#define OP_INCREMENTARE  300 // ++
#define OP_DECREMENTARE  301 // --
#define OP_DEPLASARE_ST  302 // <<
#define OP_DEPLASARE_DR  303 // >>
#define OP_MAI_MIC_EGAL  304 // <=
#define OP_MAI_MARE_EGAL 305 // >=
#define OP_DIFERIT       306 // !=
#define OP_EGALITATE     307 // == 
#define OP_SI_LOGIC      308 // &&
#define OP_SAU_LOGIC     309 // ||

#define OPERATOR_UNAR  5000
#define OPERATOR_BINAR 6000
#define OPERATOR_NECUNOSCUT 7000

struct t_atom
{
  char nume[MARIME_IDENTIFICATOR];
  int cod;
};
typedef struct t_atom ATOM;

#endif // se termina ifndef _BAZA_H

