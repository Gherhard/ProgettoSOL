/** \file visualizer.c
       \author Bachmann Gherhard
     Si dichiara che il contenuto di questo file e' in ogni sua parte opera
     originale dell' autore.  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mcheck.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <limits.h>
#include "wator.h"
#include <pthread.h>
#define NWORK_DEF 10
#define CHRON_DEF 6
#define SOCKNAME "visual.sck" /* nome socket */
#define UNIX_PATH_MAX 108 /*man 7 unix */
struct sockaddr_un visual;
int socket_desc;		/* socket fd */
sigset_t signal_set;
planet_t* pv;
volatile sig_atomic_t fine=0;
/* la terminazione gentile non e' gestita nelle migliori dei modi. */
/* ho usato fine come variabile per mettere fine al ciclo delle letture del visualizer, pero quando arriva una sigint/sigterm al processo wator, il processo visualizer fa comunque la sua parte, dopodiche e' normale che lui quando esce dal thread del handler sia bloccato da qualche parte sulla socket, perche nessuno e' piu connesso sulla socket quindi finira sempre il ciclo con un errore. Spesso nel mio caso e' un errore sulla accept */
void* signal_waiter(void *arg)
{
	FILE *f;	/* file dump */
	char* file=(char*)arg;
	int sig_number;
	if(strcmp(file,"stdout")!=0)	/* apro soltanto se e' diverso da stdout */
		f=fopen(file,"w");
	while(1)
	{
		sigwait(&signal_set,&sig_number);
		if(sig_number==SIGINT)
		{
			write(2,"Visual Recevied SIGINT\n",strlen("Visual Recevied SIGINT\n"));
			if(strcmp(file,"stdout")!=0)
			{
				print_planet(f,pv);	/* stampo il pianeta su file */
				fclose(f);
			}
			else
				print_planet(stdout,pv);	/* stampo il pianeta su stdout */
			free_planet(pv);				
			close(socket_desc); /* il visualizer ha gia fatto l'ultima iterazione quindi nel ciclo mi finira con un errore */
			/* questo perche sta cercando di leggere da una socket gia chiusa, comunque non poteva leggere niente perche */
			/* wator ha finito di scrivere e l'ultima iterzione e' gia stata letta */
			unlink(SOCKNAME);
			fine=1;
			break;
		}
		if(sig_number==SIGTERM)
		{
			write(2,"Visual Recevied SIGTERM\n",strlen("Visual Recevied SIGTERM\n"));
			if(strcmp(file,"stdout")!=0)
			{
				f=fopen(file,"w");
				print_planet(f,pv);	/* stampo il pianeta su file */
			}
			else
				print_planet(stdout,pv);	/* stampo il pianeta su stdout */
			free_planet(pv);
			close(socket_desc);
			unlink(SOCKNAME);
			fine=1;
			break;
		}
	}
	pthread_exit((void*)EXIT_FAILURE);
}
int main(int argc , char* argv[])
{
	int rfd;
	char c;
	int ncol=0,nrow=0,i,j,b;
	/* parte del signal handler */
	pthread_t signal_thread_id;
	sigemptyset(&signal_set);
	sigaddset(&signal_set,SIGINT);
	sigaddset(&signal_set,SIGTERM);
	if((pthread_sigmask(SIG_BLOCK,&signal_set,NULL))!=0)
	{
		fprintf(stderr,"Sigmask Error!\n");
		exit(EXIT_FAILURE);
	}
	if((pthread_create(&signal_thread_id,NULL,&signal_waiter,(void*)argv[1]))!=0)
	{
		fprintf(stderr,"Thread Creation Error!\n");
		exit(EXIT_FAILURE);
	}
	/*----------------------------------*/
	memset(&visual,0,sizeof(visual));
	visual.sun_family=AF_UNIX;
	strncpy(visual.sun_path,SOCKNAME,UNIX_PATH_MAX);
	
	/* parte creazione socket */
	if((socket_desc=socket(AF_UNIX,SOCK_STREAM,0)) < 0)
	{
		perror("Creation Error");
		exit(EXIT_FAILURE);
	}
	if((bind(socket_desc, (struct sockaddr *)&visual, sizeof(visual))) < 0)
	{
		perror("Binding Failed");
		unlink(SOCKNAME);
		exit(EXIT_FAILURE);
	}
	if((listen(socket_desc,SOMAXCONN)) < 0 )
	{
		perror("Listen Failed");
		unlink(SOCKNAME);
		exit(EXIT_FAILURE);
	}
	/*--------------------------*/
	/* ciclo della socket */
	while(!fine)
	{
		sleep(1);
		if((rfd = accept(socket_desc, NULL, NULL)) < 0 )
		{
			perror("Accept Failed");
			unlink(SOCKNAME);
			close(socket_desc);
			break;			/* faccio break perche in caso di SIGINT/SIGTERM lui si ferma qui */
		}
		if((read(rfd,&(nrow),sizeof(nrow))) < 0) /* legge nrow dalla socket */
		{
			perror("Read 1");
			unlink(SOCKNAME);
			close(rfd);
			exit(EXIT_FAILURE);	/* la exit mi chiude anche la socket */
		}
		if((read(rfd,&(ncol),sizeof(ncol))) < 0) /* legge ncol dalla socket */
		{
			perror("Read 2");
			unlink(SOCKNAME);
			close(rfd);
			exit(EXIT_FAILURE);
		}
		pv=new_planet(nrow,ncol);
		for(i=0;i<nrow;i++)
		{
			for(j=0;j<ncol;j++)
			{
				if((read(rfd,&(c),sizeof(c))) < 0) /* legge la matrice carattere per carattere */
				{
					perror("Read 3");
					close(rfd);
					exit(EXIT_FAILURE);
				}
				b=char_to_cell(c);
				pv->w[i][j]=b;
			}
		}
	}
	close(rfd);
	write(2,"Visualizer Finito\n",strlen("Visualizer Finito\n"));
	return 0;
}
