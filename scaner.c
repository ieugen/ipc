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

#include "include/baza.h"
#include "include/intrare.h"
#include "include/erori.h"
#include "include/scaner.h"

extern long pozitie_in_fisier;
extern long marime_fisier;
extern long numar_de_linii;
extern ATOM atom;
extern ATOM atom_vechi;
extern char* sursa;

void muta()
{
  strcpy(atom_vechi.nume,atom.nume);
  atom_vechi.cod = atom.cod;
}

/* sare peste spatii si comentarii stil C++ */
void sari_spatiu()
{
  char c = curent_char();
  do
    {
      while ( isspace (c) ) c = next_char();
      if ( c == '/' )
	{
	  c = view_next_char();
	  if ( c == '/' )
	    {
	      while ( c!='\n') c = next_char();
	      c = next_char();
	    }
	  else break;
	}
    }while( ( isspace(c) || curent_char() == '/')
	    && pozitie_in_fisier <= marime_fisier);
}

int isoperator (char c)
{
  return ( c==','|| c=='|' || c=='6'|| c=='^' || c=='='|| c== '!'|| c=='<' 
	   || c== '>' || c== '+' || c== '-' || c=='%' || c=='/' 
	   || c=='*' || c== '~' || c=='(' || c==')' || c=='[' || c== ']' 
	   || c=='.' || ';' || c=='\'' || c=='"');
}

void get_name()
{
  int i;
  muta();
  if ( isalpha( curent_char() ) == 0 )
      eroare ("nume de identificator asteptat");
  atom.cod = COD_NUME;
  i = 0;
  do
    {
      atom.nume[i] = curent_char();
      next_char();
      i++;
    }
  while ( isalnum(curent_char())  && i < MARIME_IDENTIFICATOR);
  atom.nume[i]='\0';
  if ( i == MARIME_IDENTIFICATOR && isalnum(curent_char()) )
      eroare ("nume de identificator prea lung");
}

void get_number()
{
  int i;
  muta();
  if ( isxdigit ( curent_char() ) == 0)
      eroare ("numar asteptat");
  atom.cod = COD_NUMAR;
  i = 0;
  do
    {
      atom.nume [i] = curent_char();
      next_char();
      i++;
    }
  while ( isxdigit(curent_char()) && i < MARIME_IDENTIFICATOR);
  atom.nume[i]='\0';
  if ( i == MARIME_IDENTIFICATOR && isxdigit(curent_char()) )
      eroare("numar prea mare (lung al dracu)");
}

void get_operator()
{
  muta();
  if ( isoperator(curent_char()) == FALSE )
      eroare("operator asteptat");
  atom.nume[0] = curent_char();
  atom.nume[1] = '\0';
  atom.nume[2] = '\0';

  atom.cod = COD_OPERATOR; 
  switch (atom.nume[0])
    {
    case '+':
      // daca urmatorul caracter e '+' -> '++' 
      if ( view_next_char() == '+')
	atom.nume[1] = next_char();
      break;
    case '-':
      // daca e '-' -> '--' 
      if (  view_next_char() ==  '-' )
	atom.nume[1] = next_char();
      break;
    case '>':
      // daca e '>' -> '>>' , daca e '=' -> '>='
      if ( view_next_char() == '>' || view_next_char() == '=')
	atom.nume[1] = next_char();
      break;
    case '<':
      // daca urm. e '<' -> '<<', daca e '=' -> '<='
      if ( view_next_char() == '<' || view_next_char() == '=')
	atom.nume[1] = next_char();
      break;
    case '=':
      // daca e '=' -> '=='
      if ( view_next_char() == '=')
	atom.nume[1] = next_char();
      break;
    case '!':
      // deca e '=' -> '!='
      if ( view_next_char() == '=')
	atom.nume[1] = next_char();
      break;
    case '&':
      // daca e '&' -> '&&'
      if ( view_next_char() == '&')
	atom.nume[1] = next_char();
      break;
    case '|':
      // daca e '|' -> '||'
      if ( view_next_char() == '|')
	atom.nume[1] = next_char();
      break;
    }
  next_char();
}

void get_atom()
{
  sari_spatiu();
  if ( isalpha( curent_char()) )
    get_name();
  else  if ( isdigit( curent_char()) )
    get_number();
  else  if ( isoperator(curent_char()) )
    get_operator();
  else eroare("caracter neasteptat");
  atom.cod = scan_cod();
}

void get_it(const char *valoare)
{
  get_atom();
  if ( strcmp(atom.nume,valoare) != 0)	
      eroare("valoarea asteptata nu a fost gasita");
}

int scan_cod()
{
  int i = 0;
  char **tmp;
  char *lexic[NUMAR_DE_CUVINTE_CHEIE]={"caracter","intreg","real","sub","sfsub","daca","altfel","sfdaca","pentru","sfpentru","ctimp","sfctimp","repeta","pana","citeste","scrie"};

  tmp = lexic;
  i = 0;
  if (atom.cod == COD_NUME) 	
    {
      while ( *tmp != NULL )
	{
	  if ( strcmp(atom.nume,*tmp) == 0 ) return i+1;
	  tmp++;
	  i++;
	}
      return COD_NUME;
    }
  if (atom.cod == COD_OPERATOR)
    {
      if ( atom.nume[1]=='\0') //operatorul are un sg. caracter ?
	{
	  switch (atom.nume[0])
	    {
	    case '+':  
	    case '-': 
	    case '/':
	    case '*': 
	    case ',': 
	    case ';': 
	    case '=':
	    case '!':
	    case '\'':
	    case '"':
	    case '|':
	    case '&':
	    case '^':
	    case '<':
	    case '>':
	    case '%': 
	    case '~':
	    case '(': 
	    case ')':
	    case '[':
	    case ']': 
	    case '.':
	      return atom.nume[0];
	      break;
	    default:
	      eroare("cod atom eronat");
	      break;
	    }
	}
      else 
	{
	  // avem un operator dublu
	  if ( atom.nume[0] == '+' ) return OP_INCREMENTARE;
	  if ( atom.nume[0] == '-' ) return OP_DECREMENTARE;
	  if ( atom.nume[0] == '<' )
	    {
	      if ( atom.nume[1] == '<'  ) return OP_DEPLASARE_ST;
	      if ( atom.nume[1] == '='  ) return OP_MAI_MIC_EGAL;
	    }
	  if ( atom.nume[0] == '>' )
	    {
	      if ( atom.nume[1] == '>' ) return OP_DEPLASARE_DR;
	      if ( atom.nume[1] == '=' ) return OP_MAI_MARE_EGAL;
	    }
	  if ( atom.nume[0] == '&' ) return OP_SI_LOGIC;
	  if ( atom.nume[0] == '|' ) return OP_SAU_LOGIC;
	  if ( atom.nume[0] == '=' ) return OP_EGALITATE;
	  if ( atom.nume[0] == '!' ) return OP_DIFERIT;
	}
    }
  return COD_NUMAR;
}
