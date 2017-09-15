/**
   \file
   \author lso15 
   
   \brief Test primo frammento
 */

#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>
#include <errno.h>
#include <unistd.h>

#include "wator.h"


/** file pianeta di test */
#define FILEDAT1 "planet1.dat"

#define NITER 10

/** main di test */
int main (void) {

  wator_t * pw;
  int i;

  mtrace();
 
  fprintf(stdout,"Caricamento simulazione da file.\n");
  
  if ( ( pw = new_wator(FILEDAT1) ) == NULL ) {
    fprintf(stderr,"Test wator 1\n");
    exit(EXIT_FAILURE);
    }  

  fprintf(stdout, "Esecuzione di %d iterazioni:\n", NITER);
  sleep(1);
  for(i=0; i<NITER; i++) {
    system("clear");
    update_wator(pw);
    print_planet(stdout,pw->plan);
    sleep(1);
  }
  free_wator(pw);
  muntrace();
  return EXIT_SUCCESS;
}

