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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "include/intrare.h"
#include "include/scaner.h"
#include "include/parser.h"
#include "include/baza.h"


long marime_fisier = 0;
long pozitie_in_fisier = 0;
long numar_de_linii = 1;
long numar_de_variabile = 0;
char* sursa;
int sfarsit_de_fisier = 0;
ATOM atom,atom_vechi;
arbore *tree;
// definit in parser.h; sa fac alocare dinamica
variabila_t variabile[NUMAR_MAX_DE_VARIABILE]; 

/*    LEXIC =  macro care se expandeaza la un sir de siruri de 
  caractere  si care reprezinta cuvintele cheie ale limbajului
*/
LEXIC ;

int main (int argc, char *argv[])
{
  FILE *fin;
  long i = 0;

  if (argc == 1)
    {
      printf ("Mod de utilizare : %s file_name\n",argv[0]);
      exit (0);
    }
  if ((fin = fopen (argv[1], "r")) == NULL)
    {
      fprintf (stderr, "\nEroare la deschiderea fisierului %s\n",argv[1]);
      exit (1);
    }
  marime_fisier = 0;
  fseek (fin, 0, SEEK_END);  // calculam marimea fisierului
  marime_fisier = ftell (fin);  // daca e vid inseamna ca a aparut o eroare
  
  if (marime_fisier == 0)
    {
      fprintf (stderr, "\nFisier vid sau eroare de citire\n");
      exit (1);
    }
  fseek (fin, 0, SEEK_SET);
  if ((sursa = (char *) malloc (marime_fisier * sizeof (char))) == NULL)
    {
      fprintf(stderr,"\nEroare la alocarea de memorie pentru incarcarea fisierului\n");
      exit (1);
    }
  i = 0;
  while (!feof (fin))    // citim fisierul in sursa
      sursa[i++] = fgetc (fin);
  sursa[marime_fisier] = '\0';
  pozitie_in_fisier = 0;

  main_parse();
  return 0;
}
