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

#include "include/scaner.h"
#include "include/intrare.h"
#include "include/erori.h"
#include "include/parser.h"
#include "include/baza.h"


extern long numar_de_linii;
extern long marime_fisier;
extern long pozitie_in_fisier;
extern long numar_de_variabile;
extern ATOM atom;
extern ATOM atom_vechi;
extern char* sursa;
extern int sfarsit_de_fisier;
// definit in parser.h; sa fac alocare dinamica
extern variabila_t variabile[NUMAR_MAX_DE_VARIABILE];  
arbore nod;  // arbore pentru rezolvarea expresiilor
long paranteze_deschise = 0;

void main_parse()
{
  get_atom();
  do
    {
      switch(atom.cod)
	{
	case COD_CITESTE:
	  do_citeste();
	  break;
	case COD_SCRIE:
	  do_scrie();
	  break;
	case COD_DACA:
	  do_daca();
	  break;
	case COD_PENTRU:
	  do_pentru();
	  break;
	case COD_REPETA:
	  do_repeta();
	  break;
	case COD_CTIMP:
	  do_ctimp();
	  break;
	case COD_SUB:
	  do_subrutina();
	  break;
	case COD_CARACTER:
	  declarare_var(TIP_CARACTER);
	  break;
	case COD_INTREG:
	  declarare_var(TIP_INTREG);
	  break;
	case COD_REAL:
	  declarare_var(TIP_REAL);
	  break;
	case '.':
	  sfarsit_de_fisier = 1; // s-a ajuns la sfarsitul programului
	  break;
	case COD_NUME: // atribuire (sau, cea mai incolo apel de functie)
	  // daca e funcie => apel functie
	  // daca nu => atribuire
	  do_atribuire();
	  break;
	default:
	  if (sfarsit_de_fisier == 1)
	    printf("\n s-a atins sfarsitul de fisier\n");
	  else
	    {
	      printf("\n caracterul %li din %li\n",pozitie_in_fisier,marime_fisier);
	      eroare("caracter/identificator nepermis");
	    }
	   break;
	}
    }
  while ( sfarsit_de_fisier == FALSE );
}

int cauta_identificator(const char* nume_var )
{
  int i = numar_de_variabile;
  while ( i > 0 )
    {
      if ( strcmp(nume_var,variabile[i].nume) == 0 ) return i;
      i--;
    }
  return -1;
}

void adauga_identificator(const char* nume_var,int tip_var,float val_var)
{
  int i = ++numar_de_variabile;
  if ( numar_de_variabile <= NUMAR_MAX_DE_VARIABILE )
    {
      strcpy(variabile [i].nume,nume_var);
      variabile [i].tip = tip_var;
      variabile [i].valoare_f = val_var;
    }
  else eroare("s-a atins numarul maxim de variabile");
}

void atribuie_identificator(const char* nume_var,float val_var)
{
  int i = cauta_identificator(nume_var);
  if ( i > 0 )
    // identificatorul exista
    variabile [i].valoare_f = val_var;
  else 
    eroare("identificatorul nu a fost gasit");
}

void declarare_var(int tip_xxx)
{
  //atom.nume == "intreg" sau "caracter" sau "real"
  do
    {
      get_atom();
      if (atom.cod != COD_NUME) 
	eroare("nume de identificator asteptat");
      if ( cauta_identificator(atom.nume) < 0 )	
	// nu exista deci il putem adaugam
	adauga_identificator(atom.nume,tip_xxx,0.0);
      get_atom();

      switch (atom.cod)
	{
	case '=':
	  get_atom();
	  // asteptam o constanta sau o variabila deja definita
	  if ( atom.cod == COD_NUMAR )
	    {
	      // daca e constanta facem atriguirea
	      variabile[numar_de_variabile].valoare_f = atof(atom.nume);
	    }
	  else 
	    if ( atom.cod == COD_NUME )
	      {
		// daca e nume de variabila cautam ... 
		int indice;
		if ( (indice=cauta_identificator(atom.nume))>0)
		  {
		    // daca am gasit-o luam valoarea
		    variabile[numar_de_variabile].valoare_f = 
		      variabile[indice].valoare_f;
		  }
		// daca nu iesim
		else eroare("variabila nedefinita");
	      }
	    else eroare("eroare de sintaxa,constanta asteptata");
	  get_atom();
	  if ( !(atom.cod == ',' || atom.cod == ';') )
	    eroare ("',' sau ';' astepati");
	  break;
	case ',':
	case ';':
	  break;
	default:
	  eroare ("eroare de sintaxa in declararea de variabile");
	}
    }
  while(atom.cod == ',');
  if ( atom.cod != ';' ) eroare ("lipseste ';'");
  get_atom();
}


void do_scrie()
{
  int indice = 0;
  do
    {
      get_atom();
      if (atom.cod == '"' )
	{
	  while (curent_char() != '"' ) 
	    {
	      if ( curent_char() == '\\' )
		{
		  switch (view_next_char())
		    {
		    case '\\':
		      next_char(); printf("\\");
		      break;
		    case 'n':
		      next_char(); printf("\n");
		      break;
		    case 't':
		      next_char(); printf("\t");
		      break;
		    case 'r':
		      next_char(); printf("\r");
		      break;
		    }
		}
	      else printf("%c",curent_char());
	      next_char();
	    }
	  next_char();// mancam apostroful de sfarsit
	}
      if (atom.cod == COD_NUME)
	{
	  // trebuie sa afisez valoarea variabilei
	  indice = cauta_identificator(atom.nume);
	  if ( indice == 0)
	      eroare("variabila nedefinita");
	  else 
	    {
	      switch (variabile[indice].tip)
		{
		case TIP_CARACTER:
		  printf("%c",(char)variabile[indice].valoare_f);
		  break;
		case TIP_INTREG:
		  printf("%i",(int)variabile[indice].valoare_f);
		  break;
		case TIP_REAL:
		  printf("%f",(float)variabile[indice].valoare_f);
		  break;
		default :
		  eroare(" tip nedefinit (o prosrie)");
		}
	      
	    }
	}
      get_atom(); // trebuie sa manance virgula
    }
  while (atom.cod == ',' );
  if ( atom.cod != ';' )
    eroare ("lipseste ';'");
  get_atom();
}

void do_citeste()
{
  int indice = -1;
  char var_caracter;
  int var_intreg;
  float var_real;
  
  do
    {
      get_atom();
      if ( atom.cod != COD_NUME )
	  eroare("nume de identificator asteptat");
      else
	{
	  indice = cauta_identificator(atom.nume);
	  if ( indice > 0 )
	    {
	      // variabila exista
	      switch (variabile[indice].tip)
		{
		case TIP_CARACTER:
		  scanf("%c",&var_caracter);
		  variabile[indice].valoare_f = var_caracter;
		  break;
		case TIP_INTREG:
		  scanf("%i",&var_intreg);
		  variabile[indice].valoare_f = var_intreg;
		  break;
		case TIP_REAL:
		  scanf("%f",&var_real);
		  variabile[indice].valoare_f = var_real;
		  break;
		default:
		  eroare ("tip nedefinit");
		  break;
		}
	    }
	}
      get_atom();
    }
  while (atom.cod == ',' );
  if (atom.cod != ';' )
    eroare("lipseste `;'");
  get_atom();
}

// **********************************************
//  incepe rutina pentru daca  
//
// **********************************************

void do_daca(void)
{
  float valoare_conditie = 0;

  get_atom();
  if ( atom.cod != '(' ) eroare("lipseste `('");
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ')' ) eroare("lipseste `)'");
  else get_atom();
  
  if ( valoare_conditie == 1 )
    {
      bloc_instructiuni();
      if ( atom.cod == COD_ALTFEL )
	{
	  get_atom();
	  sari_bloc();
	}
    }
  else  
    {
      sari_bloc();
      if ( atom.cod == COD_ALTFEL )
	get_atom();
      bloc_instructiuni();
    }
  if ( atom.cod != COD_SFDACA )
    eroare("declaratie incorecta, lipseste `sfdaca'");
  get_atom();
}

void bloc_instructiuni()
{
  int sfarsit_bloc = 0;
  do 
    {
      switch (atom.cod)
	{
	case COD_SCRIE:
	  do_scrie();
	  break;
	case COD_CITESTE:
	  do_citeste();
	  break;
	case COD_DACA:
	  do_daca();
	  break;
	case COD_PENTRU:
	  do_pentru();
	  break;
	case COD_REPETA:
	  do_repeta();
	  break;
	case COD_CTIMP:
	  do_ctimp();
	  break;
	case COD_SUB:
	  do_subrutina();
	  break;
	case COD_NUME:
	  // vedem daca e functie / atribuire
	  //deocamdata doar atribuire
	  do_atribuire();
	  break;
	default:
	  // inseamna ca s-a atins un cod pentru care trebuie sa iesim
	  sfarsit_bloc = 1;
	  break;
	} // sf_switch
      
    }  while ( ! sfarsit_bloc );

}

void sari_bloc()
{
  int sfarsit_bloc = 0 ;
   do 
    {
      switch(atom.cod)
	{
	case COD_SCRIE:
	  sari_scrie();
	  break;
	case COD_CITESTE:
	  sari_citeste();
	  break;
	case COD_DACA:
	  sari_daca();
	  break;
	case COD_PENTRU:
	  sari_pentru();
	  break;
	case COD_REPETA:
	  sari_repeta();
	  break;
	case COD_CTIMP:
	  sari_ctimp();
	  break;
	case COD_SUB:
	  sari_subrutina();
	  break;
	case COD_NUME:
	  // vedem daca e functie / atribuire
	  //deocamdata doar atribuire
	  sari_atribuire();
	  break;
	default:
	  // inseamna ca trebuie sa iesim
	  sfarsit_bloc = 1;
	  break;
	}
    } while ( !sfarsit_bloc );
}

void sari_scrie()
{
  int indice = 0;
  do
    {
      get_atom();
      if (atom.cod == '"' )
	{
	  while (curent_char() != '"' ) 
	    {
	      if ( curent_char() == '\\' )
		{
		  switch (view_next_char())
		    {
		    case '\\':
		      next_char();
		      break;
		    case 'n':
		      next_char();
		      break;
		    case 't':
		      next_char();
		      break;
		    case 'r':
		      next_char();
		      break;
		    }
		}
	      next_char();
	    }
	  next_char();// mancam apostroful de sfarsit
	}
      if (atom.cod == COD_NUME)
	{
	  // trebuie sa afisez valoarea variabilei
	  indice = cauta_identificator(atom.nume);
	  if ( indice == 0)
	      eroare("variabila nedefinita");
	  else 
	    {
	      switch (variabile[indice].tip)
		{
		case TIP_CARACTER:
		case TIP_INTREG:
		case TIP_REAL:
		  break;
		default :
		  eroare(" tip nedefinit (o prosrie)");
		  break;
		}
	    }
	}
      get_atom(); // trebuie sa manance virgula
    }
  while (atom.cod == ',' );
  if ( atom.cod != ';' )
    eroare ("lipseste ';'");
  get_atom();
}

void sari_citeste()
{
  int indice = -1;
  do
    {
      get_atom();
      if ( atom.cod != COD_NUME )
	  eroare("nume de identificator asteptat");
      else
	{
	  indice = cauta_identificator(atom.nume);
	  if ( indice > 0 )
	    {
	      // variabila exista
	      switch (variabile[indice].tip)
		{
		case TIP_CARACTER:
		case TIP_INTREG:
		case TIP_REAL:
		  break;
		default:
		  eroare ("tip nedefinit");
		  break;
		}
	    }
	}
      get_atom();
    }
  while (atom.cod == ',' );
  if (atom.cod != ';' )
    eroare("lipseste `;'");
  get_atom();
}

void sari_atribuire()
{
  int indice ;
  float tmp;
  indice = cauta_identificator(atom.nume);
  
  if ( indice >0 )
    {
      get_atom();
      if ( atom.cod != '=' )
	eroare("lipseste operatorul de atribuire `='");
      else 
	{
	  get_atom();
	  tmp = rezolva_exp();
	  if ( atom.cod != ';' )
	    eroare("lipseste `;'");
	  get_atom();
	}
    }
  else 
    eroare("variabila nedefinita");
}

void sari_daca()
{
 float valoare_conditie = 0;
  
  get_atom();
  if ( atom.cod != '(' )  
    eroare("lipseste `('");
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ')' )
    eroare("lipseste `)'");
  else get_atom();

  if ( valoare_conditie == 1 ) 
    {
      sari_bloc();
      if ( atom.cod == COD_ALTFEL )
	{
	  get_atom();
	  sari_bloc();
	}
    }
  else  
    {
      sari_bloc();
      if ( atom.cod == COD_ALTFEL )
	{
	  get_atom();
	  sari_bloc();
	}
    }
  if ( atom.cod != COD_SFDACA )
    eroare("lipseste `sfdaca', declaratie incorecta");
  get_atom();
}

// **********************************************
//  incepe parserul de expresii 
//
// **********************************************

float rezolva_exp()
{
  arbore *tree = NULL;
  if ( atom.cod == '(' )
    {
      paranteze_deschise = 1;
      get_atom();
    }
  if ( atom.cod == ';' || atom.cod == ')' ) eroare("expresia nu exista");
  tree = expresie(0);
  if ( tree == NULL )
    eroare("expresia nu exista");
  if ( atom.cod == ')' )
    paranteze_deschise --;
  return rezultat(tree);
}

arbore *mknod(ATOM atm,arbore *t1,arbore *t2)
{
  arbore *t;
  t = (arbore *) malloc (sizeof (arbore) );
  t->tip = atm.cod;
  strcpy(t->nume,atm.nume);
  t->vecin_st = t1;
  t->vecin_dr = t2;
  return t;
}

arbore *mkfrunza(ATOM atm)
{
  arbore *t;
  int indice = 0 ;
  t = (arbore *) malloc (sizeof (arbore) );
  t->tip = atm.cod;
  strcpy(t->nume,atm.nume);
  if ( atm.cod == COD_NUMAR )
    t->valoare = atoi(atm.nume);
  else 
    if ( atm.cod == COD_NUME)
      {
	indice = cauta_identificator(atom.nume);
	if ( indice > 0 )
	  t->valoare = variabile[indice].valoare_f;
      }
  t->vecin_st = NULL;
  t->vecin_dr = NULL;
  return t;
}

int precedenta(int codu)
{
  // aflam precedenta operatorului
  switch (codu)
    {
    case OP_INCREMENTARE:
    case OP_DECREMENTARE:
    case '~':
    case '!':
      return NUMAR_NIVELE_PRECEDENTA - 4;    
      break;
    case '*':
    case '/':
    case '%':
      return NUMAR_NIVELE_PRECEDENTA - 3;    
      break;
    case '+':
    case '-':
      return NUMAR_NIVELE_PRECEDENTA - 4;  
      break;
    case '<':
    case OP_MAI_MIC_EGAL:
    case '>':
    case OP_MAI_MARE_EGAL:
    case OP_EGALITATE:
    case OP_DIFERIT:
      return NUMAR_NIVELE_PRECEDENTA - 6; 
      break;
    case '&':
      return NUMAR_NIVELE_PRECEDENTA - 7;     
      break;
    case '^':
      return NUMAR_NIVELE_PRECEDENTA - 8;     
      break;
    case '|':
      return NUMAR_NIVELE_PRECEDENTA - 9;      
      break;
    case OP_SI_LOGIC:
      return NUMAR_NIVELE_PRECEDENTA - 10;     
      break;
    case OP_SAU_LOGIC:
      return NUMAR_NIVELE_PRECEDENTA - 11;     
      break;
      // trebuie sa modific nivelele 12 pt amandoua ?
    case '=':
      return NUMAR_NIVELE_PRECEDENTA - 12;   
      break;
    case ',':
      return NUMAR_NIVELE_PRECEDENTA - 13;    
      break;
    default:
      return -1;
      break;
    }
  return -1;
}

int e_op_binar(int codu)
{
  // aflam daca operatorul e binar
  switch (codu)
    {
    case '~':
    case '!':
    case OP_INCREMENTARE:
    case OP_DECREMENTARE:
      return OPERATOR_UNAR;
      break;
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case ',':
    case '|':
    case '&':
    case '^':
    case '=':
    case '<':
    case '>':
      return OPERATOR_BINAR;
      break;
    default:
      return OPERATOR_NECUNOSCUT;
      break;
    }
  return OPERATOR_NECUNOSCUT;
}

int asociativ(int codu)
{
  // aflam asociativitatea stnga/dreapta
  if ( e_op_binar(codu) == OPERATOR_UNAR && e_op_binar(atom_vechi.cod) )
    return ASOCIATIV_LA_DREAPTA;
  else  return ASOCIATIV_LA_STANGA ;
}

arbore *expresie(int nivel_precedenta)
{
  arbore *tmp0 = NULL ,*tmp1 = NULL;
  ATOM atom_tmp;

  tmp0 = termen();
  if (atom.cod == ';' ) return tmp0;
  get_atom();
  if ( atom.cod == '(' ) paranteze_deschise++;
  else 
    if ( atom.cod == ')' )
      {
	paranteze_deschise--;
	if (paranteze_deschise == 0 )  return tmp0;
      }
  while ( e_op_binar(atom.cod) && precedenta(atom.cod) >= nivel_precedenta  )
    {
      strcpy(atom_tmp.nume,atom.nume);
      atom_tmp.cod = atom.cod;
      get_atom();
      
      if ( asociativ (atom_tmp.cod) == ASOCIATIV_LA_DREAPTA )
	tmp1 = expresie( precedenta (atom_tmp.cod) );
      else 
	if (asociativ (atom_tmp.cod)  == ASOCIATIV_LA_STANGA )
	  tmp1 = expresie( precedenta (atom_tmp.cod) + 1 );
      tmp0 = mknod( atom_tmp, tmp0, tmp1 );
    }
  return tmp0;
}


arbore *termen()
{
  arbore *tmp;
  ATOM atom_tmp;

  if (atom.cod == COD_NUME ) return mkfrunza(atom);
  else 
    if ( atom.cod == COD_NUMAR )
      return mkfrunza(atom);
    else
      if ( e_op_binar(atom.cod) == OPERATOR_UNAR )
	{
	  strcpy(atom_tmp.nume,atom.nume);
	  atom_tmp.cod = atom.cod ; 
	  get_atom();
	  tmp = expresie(precedenta (atom_tmp.cod) );
	  return mknod(atom_tmp,tmp,NULL);
	}
      else 
	if ( atom.cod == '(' )
	  {
	    get_atom(); // mancam '('
	    paranteze_deschise++;
	    tmp = expresie(0);
	    if ( atom.cod != ')' ) eroare("in expresie, lipseste ')'");
	    else 
	      {
		paranteze_deschise --;
		return tmp;
	      }
	  }
  return NULL;
}

float rezultat(arbore *t)
{
  float valoare_st = 0,valoare_dr = 0 ;

  if ( t->vecin_st == NULL && t->vecin_dr == NULL )
    return t->valoare;
  if ( t->vecin_st != NULL && t->vecin_dr == NULL )
    {
      valoare_st = t->vecin_st->valoare;
      switch (t->tip)
	{
	case OP_INCREMENTARE:
	  valoare_st ++;
	  break;
	case OP_DECREMENTARE:
	  valoare_st --;
	  break;
	case '!':
	  valoare_st = ! ((int) valoare_st);
	  break;
	case '~':
	  valoare_dr = ~((int)valoare_st);
	  break;
	}
      return valoare_st;
    }

  if ( t->vecin_st->tip == COD_NUMAR || t->vecin_st->tip == COD_NUME )
    valoare_st = t->vecin_st->valoare;
  else valoare_st = rezultat(t->vecin_st);

  if ( t->vecin_dr->tip == COD_NUMAR || t->vecin_dr->tip == COD_NUME )
    valoare_dr = t->vecin_dr->valoare;
  else valoare_dr = rezultat(t->vecin_dr);
  
  switch(t->tip)
    {
    case '+':
      return valoare_st + valoare_dr;
      break;
    case '-':
      return valoare_st - valoare_dr;
      break;
    case '*':
      return valoare_st * valoare_dr;
      break;
    case '/':
      if ( valoare_dr == 0 )
	eroare("diviziune cu 0 (/)");
      else return valoare_st / valoare_dr;
      break;
    case '%':
      if ( valoare_dr == 0 )
	eroare("diviziune cu 0 (%)");
      return (int)valoare_st % (int)valoare_dr;
      break;
    case '<':
      return valoare_st < valoare_dr;
      break;
    case '>':
      return valoare_st > valoare_dr;
      break;
    case OP_MAI_MIC_EGAL:
      return valoare_st <= valoare_dr;
      break;
    case OP_MAI_MARE_EGAL:
      return valoare_st >= valoare_dr;
      break;
    case OP_EGALITATE:
      return valoare_st == valoare_dr;
      break;
    case OP_DIFERIT:
      return valoare_st != valoare_dr;
      break;
    case ',':
      return valoare_st;
      break;
    case '&':
      return (int)valoare_st & (int)valoare_dr;
      break;
    case '|':
      return (int)valoare_st | (int)valoare_dr;
      break;
    case '^':
      return (int)valoare_st ^ (int)valoare_dr;
      break;
    case OP_SI_LOGIC:
      return valoare_st && valoare_dr;
      break;
    case OP_SAU_LOGIC:
      return valoare_st || valoare_dr;
      break;
    case '~':
      return ~((int)valoare_st);
      break;
    case '!':
      return !((int)valoare_st);
      break;
    default:
      eroare("operator invalid");
      break;
    }
  return valoare_st + valoare_dr;
}



void do_atribuire(void)
{
  int indice ;

  indice = cauta_identificator(atom.nume);

  if ( indice >0 )
    {
      get_atom();
      if ( atom.cod != '=' )
	eroare("lipseste `=', pentru atribuire");
      else 
	{
	  get_atom();
	  variabile[indice].valoare_f = rezolva_exp();
	  if ( atom.cod != ';' )
	    eroare("lipseste `;'");
	  get_atom();
	}
    }
  else 
    eroare("variabila nedefinita");
}

void do_repeta(void)
{
  float valoare_conditie = 1;
  long pozitie_inceput,numar_linii;
  ATOM atom_tmp;
  
  get_atom(); // mancam `repeta'
  pozitie_inceput = pozitie_in_fisier;
  atom_tmp.cod = atom.cod;
  strcpy(atom_tmp.nume,atom.nume);
  numar_linii = numar_de_linii;
  
  do 
    {
      strcpy(atom.nume,atom_tmp.nume);
      atom.cod = atom_tmp.cod;
      pozitie_in_fisier = pozitie_inceput;
      numar_de_linii = numar_linii;

      bloc_instructiuni();
      if (atom.cod != COD_PANA ) 
	eroare("lipseste `pana'");
      get_atom();
      if ( atom.cod != '(' ) 
	eroare ("lipseste `('");
      get_atom();
      valoare_conditie = rezolva_exp();
      if ( atom.cod != ')' ) 
	eroare("lipseste `)'");
      get_atom();
    }
  while (!valoare_conditie );
}

void sari_repeta(void)
{
  float valoare_conditie;
  get_atom(); // mancam `repeta'
  sari_bloc();
  if (atom.cod != COD_PANA ) 
    eroare("lipseste `pana'");
  get_atom();
  if ( atom.cod != '(' ) 
    eroare ("lipseste `('");
  get_atom();
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ')' ) 
    eroare("lipseste `)'");
  get_atom();
}


void do_ctimp(void)
{
  float valoare_conditie;
  long numar_linii,pozitie_inceput;
  ATOM atom_tmp;

  get_atom();  // mancam `ctimp'
  
  numar_linii = numar_de_linii;
  pozitie_inceput = pozitie_in_fisier;
  atom_tmp.cod = atom.cod;
  strcpy(atom_tmp.nume,atom.nume);

  if (atom.cod != '(' ) 
    eroare ("lipseste `('");
  get_atom();
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ')' ) 
    eroare("lipseste `)'");
  get_atom();
  do 
    {
      if ( valoare_conditie != 0 )
	{
	  bloc_instructiuni();
	  if ( atom.cod != COD_SFCTIMP )  
	    eroare("lipseste `sfctimp'");
	  numar_de_linii = numar_linii;
	  pozitie_in_fisier = pozitie_inceput;
	  strcpy(atom.nume,atom_tmp.nume);
	  atom.cod = atom_tmp.cod;
	  get_atom();
	  valoare_conditie = rezolva_exp();
	  get_atom();
	}
      else sari_bloc();
    }
  while (valoare_conditie != 0 );
  sari_bloc();
  if ( atom.cod != COD_SFCTIMP ) 
    eroare("lipseste `sfctimp'");
  get_atom();
}

void sari_ctimp(void)
{
  float valoare_conditie;
  
  get_atom();  // mancam `ctimp'
  
  if (atom.cod != '(' ) 
    eroare ("lipseste `('");
  get_atom();
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ')' ) 
    eroare("lipseste `)'");
  get_atom();
  sari_bloc();
  if ( atom.cod != COD_SFCTIMP ) 
    eroare("lipseste `sfctimp'");
  get_atom();  //mancam sfctimp
}

void do_pentru(void)
{
  float valoare_conditie,tmp;
  long numar_linii,pozitie_inceput;
  int indice_contor,indice2;
  ATOM atom_tmp;

  get_atom();  // mancam `pentru'
  
  if (atom.cod != '(' ) 
    eroare ("lipseste `('");
  get_atom(); // asta trebuie sa fie numele contorului
  indice_contor = cauta_identificator(atom.nume);
  if ( indice_contor <= 0 ) 
    eroare("variabila nedefinita");
  get_atom();  
  if ( atom.cod == '=' ) 
    get_atom();  // mancam `='
  else if (atom.cod != ';' ) 
    eroare("initializarea contorului incorecta");
  
  variabile[indice_contor].valoare_f = rezolva_exp();
  if ( atom.cod != ';' ) 
    eroare("lipseste `;'");
  get_atom();
  // salvam pozitia si variabilele
  numar_linii = numar_de_linii;
  pozitie_inceput = pozitie_in_fisier;
  atom_tmp.cod = atom.cod;
  strcpy(atom_tmp.nume,atom.nume);
  valoare_conditie = rezolva_exp();  
  if ( atom.cod != ';' ) 
    eroare("lipseste `;'");
  get_atom();  // urmeaza incrementarea contorului
  indice2 = cauta_identificator(atom.nume);
  if ( indice2 != indice_contor ) 
    eroare("lipseste variabila contor");
  get_atom();
  if ( atom.cod != '=' ) 
    eroare("lipseste `=', atribuire asteptata");
  get_atom();
  tmp = rezolva_exp();
  if ( atom.cod != ')' ) 
    eroare("lipseste `)'");
  get_atom();

  do 
    {
      if ( valoare_conditie != 0 )
	{
	  bloc_instructiuni();
	  if ( atom.cod != COD_SFPENTRU ) 
	    eroare("lipseste `sfpentru'");
	  numar_de_linii = numar_linii;
	  pozitie_in_fisier = pozitie_inceput;
	  strcpy(atom.nume,atom_tmp.nume);
	  atom.cod = atom_tmp.cod;
	  
	  valoare_conditie = rezolva_exp();  
	  if ( atom.cod != ';' ) 
	    eroare("lipseste `;'");
	  get_atom();  // urmeaza incrementarea contorului
	  indice2 = cauta_identificator(atom.nume);
	  if ( indice2 != indice_contor ) 
	    eroare("lipseste variabila contor");
	  get_atom();
	  if ( atom.cod != '=' ) 
	    eroare("lipseste `=', atribuire asteptata");
	  get_atom();
	  variabile[indice_contor].valoare_f = rezolva_exp();
	  if ( atom.cod != ')' ) 
	    eroare("lipseste `)'");
	  get_atom();
	}
      else sari_bloc();
    }
  while (valoare_conditie != 0 );
  sari_bloc();
  if ( atom.cod != COD_SFPENTRU ) 
    eroare("`sfpentru' asteptat");
  get_atom();  
}

void sari_pentru()
{
 float valoare_conditie;
 int indice_contor,indice2;

  get_atom();  // mancam `pentru'
  
  if (atom.cod != '(' ) 
    eroare ("lipseste `('");
  get_atom(); // asta trebuie sa fie numele contorului
  indice_contor = cauta_identificator(atom.nume);
  if ( indice_contor <= 0 ) 
    eroare("variabila nedefinita");
  get_atom();  
  if ( atom.cod == '=' ) 
    get_atom();
  else if (atom.cod != ';' ) 
    eroare("initializarea contorului incorecta");
  
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ';' ) 
    eroare("lipseste `;'");
  get_atom();
  valoare_conditie = rezolva_exp();  
  if ( atom.cod != ';' ) 
    eroare("lipseste `;'");
  get_atom();  // urmeaza incrementarea contorului
  indice2 = cauta_identificator(atom.nume);
  if ( indice2 != indice_contor ) 
    eroare("lipseste variabila contor");
  get_atom();
  if ( atom.cod != '=' ) 
    eroare("lipseste `=', atribuire asteptata");
  get_atom();
  valoare_conditie = rezolva_exp();
  if ( atom.cod != ')' ) 
    eroare("lipseste `)'");
  get_atom();
  sari_bloc();  // sarim peste instructiuni
  if ( atom.cod != COD_SFPENTRU ) 
    eroare("`sfpentru' asteptat");
  get_atom();  
}

void do_subrutina(void)
{
  // ramane de implementat
  eroare (" din pacate nu am avut timp sa implementez subrutinele");
}

void sari_subrutina(void)
{
  // si asta ramane de implementat
  eroare (" din pacate nu am avut timp sa implementez subrutinele");
}
