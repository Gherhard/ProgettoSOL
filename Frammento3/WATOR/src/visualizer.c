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
planet_t* pv;
volatile sig_atomic_t fine=0;
int main(int argc , char* argv[])
{
	FILE *f;
	char op,c;
	int ncol=0,nrow=0,i,j,b,rfd;
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
		if((rfd = accept(socket_desc, NULL, NULL)) < 0 )
		{
			perror("Accept Failed");
			unlink(SOCKNAME);
			close(socket_desc);
			break;			/* faccio break perche in caso di SIGINT/SIGTERM lui si ferma qui */
		}
		if( (read(rfd,&op,sizeof(op)) ) < 0) /* legge nrow dalla socket */
		{
			perror("Read Operation");
			unlink(SOCKNAME);
			close(rfd);
			exit(EXIT_FAILURE);	/* la exit mi chiude anche la socket */
		}
		if( op == 'V' || op == 'E' )
		{
			printf("Ho letto l'operazione %c\n",op);
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
			if(op == 'E')
			{
				fine=1;
				if(strcmp(argv[1],"stdout")!=0)
				{
					f=fopen(argv[1],"w");
					print_planet(f,pv);	/* stampo il pianeta su file */
				}
				else
					print_planet(stdout,pv);
			}
				
		}
	}
	waitpid(getppid(),NULL,0);
	unlink(SOCKNAME);
	fclose(f);
	close(socket_desc);
	close(rfd);
	free_planet(pv);
	write(2,"Visualizer Finito\n",strlen("Visualizer Finito\n"));
	return 0;
}
