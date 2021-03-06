/** \file wator.c
       \author Bachmann Gherhard
     Si dichiara che il contenuto di questo file e' in ogni sua parte opera
     originale dell' autore.  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mcheck.h>
#include <errno.h>
#include <time.h>
#include "wator.h"
#define NWORK_DEF 0
#define CHRON_DEF 0
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
char cell_to_char (cell_t a)
{	
	switch(a)
	{
		case WATER:  return 'W';
		
		case SHARK: return 'S';
		
		case FISH: return 'F';
		
		default: return '?';
	}
}
int char_to_cell (char c)
{
	switch(c)
	{
		case 'W':  return WATER;
		
		case 'S': return SHARK;
		
		case 'F': return FISH;
		
		default: return -1;
	}
}
planet_t * new_planet (unsigned int nrow, unsigned int ncol)
{
	int i,j;
	planet_t *p=NULL;/* inizializzo il pianeta a NULL */
	if(nrow==0 || ncol==0)/* caso di errore */
	{
		errno=EINVAL;/* setto errno */
		return NULL;
	}
	p=(planet_t*)malloc(sizeof(planet_t));/* allocazione di p */
	if(p==NULL)/* errore sulla malloc */
	{
		errno=EINVAL;
		return NULL;
	}
	p->nrow=nrow;/* inserisco il valore nrow dentro il mio nuovo pianeta */
	p->ncol=ncol;/* inserisco il valore ncol dentro il mio nuovo pianeta */
	/* allocazione W */
	p->w=(cell_t **)malloc(nrow * sizeof(cell_t*));
	if((p->w) == NULL)/* errore sull'allocazione di p->w */
	{
		free(p);/* devo deallocare p */
		errno=EINVAL;
		return NULL;
	}
	/* allocazione BTIME */
	p->btime=(int**)malloc(nrow * sizeof(int *));/* allocazione p->btime */
	if(p->btime==NULL)/* errore sull'allocazione di p->btime */
	{
		errno=EINVAL;
		free(p->w);
		free(p);
		return NULL;	
	}
	/* allocazione DTIME */
	p->dtime=(int**)malloc(nrow * sizeof(int *));/* allocazione p->dtime */
	if(p->dtime==NULL)/* errore sull'allocazione di p->dtime */
	{
		errno=EINVAL;
		free(p->w);
		free(p->btime);
		free(p);
		return NULL;	
	}
	/* all'inizio i seguenti 3*nrow valori li metto a NULL cosi in caso di errore di allocazione posso chiamare la funzione free_planet */
	for(i=0;i<nrow;i++)
	{
		p->w[i]=NULL;
		p->btime[i]=NULL;
		p->dtime[i]=NULL;
	}
	/* allocazione di p->w[i], p->btime[i] , p->dtime[i] */
	for(i=0;i<nrow;i++)
	{
		p->w[i]=(cell_t*)malloc(ncol * sizeof(cell_t));
		p->btime[i]=(int *)malloc(ncol*sizeof(int));
		p->dtime[i]=(int *)malloc(ncol*sizeof(int));
		if(p->w[i]==NULL || p->btime[i]==NULL || p->dtime[i]==NULL)/* gestione di errori di allocazione */
		{
			errno=EINVAL;
			free_planet(p);/* chiamo la free_planet(p) */
			return NULL;
		}	
	}
	/* creazione del pianeta iniziale */
	for(i=0;i<nrow;i++)
		for(j=0;j<ncol;j++)
			p->w[i][j]=WATER;
	for(i=0;i<nrow;i++)
		for(j=0;j<ncol;j++)
			p->btime[i][j]=0;
	for(i=0;i<nrow;i++)
		for(j=0;j<ncol;j++)
			p->dtime[i][j]=0;
	return p;
}
void free_planet (planet_t* p)
{
	/*in ordine; fa la free di dtime,btime e w*/
	int i;
	for(i=0;i<p->nrow;i++)
		free(p->dtime[i]);
	free(p->dtime);
	for(i=0;i<p->nrow;i++)
		free(p->btime[i]);
	free(p->btime);
	for(i=0;i<p->nrow;i++)
		free(p->w[i]);
	free(p->w);
	free(p);
}
int print_planet (FILE* f, planet_t* p)
{
	char c;
	int i,j;
	/* file gia aperto, controllo l'apertura e controllo il secondo parametro "p" */
	if(f == NULL || p == NULL)
	{
		errno=EINVAL;
		return -1;
	}
	/*stampo sulle prime due righe del file le variabili nrow e ncol*/
	fprintf(f, "%d\n",p->nrow);
	fprintf(f, "%d\n",p->ncol);
	/*stampo la matrice completa sul file*/
	for(i=0;i<(p->nrow);i++)
	{
		for(j=0;j<(p->ncol-1);j++)
		{
			c=cell_to_char((p->w[i][j]));
			fprintf(f,"%c ",c);
			/* stampa colorata, commentata perche da errore nel test12 per la stampa sul file */
			/*if(c=='S')
				fprintf(f,RED "%c ",c);
			if(c=='F')
				fprintf(f,YELLOW "%c ",c);
			if(c=='W')
				fprintf(f,BLUE "%c ",c);*/
		}
		/*inserisco nell'ultima colonna il carattere e  il simbolo \n per una formattazione coretta del file*/
		c=cell_to_char((p->w[i][j]));
		fprintf(f, "%c\n",c);
	}
	return 0;
}
planet_t* load_planet (FILE* f)
{
	int nrow=0,ncol=0;
	int i,j,ris=0;
	char c,temp;
	planet_t* p;
	/* dal file prendo il primo numero e il carattere newline con la fscanf */
	/* ris mi serve nel caso in cui al posto dell'intero non ho niente. Avviene in caso di matrice malformattata */
	ris=fscanf(f,"%d%c",&nrow,&c);
	if(ris!=2 || c!='\n')/*se gli argomenti non sono 2 oppure dopo l'intero letto non ho \n */
	{
		errno=ERANGE;
		return NULL;
	}
	/* dal file prendo il secondo numero e il carattere newline con la fscanf */
	ris=fscanf(f,"%d%c",&ncol,&c);
	if(ris!=2 || c!='\n')/*se gli argomenti non sono 2 oppure dopo l'intero letto non ho \n */
	{
		errno = ERANGE;
		return NULL;
	}
	/*controllo se i valori letti dal file sono positivi*/
	if(nrow<=0 || ncol<=0)
	{
		errno = ERANGE;
		return NULL;
	}
	p=new_planet(nrow,ncol);
	/* controllo la creazione del pianeta */
	if(p==NULL)
		return NULL;
	/* qui legge la matrice dal file*/
	for(i=0;i<nrow;i++)
	{
		for(j=0;j<ncol;j++)
		{
			/* con la fscanf prendo due caratteri uno e' il simbolo della matrice,l'altro e' uno "spazio"
			   pero non vado fino all'ultima colonna perche all'ultima colonna devo controllare che sia \n*/
			fscanf(f,"%c%c",&c,&temp);
			/* ris contiene l'intero che rappresenta il carattere letto dalla matrice */
			ris=char_to_cell (c);
			if(j<ncol-1 && temp!=' ')
			{
					fprintf(stderr,"1!\n");
					errno = ERANGE;
					free_planet(p);
					return NULL;
			}
			if(j==ncol && temp!='\0')
			{
					errno = ERANGE;
					free_planet(p);
					return NULL;
			}
			if(ris==0)
				p->w[i][j]=SHARK;
			if(ris==1)					
				p->w[i][j]=FISH;
			if(ris==2)					
				p->w[i][j]=WATER;
			/* se non ho trovato W S o F nella matrice ma un altro carattere allora devo restituire un errore */
			if(ris!=0 && ris!=1 && ris!=2)
			{
				errno = ERANGE;
				free_planet(p);
				return NULL;
			}
		}
	}
	return p;
}
wator_t* new_wator (char* fileplan)
{
	FILE* f;
	FILE* wconf;
	wator_t * neww;
	/* utilizzo di contatori csd,csb,cfb per verificare che dentro il file wconf ognuna delle 3 variabile ci sia esattamente 1 volta */
	int csd=0,csb=0,cfb=0;
	char sep[2]=" ";/*separatore usato per la fgets*/
	char a[3];/* Le stringhe a e b sono usate per separare la riga letta da fgets */
	char b[5];
	/*fgets richiede una stringa str su cui ci devono andare le informazioni,la dimensione della stringa e' stata scelta da me*/
	char str[1001];
	/*token e' un puntatore a carattere, viene usata con la fgets per separare le stringhe da dei separatori*/
	char* token;
	f=fopen(fileplan,"r");/* file di configurazione del planet */
	wconf=fopen(CONFIGURATION_FILE,"r");/* file di configurazione wator */
	/* controllo apertura file */
	if(wconf == NULL)
	{
		errno=EMFILE;
		return NULL;
	}
	if(f == NULL)
	{
		errno=EMFILE;
		return NULL;
	}
	neww=(wator_t*)malloc(sizeof(wator_t));
	/* controllo allocazione */
	if(neww==NULL)
	{
		errno = ENOMEM;
		fclose(f);
		fclose(wconf);
		return NULL;
	}
	neww->plan=load_planet(f);
	/* controllo della load dal file f */
	if(neww->plan==NULL)
	{
		errno=ERANGE;
		fclose(f);
		fclose(wconf);
		return NULL;
	}
	neww->nf=fish_count(neww->plan);/* fish_count per contare i pesci nella matrice */
	neww->nwork=NWORK_DEF; /* 0 per ora, da modificare nel secondo frammento */
	neww->ns=shark_count(neww->plan);/* shark_count per contare gli squali nella matrice */
	neww->chronon=CHRON_DEF; /* 0 per ora, da modificare nel secondo frammento */
	/*uso di fgets per prendere le informazioni dal file wconf*/
	while(fgets(str, sizeof(str), wconf) != NULL)
	{
  		token=strtok(str,sep);/* la prima stringa del file fino allo primo spazio va in str */
		strcpy(a,token);/* copio dentro a la stringa di sopra */
		token=strtok(NULL,"\n");
		strcpy(b,token);/* copio dentro b la stringa di sopra */
		if(strcmp("sd",a)==0)
		{
			if(csd==0)/* controllo per non avere due sd dentro il file */
			{
				neww->sd=atoi(b);/* inserisco l'intero dentro neww->sd */
				csd++;
			}
			else/* malformattato il file */
				return NULL;
		}
		if(strcmp("sb",a)==0)
		{
			if(csb==0)/* controllo per non avere due sb dentro il file */
			{
				neww->sb=atoi(b);
				csb++;
			}
			else/* malformattato il file */
				return NULL;
		}
		if(strcmp("fb",a)==0)
		{
			if(cfb==0)/* controllo per non avere due fb dentro il file */
			{
				neww->fb=atoi(b);
				cfb++;
			}
			else/* malformattato il file */
				return NULL;
		}
		
	}
	/* chiusura file */
	fclose(f);
	fclose(wconf);
	return neww;
}
void free_wator(wator_t* pw)
{
	free_planet(pw->plan);
	free(pw);
}
/* Funzione che sceglie una posizione random per mangiare, per spostarsi oppure per riprodursi */
void posrand(int x,int y,int su,int giu,int sx,int dx,int *k,int *l,int nrow,int ncol)
{
	int r;
	/* srand(time(NULL)); sta solo nel main */
	/*casi in cui ho una sola cella adiacente su cui posso fare una delle 3 operazioni: mangiare,spostare o riprodurre*/
	if(su!=0 && giu==0 && sx==0 && dx==0)/*su*/
	{
		*k=((x-1)+nrow)%nrow;/* (x-1)+nrow perche altrimenti mi avrebbe dato come valore "-1" */
		*l=y;
	}
	if(su==0 && giu!=0 && sx==0 && dx==0)/*giu*/
	{
		*k=(x+1)%nrow;
		*l=y;
	}
	if(su==0 && giu==0 && sx!=0 && dx==0)/*sx*/
	{
		*k=x;
		*l=((y-1)+ncol)%ncol;
	}
	if(su==0 && giu==0 && sx==0 && dx!=0)/*dx*/
	{
		*k=x;
		*l=(y+1)%ncol;
	}
	/*casi in cui ho due celle adiacenti disponibili*/
	/*su e sx*/
	if(su!=0 && giu==0 && sx!=0 && dx==0)
	{
		r=rand()%2;/*il valore 0 per SU e 1 per SX*/
		if(r==0)
		{
			*k=((x-1)+nrow)%nrow;
			*l=y;		
		}
		else
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
	}
	/*su e dx*/
	if(su!=0 && giu==0 && sx==0 && dx!=0)
	{
		r=rand()%2;/*il valore 0 per SU e 1 per DX*/
		if(r==0)
		{		
			*k=((x-1)+nrow)%nrow;
			*l=y;	
		}
		else
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
	/*su e giu*/
	if(su!=0 && giu!=0 && sx==0 && dx==0)
	{
		r=rand()%2;/*il valore 0 per SU e 1 per GIU*/
		if(r==0)
		{
			*k=((x-1)+nrow)%nrow;
			*l=y;
		}
		else
		{
			*k=(x+1)%nrow;
			*l=y;

		}
	}
	/*sx e giu*/
	if(su==0 && giu!=0 && sx!=0 && dx==0)
	{
		r=rand()%2;/*il valore 0 per SX e 1 per GIU*/
		if(r==0)
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
		else
		{
			*k=(x+1)%nrow;
			*l=y;
		}
	}
	/*sx e dx*/
	if(su==0 && giu==0 && sx!=0 && dx!=0)
	{
		r=rand()%2;/*il valore 0 per SX e 1 per DX*/
		if(r==0)
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
		else
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
	/*giu e dx*/
	if(su==0 && giu!=0 && sx==0 && dx!=0)
	{
		r=rand()%2;/*il valore 0 per GIU e 1 per DX*/
		if(r==0)
		{
			*k=(x+1)%nrow;
			*l=y;	
		}
		else
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
	/*casi in cui ho 3 celle adiacenti disponibili*/
	/*su sx dx*/
	if(su!=0 && giu==0 && sx!=0 && dx!=0)
	{
		r=rand()%3;/*0 per su, 1 per sx, 2 per dx*/
		if(r==0)
		{
			*k=((x-1)+nrow)%nrow;
			*l=y;
		}
		if(r==1)
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
		if(r==2)
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
	/*su sx giu*/
	if(su!=0 && giu!=0 && sx!=0 && dx==0)
	{
		r=rand()%3;/*0 per su, 1 per sx, 2 per giu*/
		if(r==0)
		{
			*k=((x-1)+nrow)%nrow;
			*l=y;
		}
		if(r==1)
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
		if(r==2)
		{
			*k=(x+1)%nrow;
			*l=y;
		}
	}
	/*su giu dx*/
	if(su!=0 && giu!=0 && sx==0 && dx!=0)
	{
		r=rand()%3;/*0 per su, 1 per giu, 2 per dx*/
		if(r==0)
		{
			*k=((x-1)+nrow)%nrow;
			*l=y;
		}
		if(r==1)
		{
			*k=(x+1)%nrow;
			*l=y;
		}
		if(r==2)
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
	/*sx giu dx*/
	if(su==0 && giu!=0 && sx!=0 && dx!=0)
	{
		r=rand()%3;/*0 per sx, 1 per giu, 2 per dx*/
		if(r==0)
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
		if(r==1)
		{
			*k=(x+1)%nrow;
			*l=y;
		}
		if(r==2)
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
	/*caso in cui ho 4 celle adiacenti disponibili*/
	if(su!=0 && giu!=0 && sx!=0 && dx!=0)
	{
		r=rand()%3;/*0 per su, 1 per giu, 2 per sx, 3 per dx*/
		if(r==0)
		{
			*k=((x-1)+nrow)%nrow;
			*l=y;
		}
		if(r==1)
		{
			*k=(x+1)%nrow;
			*l=y;
		}
		if(r==2)
		{
			*k=x;
			*l=((y-1)+ncol)%ncol;
		}
		if(r==3)
		{
			*k=x;
			*l=(y+1)%ncol;
		}
	}
}
/* controlloPos e' la funzione che tra le celle adiacenti mi dice quali sono disponibili, le celle disponibili verrano poi usate
nella posrand per prendere una a caso tra quelle disponibili
la funzione mette a le variabili disponibili tra su,giu,sx,dx
le 4 variabili sono state passate per riferimento in questo modo la funzione non ritorna niente */
void controlloPOS(wator_t* pw, int x, int y, int *su,int *giu,int *sx,int *dx,cell_t t)
{
	int nrow=pw->plan->nrow;
	int ncol=pw->plan->ncol;
	int i;
	/*SOPRA*/
	
	if(x==0)/*se la x==0 per controllare su controllo la x di nrow-1*/
	{
		i=(x-1+nrow)%nrow;
		if(pw->plan->w[i][y]==t)
			*su=1;
	}
	else
	{
		if(pw->plan->w[x-1][y]==t)
			*su=1;
	}
	/*SOTTO*/
	if(x==nrow-1)/*se sono nell'ultima riga per controllare giu controllo la x di 0 */
	{
		if(pw->plan->w[0][y]==t)
			*giu=1;
	}
	else
	{
		if(pw->plan->w[x+1][y]==t)
			*giu=1;
	}
	/*SINISTRA*/
	if(y==0)/*se sono la prima colonna per controllare a sx controllo y di ncol-1*/
	{
		i=((y-1)+ncol)%ncol;
		if(pw->plan->w[x][i]==t)
			*sx=1;
	}
	else
	{
		if(pw->plan->w[x][y-1]==t)
			*sx=1;
	}
	/*DESTRA*/
	if(y==ncol-1)/*se sono sull'ultima colonna per controllare a destra controllo la y di 0*/
	{
		if(pw->plan->w[x][0]==t)
			*dx=1;
	}
	else
	{
		if(pw->plan->w[x][y+1]==t)
			*dx=1;
	}
}
int shark_rule1 (wator_t* pw, int x, int y, int *k, int* l)
{
	int su=0,sx=0,dx=0,giu=0;
	*k=x;
	*l=y;
	if((pw->plan->nrow-1 < x) || (pw->plan->ncol-1 < y) || y<0 || x<0 || (pw->plan->w[x][y] != SHARK))/* i casi di errore */
	{
		errno=EINVAL;
		return -1;
	}
	/*MANGIARE*/
	controlloPOS(pw,x,y,&su,&giu,&sx,&dx,FISH); /* passo le quattro variabili per riferimento e le controllo sotto */
	/*controllo che ha mangiato*/
	if(su!=0 || giu!=0 || sx!=0 || dx!=0)
	{
		/* se mi sposto mangiando devo portare con me tutti i tempi e mettere a zero i tempi della cella dove ero prima */
		posrand(x,y,su,giu,sx,dx,k,l,(pw->plan->nrow),(pw->plan->ncol));
		/* Squalo ha mangiato in posizione *k *l */
		pw->plan->w[x][y]=WATER; /* acqua dove ero prima */
		pw->plan->w[*k][*l]=SHARK; /* lo squalo si sposta nella nuova cell */
		pw->plan->btime[*k][*l]=pw->plan->btime[x][y];
		pw->plan->btime[x][y]=0;
		pw->plan->dtime[*k][*l]=0;
		pw->plan->dtime[x][y]=0;
		return EAT;
	}
	/* se nessuna delle celle adiacenti contengono pesci mi devo spostare con lo squalo in una posizione a caso adiacente */
	else
	{
		/* controllo dove mi posso spostare */
		controlloPOS(pw,x,y,&su,&giu,&sx,&dx,WATER);
		
		/* controllo che si puo spostare in una cella adiacente */
		if(su!=0 || giu!=0 || sx!=0 || dx!=0)
		{
			/* ok mi posso spostare e allora prendo una adiacente a caso */
			posrand(x,y,su,giu,sx,dx,k,l,(pw->plan->nrow),(pw->plan->ncol));
			/* k e l sono le mie nuove coordinate */
			/* se mi sposto devo portare con me tutti i tempi e mettere a zero i tempi della cella dove ero prima */
			/* Squalo si sposta in posizione *k *l */
			pw->plan->w[x][y]=WATER; /* acqua dove ero prima */
			pw->plan->w[*k][*l]=SHARK;/* lo squalo si sposta nella nuova cella */
			pw->plan->btime[*k][*l]=pw->plan->btime[x][y];
			pw->plan->btime[x][y]=0;
			pw->plan->dtime[*k][*l]=pw->plan->dtime[x][y];
			pw->plan->dtime[x][y]=0;
			return MOVE;
		}
		else/* non mi posso spostare perche ci sono squali in tutte le celle adiacenti */			
			return STOP;
	}

}
int shark_rule2 (wator_t* pw, int x, int y, int *k, int* l)
{
	int su=0,giu=0,dx=0,sx=0;
	*k=x;/* nel caso in cui k e x sono uguali dopo questa funzione significa che lo qualo non si poteva riprodurre */
	*l=y;
	if(pw->plan->nrow < x || pw->plan->ncol < y || x<0 || y<0 || pw->plan->w[x][y]!=SHARK )/* i casi di errore */
	{
		errno=EINVAL;
		return -1;
	}
	/* parte della morte */
	if(pw->plan->dtime[x][y]==pw->sd)
	{
		pw->plan->w[x][y]=WATER;
		/* Squalo e' morto in posizione x y */
		return DEAD;
	}
	pw->plan->dtime[x][y]++;
	/*  Lo squalo in x y non muore */
	/* parte della riproduzione */
	if(pw->plan->btime[x][y] == pw->sb)/* lo squalo puo riprodursi */
	{
		/* dobbiamo controllare dove c'e acqua intorno allo squalo per la riproduzione */
		controlloPOS(pw,x,y,&su,&giu,&sx,&dx,WATER);
		if(su!=0 || giu!=0 || sx!=0 || dx!=0)
		{
			posrand(x,y,su,giu,sx,dx,k,l,(pw->plan->nrow),(pw->plan->ncol));
			/*printf("Squalo nato in posizione %d %d \n",*k,*l);*/
			pw->plan->w[*k][*l]=SHARK;
			/* azzero i tempi dello squalo nato ora e di quello che l'ha fatto nascere */
			pw->plan->dtime[*k][*l]=0;
			pw->plan->btime[*k][*l]=0;
			pw->plan->btime[x][y]=0;
		}
		else
		{
			/* Nascita dello squalo fallita per mancanza di spazio */
			pw->plan->btime[x][y]=0;
		}
	}
	else
	{
		pw->plan->btime[x][y]++;	
		/* Lo squalo in x y non si puo riprodurre */
	}
	return ALIVE;

}
int fish_rule3 (wator_t* pw, int x, int y, int *k, int* l)
{
	int su=0,sx=0,dx=0,giu=0;
	if(pw->plan->nrow < x || pw->plan->ncol < y || x<0 || y<0 || pw->plan->w[x][y]!=FISH)/* i casi di errore */
	{
		errno=EINVAL;
		return -1;
	}
	/* controllo dove mi posso spostare, quindi cerco acqua */
	controlloPOS(pw,x,y,&su,&giu,&sx,&dx,WATER);
	
	/* controllo se mi sono spostato */
	if(su!=0 || giu!=0 || sx!=0 || dx!=0)
	{
		posrand(x,y,su,giu,sx,dx,k,l,(pw->plan->nrow),(pw->plan->ncol));
		/* Pesce si sposta in posizione *k *l */
		pw->plan->w[x][y]=WATER;
		pw->plan->w[*k][*l]=FISH;
		pw->plan->btime[*k][*l]=pw->plan->btime[x][y];
		pw->plan->btime[x][y]=0;
		pw->plan->dtime[*k][*l]=pw->plan->dtime[x][y];
		pw->plan->dtime[x][y]=0;
		return MOVE;
	}
	/* se nessuna delle celle adiacenti e' vuota */
	else
	{
		/* Non mi sono spostato! */
			return STOP;
	}
}
int fish_rule4 (wator_t* pw, int x, int y, int *k, int* l)
{
	int su=0,giu=0,dx=0,sx=0;
	*k=x;
	*l=y;
	if(pw->plan->nrow < x || pw->plan->ncol < y || x<0 || y<0 || pw->plan->w[x][y]!=FISH)/*i casi di errore*/
	{
		errno=EINVAL;
		return -1;
	}
	/* parte della riproduzione */
	if(pw->plan->btime[x][y] == pw->fb)/* il pesce puo riprodursi */
	{
		/* dobbiamo controllare dove c'e acqua intorno al nostro pesce */
		controlloPOS(pw,x,y,&su,&giu,&sx,&dx,WATER);		
		if(su!=0 || giu!=0 || sx!=0 || dx!=0)
		{
			/* abbiamo trovato acqua intorno al pesce */
			posrand(x,y,su,giu,sx,dx,k,l,(pw->plan->nrow),(pw->plan->ncol));
			pw->plan->btime[x][y]=0;
			pw->plan->btime[*k][*l]=0;
			pw->plan->w[*k][*l]=FISH;
			pw->plan->dtime[*k][*l]=0;
		}
		else
		{
			/* Nascita del pesce fallita per mancanza di spazio! */
			pw->plan->btime[x][y]=0;
		}
		
	}
	else
		pw->plan->btime[x][y]++;
	return 0;

}
int fish_count (planet_t* p)
{
	int i=0,j=0,ris=0;
	for(i=0;i<p->nrow;i++)
		for(j=0;j<p->ncol;j++)
		{
			if(p->w[i][j]==FISH)
				ris++;
		}
	return ris;
}	
int shark_count (planet_t* p)
{
	int i=0,j=0,ris=0;
	for(i=0;i<p->nrow;i++)
		for(j=0;j<p->ncol;j++)
		{
			if(p->w[i][j]==SHARK)
				ris++;
		}
	return ris;
}
void free_matrix(int **m,int nrow)
{
	int i;
	for(i=0;i<nrow;i++)
		free(m[i]);
	free(m);
}
int update_wator (wator_t * pw)
{
	int i=0,j=0,k=0,l=0;
	int ris;
	int **m;
	/* la matrice m viene usata per tenere traccia delle cella gia controllate sulla matrice */
	if(pw==NULL)
	{
		errno=EINVAL;
		return -1;
	}
	/* allocazione di m */
	m=(int**)malloc(pw->plan->nrow * sizeof(int *));
	if(m==NULL)
		return -1;
	for(i=0;i<pw->plan->nrow;i++)
	{
		m[i]=(int*)malloc((pw->plan->ncol)*sizeof(int));
		if(m[i]==NULL) /* errore di allocazione */
		{
			for(j=i;j>0;j--)
				free(m[j]);
			free(m);
			return -1;
		}
	}
	/* per ogni iterazione di update dentro il main devo azzerare la matrice m */
	for(i=0;i<(pw->plan->nrow);i++)
		for(j=0;j<(pw->plan->ncol);j++)
			m[i][j]=0;
	/*I pesci e gli squali applicano sempre prima la regola relativa alla riproduzione e poi quella relativa a mangiare e spostarsi.*/
	for(i=0;i<(pw->plan->nrow);i++)
	{
		for(j=0;j<(pw->plan->ncol);j++)
		{
			if(pw->plan->w[i][j]==SHARK && m[i][j]==0)/* uno squalo che non ho incontrato prima */
			{
				ris=shark_rule2 (pw,i,j,&k,&l); /* regola per la riproduzione */
				if(ris==-1)
				{
					free_matrix(m,pw->plan->nrow);
					return -1;
				}
				m[k][l]=1; /* metto a 1 cosi quella cella non la riguardo piu */
				/* k e l sono uguali ad i e j nel caso in cui lo squalo non si riproduce */
				if(ris==ALIVE)
					ris=shark_rule1 (pw,i,j,&k,&l);
				if(ris == -1 )/* in caso di errore devo deallocare la matrice m e la wator pw */
				{
					free_matrix(m,pw->plan->nrow);
					return -1;
				}
				if(ris==MOVE || ris==EAT)/* mangia e si sposta oppure si sposta */
				{
					m[i][j]=0;
					m[k][l]=1;
				}
			}
			if(pw->plan->w[i][j]==FISH && m[i][j]==0)
			{
				m[i][j]=1;
				ris=fish_rule4 (pw,i,j,&k,&l); /* regola per la riproduzione del pesce */
				if(ris==-1) /* errore nella regola */
				{
					free_matrix(m,pw->plan->nrow);
					return -1;
				}
				m[k][l]=1; /* se non ho errori metto la cella k,l della matrice m a 1 cosi non la riguardo */
				ris=fish_rule3 (pw,i,j,&k,&l);
				if(ris==-1)/* in caso di errore devo deallocare la matrice m e la wator pw */
				{
					free_matrix(m,pw->plan->nrow);
					return -1;
				}
				if(ris==MOVE)
					m[k][l]=1;
			}
		}/*chiude for interno*/
	}/*chiude for esterno*/
	/*pw->nf=fish_count(pw->plan);
	pw->ns=shark_count(pw->plan);*/
	free_matrix(m,pw->plan->nrow);
	return 0;
}
