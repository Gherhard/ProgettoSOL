=========================================================================
ISTRUZIONI e SPECIFICHE per il PRIMO FRAMMENTO del PROGETTO di LabSO 14/15
========================================================================

Il progetto WATOR e' suddiviso in  tre frammenti.
Il primo e' descritto in questo README.

Il primo frammento del progetto prevede l'implementazione di una
libreria che realizza alcune funzioni per manipolare il pianeta acquatico.
 
Questo primo frammento e' realizzabile usando esclusivamente C sequenziale, 
prerequisito del corso.


------------------------------------------------
Estrarre il materiale dal KIT di progetto
------------------------------------------------
Creare una directory temporanea, copiare lsokit_f1WATOR.tar  
nella directory e spostarsi nella directory appena creata. Es.

$$ mkdir Frammento1
$$ mv lsokit_f1WATOR.tar Frammento1
$$ cd Frammento1

S-tarare i file del primo kit con

$$ tar xvf lsokit_f1WATOR.tar 

questo comando crea nella directory corrente una directory "WATOR" che
contiene le seguenti sottodirectory e file 

$$ ls WATOR
doc/  src/ lib/ 

doc/ contiene i file di configurazione e la documentazione formato doxygen
src/ contiene sorgenti, header, Makefile, dati
lib/ e' vuota, conterra' le librerie del progetto



========================================================================
ISTRUZIONI per la realizzazione della libreria 
========================================================================

La libreria prevede la realizzazione di un insieme di funzioni. 
La descrizione dettagliata delle strutture dati e delle funzioni 
da realizzare si trova all'interno dei file *.h del kit.

Il kit fornisce dei programmi di test per verificare il funzionamento della
libreria: 

src/test-one.c
src/test-two.c
src/test-three.c


I test possono essere invocati con "make test11", "make test12" e "make test13"
rispettivamente dalla directory src.

------------------------------------------------------------------
File contenuti in src
------------------------------------------------------------------

$$ ls src/

gruppo-check.pl   README-1
gruppo.txt        README.doxygen
wator.h		  
test-one.c	  test-two.c
test-three.c	  DATA/
Makefile	  

-----------------------------------------------
Descrizione del contenuto dei file 
-----------------------------------------------

README-1		: questo file

gruppo-check.pl		: uno script Perl che controlla il formato del file
			  gruppo.txt prima di effettuare la consegna
			  (NON MODIFICARE)

gruppo.txt		: un file di esempio di specifica del gruppo
			  (solo 1 studente per gruppo)
			  (deve essere aggiornato con i dati di chi consegna,
			  secondo il formato esemplificato)

test-*.c
			: programmi di test (NON MODIFICARE)

wator.h
			: tipi e prototipi delle funzioni da realizzare
			  nel primo frammento
			  (si puo' modificare solo aggiungendo
			  campi/definizioni/prototipi senza MODIFICARE/ELIMINARE
			  quelli gia' esistenti)

Makefile		: makefile per il test del frammento e la
			  consegna 
			  (MODIFICARE SOLO nelle parti richieste, vedi
			  commenti nel file)

README.doxygen		:  breve descrizione di doxygen, 
                             con rimandi ai siti rilevanti
			     per chi desidera usarlo per la
                             documentazione del codice (facoltativo)


------------------------------------------------
Cosa deve essere realizzato per il primo frammento :
------------------------------------------------

Il primo frammento prevede la realizzazione delle  funzioni i cui
prototipi sono specificati in wator.h 
Negli stessi file sono specificati i tipi di dato usati dalle
funzioni.

I commenti nei file *.h spiegano le strutture dati da utilizzare
il comportamento delle varie funzioni, il significato dei parametri ed
i valori ritornati. La sintassi utilizzata e' quella di doxygen, un tool per
la creazione della documentazione 'tipo javadoc' comunemente utilizzato.
Chi vuole utilizzarlo per la produzione della documentazione puo' leggere
il README.doxygen.

Per generare la documentazione html dei file contenuti nel kit basta invocare

  bash:~$ make docu

e poi visualizzare il file ../doc/html/index.html con un browser.

E' fortemente consigliato di sviluppare il codice in modo incrementale.
Le funzioni definite prima in wator.h
vengono naturalmente utilizzate
nella realizzazione delle funzioni successive.
E' quindi ragionevole realizzare le funzioni in ordine.
Ogni funzione deve essere testata con opportuni programmi sviluppati
dallo studente prima di implementare le funzioni successive. 

Questo permette di localizzare molto meglio errori ed anomalie.

Solo quando siamo ragionevolmente convinti della correttezza del
codice si puo' passare al testing automatico del codice co i test
forniti dai docenti. 

Le funzioni definite in un file .h ( es wator.h ) possono essere realizzate
usando un singolo file (in questo caso PER CONVENZIONE il file .c
ha la stessa radice del .h -- es wator.c ) o in piu' file (di cui uno
ha comunque la stessa radice del .h).
In ogni caso il Makefile fornito nel kit DEVE essere esteso con i target
corrispondenti a i moduli oggetti di tutti i file "*.c" realizzati.

Inoltre, il Makefile contiene una variabile 
FILE_DA_CONSEGNARE1
che deve contenere **tutti i file che lo studente intende consegnare
per il primo frammento**.

------------------
Come procedere :
-----------------

0) leggere attentamente il README e capire il funzionamento il codice fornito
   dai docenti 

1) analizzare accuratamente i file di test per capire il tipo di test
   effettuati 

2) implementare le funzioni richieste in uno o piu' file
   ed effettuare testing preliminare utilizzando programmi sviluppati
   allo scopo

3) testare il software complessivo con i test forniti dai
  docenti. 

       bash:~$ make test11
       bash:~$ make test12
       bash:~$ make test13

  NOTA: (1) tutti i file sviluppati per la soluzione si devono trovare nella
  directory src/
  (2) E' importante che il test fornito dai docenti venga effettuato solo su
  un programma gia' funzionante e testato altrimenti i risultati
  possono essere di difficile interpretazione, fuorvianti o inutili.

4) preparare la documentazione: ovvero commentare adeguatamente il/i file che
   contengono la soluzione  ed inserire una intestazione contenente il nome
   dello sviluppatore ed una dichiarazione di originalita' 

   /** \file pippo.c
       \author Nino Bixio
     Si dichiara che il contenuto di questo file e' in ogni sua parte opera
     originale dell' autore.  */

     Non e' richiesto di preparare anche la documentazione formato
     doxygen. Chi volesse farlo e' invitato a leggere il  README.doxigen.

6) aggiornare il file "gruppo.txt" con nome e dati

7) consegnare il file ESCLUSIVAMENTE eseguendo

      bash:~$ make consegna1

   e seguendo le istruzioni. 
   Il target consegna1 crea un file tar che deve essere inviato
   all'indirizzo del docente con subject

   "lso15: consegna primo frammento"

   Tutte le consegne verranno confermate con un messaggio entro 2/3
   giorni all'indirizzo da cui e' stata effettuata la consegna. In
   caso questo non si verifichi contattare il docente.
   

---------------------------------------
 NOTE IMPORTANTI: LEGGERE ATTENTAMENTE
---------------------------------------

1) gli eleborati contenenti tar non creati con  "make consegna1" 
   non verranno accettati
   ( gli studenti sono invitati a controllare che il tar contenga
   tutti i file necessari ad una coretta compilazione ed esecuzione
   del progetto -- in pratica basta starare di nuovo il primo kit in
   una directory di prova e vedere se aggiungendo alla directory src i
   file nel tar di consegna i test vengono compilati ed eseguiti
   correttamente)

2) tutti gli elaborati verranno confrontati fra di loro con tool automatici
   per stabilire eventali situazioni di PLAGIO. Se tali situazioni si
   verificheranno *tutti* gli elaborato coinvolti verranno annullati con
   conseguente perdita del bonus. 

3) Tutti gli studenti coinvolti in un episodio di PLAGIO comprovato dovranno
   re-implementare il frammento incriminato e saranno sottoposti ad una prova
   orale supplementare. In caso di recidive verranno esclusi dalla
   possibilita' di sostenere l'esame per TUTTO IL RESTO DELL'ANNO ACCADEMICO
   IN CORSO.

4) Chi in sede di orale risulta palesemente non essere l'autore del software
   consegnato in uno dei frammenti verra' escluso dalla possibilita' di
   sostenere l'esame per TUTTO IL RESTO DELL'ANNO ACCADEMICO IN CORSO.

5) Tutti i comportamenti scorretti ai punti 3 e 4 verranno segnalati
   ufficialmente al presidente del corso di laurea, che si riserva di
   decidere azioni disciplinari supplementari a quelle sopra descritte.

----------------------------
 VALUTAZIONE DEL FRAMMENTO:
----------------------------

Gli studenti che consegnano una versione funzionante e ragionevolmente
corretta del frammento entro la data di scadenza accumulano un bonus di 2
punti che verra' sommato al voto finale (vedi lucidi lezioni ed
esercitazioni).

La qualita' del codice consegnato per il primo frammento verra' valutata come
parte del progetto finale e contribuira' alla votazione assegnata al progetto.
Eventuali caratteristiche in piu' rispetto a quelle strettamente richieste
dalle specifiche date qua (es. documentazione HTML o in altri
formati, funzionalita' in piu') verranno valutate in questa sede.
