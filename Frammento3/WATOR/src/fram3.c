/** \file processowator.c
       \author Bachmann Gherhard
     Si dichiara che il contenuto di questo file e' in ogni sua parte opera
     originale dell' autore.  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mcheck.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include "wator.h"
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#define NWORK_DEF 10
#define CHRON_DEF 6
#define SOCKNAME "visual.sck"	/* nome socket */
#define UNIX_PATH_MAX 108 /*man 7 unix */
#define FILE1 "wator.check"
#define N 4
#define K 3
typedef struct
{
	int id;
	pthread_t tid;
}workers; /* struttura per i workers */
workers * wo;
wator_t* pf;
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER; /* mutex per accedere alla coda tra dispatcher e worker */
pthread_cond_t list=PTHREAD_COND_INITIALIZER;
pthread_cond_t coll=PTHREAD_COND_INITIALIZER;
pthread_cond_t workersdone=PTHREAD_COND_INITIALIZER;
pthread_mutex_t **mutexm;
int nwork=NWORK_DEF;
int chron=CHRON_DEF;
int socket_descw;
struct sockaddr_un wat;
sigset_t signal_set;
volatile sig_atomic_t fine=0;
volatile sig_atomic_t lavorid=0;
volatile sig_atomic_t lavoriw=0;
enum { STATE_D, STATE_W, STATE_C } state = STATE_D; /* stati per la sincronizzazione thread */
int sig_number;
int **m;
/* Uso sigwait per aspettare il segnale, quindi creo un thread signal_waiter dedicato all'attesa dei segnali */
/* Appena arriva sigint o sigterm non finisce subito ma fa un ultima iterazione e viene quindi fatta anche dal visualizer */
typedef struct node
{
	int x;
	int y;
	int lung;
	int alt;
	struct node *next;
}lista;
lista *l;
/*----------PARTE DELLA LISTA-----------*/
void inseriscilista(int x, int y,int alt,int lung)
{
	lista *newlista = (lista*)malloc(sizeof(lista));
	lista *current;
	if(newlista == NULL)
	{
		perror("");
		exit(-1);
	}
	newlista->x = x;
	newlista->y = y;
	newlista->lung=lung;
	newlista->alt=alt;
	newlista->next = NULL;
	if(l==NULL)
		l=newlista;
	else
	{
		
		if(l->next == NULL)
			l->next = newlista;
		else
		{
			current = l;
			while (current->next != NULL)
				current = current->next;
			if(current->next == NULL)
				current->next = newlista;
		}
	}	
}
void freelist(lista *p)
{
	lista *temp;
	while ((p)!=NULL)
	{
		temp=p;
		p=p->next;
		free(temp);
	}
}
/*-------------------------------------------*/
void* signal_waiter(void *arg)
{
	FILE *f;
	while(1)
	{
		sigwait(&signal_set,&sig_number);
		if(sig_number==SIGUSR1)
		{
			f=fopen(FILE1,"w");
			write(1,"Wator Recevied SIGUSR1\n",strlen("Wator Recevied SIGUSR1\n"));
			print_planet(f,(pf->plan));
			fclose(f);
		}
		else
		{
			if(sig_number==SIGINT)
			{
				write(2,"Wator Recevied SIGINT\n",strlen("Wator Recevied SIGINT\n"));
				fine=1;
				break;
			}
			if(sig_number==SIGTERM)
			{
				write(2,"Wator Recevied SIGTERM\n",strlen("Wator Recevied SIGTERM\n"));	
				fine=1;
				break;				
			}
		}
	}
	pthread_exit((void*)EXIT_FAILURE);
}
/*-------------FUNZIONE DEI WORKER---------------*/
int new_update (int x,int y,int alt, int lung)
{
	int i=0,j=0,k=0,l=0,temp;
	int ris,msi=0;
	int rows,cols;
	rows=pf->plan->nrow;
	cols=pf->plan->ncol;
	if(pf==NULL)
	{
		errno=EINVAL;
		return -1;
	}
	for(i=x;i<alt;i++)
	{
		for(j=y;j<lung;j++)
		{
			/* caso di matrice con due righe o due colonne */
			/* oppure matrici 2xn e nx2 dove n e' il numero di righe o colonne preso dal file di configurazione*/
			if ( rows == 2 || cols == 2 ) /* non considero il caso iniziale di matrice 1x1 1x2 o 2x1 */
			{
				if(rows==2 && cols==2)
				{
					msi=2;
					temp=(i-1)%rows;
					if( temp < 0 )
						temp=rows-1;
					pthread_mutex_lock(&mutexm[temp][j]);
					temp=(j-1)%cols;
					if( temp < 0 )
						temp=cols-1;
					pthread_mutex_lock(&mutexm[i][temp]);
					pthread_mutex_lock(&mutexm[i][j]);
				}
				if(rows==2 && cols>2)
				{
					msi=3;
					temp=(i-1)%rows;
					if( temp < 0 )
						temp=rows-1;
					pthread_mutex_lock(&mutexm[temp][j]);
					temp=(j-1)%cols;
					if( temp < 0 )
						temp=cols-1;
					pthread_mutex_lock(&mutexm[i][temp]);
					pthread_mutex_lock(&mutexm[i][j]);
					temp=(j+1)%(pf->plan->ncol);
					if( temp > (cols-1) )
						temp=0;
					pthread_mutex_lock(&mutexm[i][temp]);
				}
				if(rows>2 && cols==2)
				{
					msi=4;
					temp=(i-1)%rows;
					if( temp < 0 )
						temp=rows-1;
					pthread_mutex_lock(&mutexm[temp][j]);
					temp=(j-1)%cols;
					if( temp < 0 )
						temp=cols-1;
					pthread_mutex_lock(&mutexm[i][temp]);
					pthread_mutex_lock(&mutexm[i][j]);
					temp=(i+1)%rows;
					if( temp > (rows-1) )
						temp=0;
					pthread_mutex_lock(&mutexm[temp][j]);
				}
			}
			else
			{
				/* caso della matrice generale, mi trovo sulle prime due righe o ultime due righe*/
				/* prime due colonne o ultme due colonne */
				/* in questo caso faccio la mutex delle 5 celle cosi non ce race condition tra i worker */
				if( (i < (x+2) ) || (i >= (alt-2) ) || (j < (y+2) ) || (j >= (lung-2)))
				{
					msi=1;
					temp=(i-1)%rows;
					if( temp < 0 )
						temp=rows-1;
					pthread_mutex_lock(&mutexm[temp][j]);	
					temp=(j-1)%cols;
					if( temp < 0 )
						temp=cols-1;
					pthread_mutex_lock(&mutexm[i][temp]);
					pthread_mutex_lock(&mutexm[i][j]);
					temp=(j+1)%(pf->plan->ncol);
					if( temp > (cols-1) )
						temp=0;
					pthread_mutex_lock(&mutexm[i][temp]);
					temp=(i+1)%rows;
					if( temp > (rows-1) )
						temp=0;
					pthread_mutex_lock(&mutexm[temp][j]);
				}
			}/* chiude else */
			if(pf->plan->w[i][j]==SHARK && m[i][j]==0)/* uno squalo che non ho incontrato prima */
			{
				ris=shark_rule2 (pf,i,j,&k,&l); /* regola per la riproduzione */
				if(ris==-1)
				{
					free_matrix(m,(pf->plan->nrow));
					return -1;
				}
				m[k][l]=1; /* metto a 1 cosi quella cella non la riguardo piu */
				/* k e l sono uguali ad i e j nel caso in cui lo squalo non si riproduce */
				if(ris==ALIVE)
					ris=shark_rule1 (pf,i,j,&k,&l);
				if(ris == -1 )/* in caso di errore devo deallocare la matrice m e la wator pw */
				{
					free_matrix(m,pf->plan->nrow);
					return -1;
				}
				if(ris==MOVE || ris==EAT)/* mangia e si sposta oppure si sposta */
				{
					m[i][j]=0;
					m[k][l]=1;
				}
			}
			if(pf->plan->w[i][j]==FISH && m[i][j]==0)
			{
				m[i][j]=1;
				ris=fish_rule4 (pf,i,j,&k,&l); /* regola per la riproduzione del pesce */
				if(ris==-1) /* errore nella regola */
				{
					free_matrix(m,pf->plan->nrow);
					return -1;
				}
				m[k][l]=1; /* se non ho errori metto la cella k,l della matrice m a 1 cosi non la riguardo */
				ris=fish_rule3 (pf,i,j,&k,&l);
				if(ris==-1)/* in caso di errore devo deallocare la matrice m e la wator pw */
				{
					free_matrix(m,pf->plan->nrow);
					return -1;
				}
				if(ris==MOVE)
					m[k][l]=1;
			}
			if(msi==1)
			{
				temp=(i-1)%rows;
				if( temp < 0 )
					temp=rows-1;
				pthread_mutex_unlock(&mutexm[temp][j]);	
				temp=(j-1)%cols;
				if( temp < 0 )
					temp=cols-1;
				pthread_mutex_unlock(&mutexm[i][temp]);
				pthread_mutex_unlock(&mutexm[i][j]);
				temp=(j+1)%(pf->plan->ncol);
				if( temp > (cols-1) )
					temp=0;
				pthread_mutex_unlock(&mutexm[i][temp]);
				temp=(i+1)%rows;
				if( temp > (rows-1) )
					temp=0;
				pthread_mutex_unlock(&mutexm[temp][j]);
				msi=0;
			}
			if(msi==2)
			{
				temp=(i-1)%rows;
				if( temp < 0 )
					temp=rows-1;
				pthread_mutex_unlock(&mutexm[temp][j]);
				temp=(j-1)%cols;
				if( temp < 0 )
					temp=cols-1;
				pthread_mutex_unlock(&mutexm[i][temp]);
				pthread_mutex_unlock(&mutexm[i][j]);
				msi=0;
			}
			if(msi==3)
			{
				temp=(i-1)%rows;
				if( temp < 0 )
					temp=rows-1;
				pthread_mutex_unlock(&mutexm[temp][j]);
				temp=(j-1)%cols;
				if( temp < 0 )
					temp=cols-1;
				pthread_mutex_unlock(&mutexm[i][temp]);
				pthread_mutex_unlock(&mutexm[i][j]);
				temp=(j+1)%(pf->plan->ncol);
				if( temp > (cols-1) )
					temp=0;
				pthread_mutex_unlock(&mutexm[i][temp]);
				msi=0;
			}
			if(msi==4)
			{
				temp=(i-1)%rows;
				if( temp < 0 )
					temp=rows-1;
				pthread_mutex_unlock(&mutexm[temp][j]);
				temp=(j-1)%cols;
				if( temp < 0 )
					temp=cols-1;
				pthread_mutex_unlock(&mutexm[i][temp]);
				pthread_mutex_unlock(&mutexm[i][j]);
				temp=(i+1)%rows;
				if( temp > (rows-1) )
					temp=0;
				pthread_mutex_unlock(&mutexm[temp][j]);
				msi=0;
			}
		}/*chiude for interno*/
	}/*chiude for esterno*/
	pf->nf=fish_count(pf->plan);
	pf->ns=shark_count(pf->plan);
	return 0;
}
static void * collector (void * arg)
{
	int i,j,count=1;
	char c;
	do
	{
		pthread_mutex_lock(&mtx);
		while(state!=STATE_C)
		{
			/*printf("Collector aspetta worker\n");*/
			pthread_cond_wait(&workersdone,&mtx);
		}
		pthread_mutex_unlock(&mtx);
		if( ((count % chron) == 0) || fine==1)
		{
			if((socket_descw=socket(AF_UNIX,SOCK_STREAM,0)) < 0) /* creazione socket */
			{
				perror("Creation Error");
				pthread_exit((void*)EXIT_FAILURE);
			}
			while((connect(socket_descw, (struct sockaddr *)&wat, sizeof(wat))) == -1) /* connessione socket */
			{
				if(errno==ENOENT) /* aspetto */
					sleep(3);
				else
				{
					perror("Connection Failed");
					close(socket_descw);
					unlink(SOCKNAME);
					pthread_exit((void*)EXIT_FAILURE);
				}
			}
			if(fine==1)
			{
				if( write(socket_descw,"E",strlen("E")) < 0) 
				{
					perror("In write operation");
					unlink(SOCKNAME);
				}
			}
			else
			{
				if( write(socket_descw,"V",strlen("V")) < 0) 
				{
					perror("In write operation");
					unlink(SOCKNAME);
				}
			}
			if( write(socket_descw,&(pf->plan->nrow),sizeof(pf->plan->nrow)) < 0) /* scrittura di nrow */
			{
				perror("In write 1");
				unlink(SOCKNAME);
			}
			if( write(socket_descw,&(pf->plan->ncol),sizeof(pf->plan->ncol)) < 0) /* scrittura di ncol */
			{
				perror("In write 2");
				unlink(SOCKNAME);
			}
			for(i=0;i<(pf->plan->nrow);i++)
				for(j=0;j<(pf->plan->ncol);j++)
				{
					c=cell_to_char(pf->plan->w[i][j]);
					if((write(socket_descw,&c,sizeof(c))) < 0) /* scrittura della matrice, carattere per carattere */
					{
						perror("In write 3");
						unlink(SOCKNAME);
					}
				}
			close(socket_descw);
		} /* chiude if */
		count++;
		/*printf("Collector vuole mutex\n");*/
		pthread_mutex_lock(&mtx);
		/*printf("Collector prende mutex\n");*/
		state=STATE_D;
		/*printf("Collector Invia segnale a dispatcher\n");*/
		pthread_cond_signal(&coll);
		pthread_mutex_unlock(&mtx);
		/*printf("Collector ha rilasciato la lock\n");*/
		
	} while(!fine);
	close(socket_descw);
	printf("Collector Finito\n");	
	pthread_exit((void*)EXIT_FAILURE);
}
static void * worker (void * arg) 
{
	FILE *f;
	int wid,x,y,alt,lung,tmp;
	char nome[20];
	lista *temp;
	wid = *(int*) arg;	
	temp=(lista *)malloc(sizeof(lista));
	sprintf(nome,"wator_worker_%d",wid);
	f=fopen(nome,"w");
	fclose(f);
	/*--leggo dalla coda--*/
	/* prendo il lavoro dalla coda */
	do
	{
		pthread_mutex_lock(&mtx);
		while(l==NULL || state!=STATE_W)
		{
			/*printf("Worker aspettano il dispatcher\n");*/
			pthread_cond_wait(&list,&mtx);
		}
		pthread_mutex_unlock(&mtx);	
		temp=l;
		l=l->next;
		x=temp->x;
		y=temp->y;
		alt=temp->alt;
		lung=temp->lung;
		printf("Worker %d \n",wid);
		/*printf("X = %d Y = %d alt= %d lung= %d\n",x,y,alt,lung);*/
		new_update(x,y,alt,lung);
		pthread_mutex_lock(&mtx);
			tmp=++lavoriw;
		pthread_mutex_unlock(&mtx);
		if(lavorid==tmp)
		{
			pthread_mutex_lock(&mtx);
			state=STATE_C;
			lavoriw=0;
			pthread_cond_signal(&workersdone);
			pthread_mutex_unlock(&mtx);			
		}
		sleep(1);
	}while(!fine);
	
	while(l!=NULL) /* finisco il lavoro e poi chiudo il thread */
	{
		temp=l;
		l=l->next;
		x=temp->x;
		y=temp->y;
		alt=temp->alt;
		lung=temp->lung;
		printf("Dopo Worker %d \n",wid);
		/*printf("X = %d Y = %d alt= %d lung= %d\n",x,y,alt,lung);*/
		new_update(x,y,alt,lung);
		lavoriw++;
		if(lavorid==lavoriw)
		{
			pthread_mutex_lock(&mtx);
			state=STATE_C;
			lavoriw=0;
			pthread_cond_signal(&workersdone);
			pthread_mutex_unlock(&mtx);
		}
	}
	printf("Worker %d Finito\n",wid);
	free(temp);
	pthread_exit((void *) 1);
}
static void * dispatcher( void *arg)
{
	/*---- inserisce nella coda i rettangoli per ogni chronon */
	/* una volta finito aspetta un segnale per rimettere nella coda, aspetta  il collector */
	/* anche lui finisce con fine=1; */
	int x=0,y=0,alt=0,lung=0,i=0,j=0;
	l=(lista*)malloc(sizeof(lista));
	l=NULL;
	do
	{
		i=0;
		pthread_mutex_lock(&mtx); /* per bloccare la coda finche inserisco */
		while(i<(pf->plan->nrow))
		{
			i=i+K;
			if(i<=(pf->plan->nrow))
				alt=K;
			else
				alt=(pf->plan->nrow)%K;
			j=0;
			while(j<(pf->plan->ncol))
			{
				j=j+N;
				if(j<=(pf->plan->ncol))
					lung=N;
				else
					lung=(pf->plan->ncol)%N;
				x=i-K;
				y=j-N;
				inseriscilista(x,y,alt,lung);
				lavorid++;
			}
		}
		printf("Inserito coordinate nella lista\n");
		state=STATE_W;
		/*printf("Dispatcher risveglia worker\n");*/
		pthread_cond_broadcast(&list);
		pthread_mutex_unlock(&mtx);
		/*printf("Dispatcher vuole mtx\n");*/
		/* in attesa del collector */
		pthread_mutex_lock(&mtx);
		/*printf("Dispatcher Ha preso mtx\n");*/
		while(state!=STATE_D)
		{
			/*printf("Dispatcher aspetta collector\n");*/
			pthread_cond_wait(&coll,&mtx);
		}
		pthread_mutex_unlock(&mtx);
		/*printf("Dispatcher rilascia mutex\n");*/
		lavorid=0;	
	}while(!fine);
	printf("Dispatcher Finito\n");
	pthread_exit((void*)EXIT_FAILURE);
}
void proc_wator(char *p,int pid)
{
	/* Creazione dei thread dispatcher , collector e il signal handler */
	int i,err;
	pthread_t tid;
	pthread_t tid2;
	pthread_t signal_thread_id;
	wo=(workers*)malloc(nwork*sizeof(workers));
	/*-------------segnale-----------*/
	sigemptyset(&signal_set);
	sigaddset(&signal_set,SIGINT);
	sigaddset(&signal_set,SIGTERM);
	sigaddset(&signal_set,SIGUSR1);
	if((pthread_sigmask(SIG_BLOCK,&signal_set,NULL))!=0)
	{
		fprintf(stderr,"Sigmask Error!\n");
		exit(EXIT_FAILURE);
	}
	/* handler del segnale */
	if((pthread_create(&signal_thread_id,NULL,&signal_waiter,NULL))!=0)
	{
		fprintf(stderr,"Thread Creation Error!\n");
		pthread_exit((void*)EXIT_FAILURE);
	}
	/*--------------------------------------------*/
	/*---------DISPATCHER-----------*/
	if((pthread_create(&tid,NULL,&dispatcher,NULL)) != 0 )
	{
		fprintf(stderr,"Errore nella creazione del thread dispatcher\n");
		pthread_exit((void*)EXIT_FAILURE);
	}
	/*-----------------WORKER-------------*/
	if(pf->nwork != nwork) /* verifico il numero di work */
		pf->nwork=nwork;
	/* creo i worker */
	for(i=0;i<nwork;i++)
	{
		wo[i].id=i;
		if( (err=pthread_create(&wo[i].tid,NULL,&worker,(void*)&wo[i].id )!= 0))
		{
			fprintf(stderr,"Errore nella creazione del thread dispatcher\n");
			pthread_exit((void*)EXIT_FAILURE);
		}			
	}
	/*-----------COLLECTOR---------------*/
	if((pthread_create(&tid2,NULL,&collector,NULL)) != 0 )
	{
		fprintf(stderr,"Errore nella creazione del thread dispatcher\n");
		pthread_exit((void*)EXIT_FAILURE);
	}
	/*-------ATTESA-----------*/
	if(pthread_join(signal_thread_id,NULL) != 0)
	{
		fprintf(stderr,"Join Error!\n");
		pthread_exit((void*)EXIT_FAILURE);
	}
	if(pthread_join(tid,NULL) != 0)
	{
		fprintf(stderr,"Join Error!\n");
		pthread_exit((void*)EXIT_FAILURE);
	}
	for(i=0;i<nwork;i++)
	{
		if(pthread_join(wo[i].tid,NULL) != 0)
		{
			fprintf(stderr,"Join Error!\n");
			pthread_exit((void*)EXIT_FAILURE);
		}
	}
	if(pthread_join(tid2,NULL) != 0)
	{
		fprintf(stderr,"Join Error!\n");
		pthread_exit((void*)EXIT_FAILURE);
	}
	free(wo);
}
int main (int argc , char* argv[])
{
	int pid,fv=0,fn=0,ff=0,i=0,j=0;
	char dump[30];	/* stringa di max 30 caratteri che conterra il nome del dumpfile */
	char *planet;
	int opt; /* per la gestione delle opzioni */
	pf=(wator_t*)malloc(sizeof(wator_t));
	if(pf==NULL)
	{
		perror("Allocazione fallita");
		exit(EXIT_FAILURE);
	}
	if( argc > 8 || argc < 2) /* troppi argomenti oppure troppo pochi argomenti */
	{
		fprintf(stderr,"Troppi/Troppo pochi argomenti!!\n");
		free(pf);
		return -1;
	}
	strcpy(dump,"stdout"); /* inizializzo dump  a stdout */
	while (( opt = getopt (argc,argv,"n:v:f:")) !=-1) /* gestione dell'opzioni con getopt */
	{
		switch(opt)
		{
		/* fn fv e ff sono contatori delle opzioni, cosi verifico che un opzione non e' stata inserita piu volte */
			case 'n':
				if(fn == 0)
				{
					fn++;
					nwork=atoi(optarg);
					break;
				}
				else
				{
					fprintf(stderr,"Inserito -n piu volte\n");
					free(pf);
					exit(EXIT_FAILURE);
				}
			case 'v':
				if(fv == 0)
				{
					fv++;
					chron=atoi(optarg);
					break;
				}
				else
				{
					fprintf(stderr,"Inserito -v piu volte\n");
					free(pf);
					exit(EXIT_FAILURE);
				}
			case 'f':
				if(ff == 0)
				{
					ff++;
					strcpy(dump,optarg);
					break;
				}
				else
				{
					fprintf(stderr,"Inserito -f piu volte\n");
					free(pf);
					exit(EXIT_FAILURE);
				}
			default:
				{
					fprintf(stderr,"Opzione sbagliata\n");
					free(pf);
					exit(EXIT_FAILURE);
				}
		}
	}
	/* controllo delle opzioni */
	if( (argc-1) != optind && argv[optind] != NULL)
	{
		/* significa che ce un altro argomento dopo optind, quindi sono stati passati piu file */
		printf("argc= %d , optind= %d \n",argc,optind);
		fprintf(stderr,"Troppi file passati\n");
		exit(1);
	}
	if(argv[optind]==NULL)
	{
		fprintf(stderr,"Nessuno file passato\n");
		exit(EXIT_FAILURE);
	}
	planet=(char*)malloc(strlen(argv[optind])*sizeof(char));
	strcpy(planet,argv[optind]); /* nome del file e' dentro optind */
	pf=new_wator(planet);
	if(pf==NULL)
	{
		fprintf(stderr,"Nessun file valido passato\n");
		exit(EXIT_FAILURE);
	}
	/* allocazione di m matrice di riserva e inizializzo la m e della matrice di mutex */
	m=(int**)malloc((pf->plan->nrow) * sizeof(int *));
	mutexm=(pthread_mutex_t **)malloc((pf->plan->nrow)*sizeof(pthread_mutex_t*));
	if(m==NULL)
		return -1;
	if(mutexm==NULL)
		return -1;
	for(i=0;i<(pf->plan->nrow);i++)
	{
		m[i]=(int*)malloc((pf->plan->ncol)*sizeof(int));
		mutexm[i]=(pthread_mutex_t*)malloc((pf->plan->ncol)*sizeof(pthread_mutex_t));
		if(m[i]==NULL) /* errore di allocazione */
		{
			for(j=i;j>0;j--)
				free(m[j]);
			free(m);
			return -1;
		}
		if(mutexm[i]==NULL)
		{
			for(j=i;j>0;j--)
				free(mutexm[j]);
			free(mutexm);
			return -1;
		}
	}
	for(i=0;i<(pf->plan->nrow);i++)
		for(j=0;j<(pf->plan->ncol);j++)
		{
			m[i][j]=0;
			pthread_mutex_init(&(mutexm[i][j]),NULL);
		}
	/* parte inizializzazione socket */
	memset(&wat,0,sizeof(wat));
	wat.sun_family=AF_UNIX;
	strncpy(wat.sun_path,SOCKNAME,UNIX_PATH_MAX);
	/*-----------------------------------------*/
	/* parte dei processi */
	pid=fork();
	if(pid < 0)
	{
		perror("ERRORE Creazione del figlio");
		free_wator(pf);
		exit(EXIT_FAILURE);
	}
	if(pid==0) /* figlio manda in esecuzione il visualizer */
	{
		if( (execl("./visualizer","visualizer",dump,NULL)) == -1)
		{
			perror("Exec error");
			free_wator(pf);
			exit(EXIT_FAILURE);
		}
	}	
	if(pid) /* padre, processo wator */
	{
		proc_wator(planet,pid);
	}
	/* chiusure e free */
	for(i=0;i<(pf->plan->nrow);i++)
		free(mutexm[i]);
	free(mutexm);
	free(planet);
	free_wator(pf);
	free(l);
	unlink(SOCKNAME);
	free_matrix(m,(pf->plan->nrow));
	write(2,"Wator Finito\n",strlen("Wator Finito\n"));
	return 0;
}
