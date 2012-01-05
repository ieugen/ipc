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
 
#include "baza.h"

struct s_identificator
{
  char nume[MARIME_IDENTIFICATOR]; // retine numele identificatorului
  int tip; // retine tipul identificatorului (TIP_INTREG,... )
  float valoare_f; // retine val reala (daca e reala)
};

typedef struct s_identificator variabila_t;

struct _arbore
{
  char nume[MARIME_IDENTIFICATOR];  // numele variabilei
  int tip; // retine tipul var
  float valoare;
  struct _arbore *vecin_st,*vecin_dr;
};

typedef struct _arbore arbore;

void main_parse(void);
void declarare_var(int tip_xxx);
void do_scrie(void);
void sari_scrie(void);
void do_citeste(void);
void sari_citeste(void);
int cauta_identificator(const char* nume_var );
void adauga_identificator(const char* nume_var,int tip_var,float val_var);
void atribuie_identificator(const char* nume_var,float val_var);

void do_daca(void);
void sari_daca(void);
void bloc_instructiuni(void);
void sari_bloc(void);
void do_atribuire(void);
void sari_atribuire(void);

void do_repeta(void);
void sari_repeta(void);
void do_ctimp(void);
void sari_ctimp(void);
void do_pentru(void);
void sari_pentru();

void do_subrutina(void);
void sari_subrutina(void);
float rezolva_exp(void);
float rezultat(arbore *t);
arbore *expresie(int nivel_precedenta);
arbore *termen(void);
arbore *mkfrunza(ATOM atm);
arbore *mknod(ATOM atm,arbore *t1,arbore *t2);
int precedenta(int codu);
int e_op_binar(int codu);
int asociativ(int codu);

