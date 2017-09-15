/**
   \file
   \author lso15 
   
   \brief Test primo frammento
 */

#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>
#include <errno.h>

#include "wator.h"

/** test inizializzazioni */


/** file pianeta di test */
#define FILEDAT1 "planet4.dat"
#define FILEDAT2 "planet2.dat"


/** main di test */
int main (void) {

  wator_t * pw;

  int k,l;

  mtrace();

 
 
  /* carico simulazione */
  fprintf(stdout,"Caricamento simulazione da file.\n");
  
  if ( ( pw = new_wator(FILEDAT1) ) == NULL ) {
    fprintf(stderr,"Test wator 1\n");
    exit(EXIT_FAILURE);
    }  

   print_planet(stdout,pw->plan);

  /* test regola 1 */
  /* test 1 nessuna mossa */
  if ( shark_rule1(pw,8,13,&k, &l) != STOP )   {
    fprintf(stderr,"Shark rule 1: test 1 KO.\n");
    exit(EXIT_FAILURE);
  }
  else
    fprintf(stderr,"Shark rule 1: test 1 OK.\n");

   print_planet(stdout,pw->plan);

  /* test2 mangia e si sposta */
  if ( shark_rule1(pw,0,1,&k, &l) != EAT )  {
    fprintf(stderr,"Shark rule 1: test 2 KO.\n");
    exit(EXIT_FAILURE);
  }
  
  else if ( ( k == 0 && l == 2 ) ||  ( k == 19 && l == 1 ) ) 
    fprintf(stderr,"Shark rule 1: test 2 OK.\n");
  else {
    fprintf(stderr,"Shark rule 1: test 2 KO: k = %d, l= %d...\n",k,l);
    exit(EXIT_FAILURE);
  }
   print_planet(stdout,pw->plan);
  
  /* test3 si sposta solo */
  if ( shark_rule1(pw,17,17,&k, &l) != MOVE ) {
      fprintf(stderr,"Shark rule 1: test 3 KO: k = %d, l= %d...\n",k,l);
      exit(EXIT_FAILURE);
   }
  else
    fprintf(stderr,"Shark rule 1: test 3 OK.\n");

  print_planet(stdout,pw->plan);

  /* ricarico il pianeta nello stato iniziale per il nuovo test */
  free_wator(pw);
  
  /* carico simulazione pianeta da file  */
  fprintf(stdout,"Caricamento simulazione da file.\n");
  
  if ( ( pw = new_wator(FILEDAT1) ) == NULL ) {
    fprintf(stderr,"Test wator 2\n");
    exit(EXIT_FAILURE);
    }  

  print_planet(stdout,pw->plan);

  /* test regola 3 Pesci */
  /* test 1 nessuna mossa */
  if ( fish_rule3(pw,19,28,&k, &l) != STOP )   {
    fprintf(stderr,"Fish rule 3: test 1 KO.\n");
    exit(EXIT_FAILURE);
  }
  else
    fprintf(stderr,"Fish rule 3: test 1 OK.\n");

  /* test2 si sposta */
  if ( fish_rule3(pw,19,29,&k, &l) != MOVE ) {
      fprintf(stderr,"Fish rule 3: test 2 KO: k = %d, l= %d...\n",k,l);
      exit(EXIT_FAILURE);
   }
  else if ( ( k == 19 && l == 0 ) ) 
    fprintf(stderr,"Fish rule 3: test 2 OK.\n");
  else {
    fprintf(stderr,"Fish rule 3: test 2 KO: k = %d, l= %d...\n",k,l);
    exit(EXIT_FAILURE);
  }

  print_planet(stdout,pw->plan);


   /* ricarico il pianeta nello stato iniziale per il nuovo test */
  free_wator(pw);
  
  /* carico simulazione pianeta da file  */
  fprintf(stdout,"Caricamento simulazione da file.\n");
  
  if ( ( pw = new_wator(FILEDAT2) ) == NULL ) {
    fprintf(stderr,"Test wator 3\n");
    exit(EXIT_FAILURE);
    }  

  print_planet(stdout,pw->plan);

  /* test regola 2 */
  /* NON SUCCEDE NIENTE */
  if ( shark_rule2(pw,2,3,&k, &l) != ALIVE || shark_count(pw->plan) != 1 ) {
      fprintf(stderr,"Shark rule 2: test 1 KO: k = %d, l= %d...\n",k,l);
      exit(EXIT_FAILURE);
   }
  else
    fprintf(stderr,"Shark rule 2: test 1 OK.\n");

  print_planet(stdout,pw->plan);

  /* NASCE UNO SQUALO */
  if ( shark_rule2(pw,2,3,&k, &l) != ALIVE || shark_count(pw->plan) != 2 ) {
      fprintf(stderr,"Shark rule 2: test 2 KO: k = %d, l= %d...\n",k,l);
      exit(EXIT_FAILURE);
   }
  else
    fprintf(stderr,"Shark rule 2: test 2 OK.\n");

  print_planet(stdout,pw->plan);
  /* muore uno squalo */
  if ( shark_rule2(pw,2,3,&k, &l) != DEAD || shark_count(pw->plan) != 1 ) {
      fprintf(stderr,"Shark rule 2: test 3 KO: k = %d, l= %d...\n",k,l);
      print_planet(stdout,pw->plan);
      exit(EXIT_FAILURE);
   }
  else
    fprintf(stderr,"Shark rule 2: test 3 OK.\n");

  print_planet(stdout,pw->plan);

   /* ricarico il pianeta nello stato iniziale per il nuovo test */
  free_wator(pw);
  
  
  /* carico simulazione pianeta da file  */
  fprintf(stdout,"Caricamento simulazione da file.\n");
  
  if ( ( pw = new_wator(FILEDAT2) ) == NULL ) {
    fprintf(stderr,"Test wator 4\n");
    exit(EXIT_FAILURE);
    }  

  print_planet(stdout,pw->plan);

  /* test regola 4 */
  /* NON SUCCEDE NIENTE */
  if ( fish_rule4(pw,0,0,&k, &l) != 0 || fish_count(pw->plan) != 1 ) {
      fprintf(stderr,"Fish rule 4: test 1 KO: k = %d, l= %d...\n",k,l);
      exit(EXIT_FAILURE);
   }
  else
    fprintf(stderr,"Fish rule 4: test 1 OK.\n");

  print_planet(stdout,pw->plan);

  /* nasce un pesce  */
  if ( fish_rule4(pw,0,0,&k, &l) != 0 || fish_count(pw->plan) != 2 ) {
      fprintf(stderr,"Fish rule 4: test 2 KO: k = %d, l= %d...\n",k,l);
 
      exit(EXIT_FAILURE);
   }
  else
    fprintf(stderr,"Fish rule 4: test 2 OK.\n");


  print_planet(stdout,pw->plan);

  free_wator(pw);
  muntrace();
  return EXIT_SUCCESS;
}

