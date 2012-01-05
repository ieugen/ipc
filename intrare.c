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
#include "include/intrare.h"
#include "include/erori.h"

extern long pozitie_in_fisier;
extern long numar_de_linii;
extern long marime_fisier;
extern char* sursa;
extern int sfarsit_de_fisier;

char next_char ()
{
  if (sursa[pozitie_in_fisier] == '\n') numar_de_linii++;
  if ( pozitie_in_fisier == marime_fisier ) sfarsit_de_fisier = 1;
  pozitie_in_fisier++;
  return sursa[pozitie_in_fisier-1];
}

char curent_char ()
{
  if (pozitie_in_fisier == 0) return sursa[pozitie_in_fisier];
  else return sursa [pozitie_in_fisier-1];
}

char view_next_char ()
{
  return sursa[pozitie_in_fisier];
}
