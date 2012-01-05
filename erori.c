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

#include "include/baza.h"
#include "include/erori.h"
#include "include/intrare.h"

extern long pozitie_in_fisier;
extern long numar_de_linii;
extern long marime_fisier;
extern ATOM atom_vechi;
extern ATOM atom;

void eroare (const char *mesaj_eroare)
{
  fprintf(stderr,"\n Eroare :(linia: %li, car. %li/%li ):  dupa `%s` , %s (%s)\n",
	numar_de_linii , pozitie_in_fisier, marime_fisier, atom_vechi.nume, mesaj_eroare,atom.nume  );
  exit (1);
}
