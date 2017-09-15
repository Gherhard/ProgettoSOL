#################################
#
#     Makefile WATOR (progetto lso 2015)
#     (fram 1)(fram 2)(fram 3)
################################

# ***** DA COMPLETARE ******  con i file da consegnare *.c e *.h     
# primo frammento 
FILE_DA_CONSEGNARE1=wator.c

 

# Compilatore
CC= gcc
# Flag di compilazione
CFLAGS = -Wall -pedantic -g

# Librerie 
# Directory in cui si trovano le librerie
LIBDIR = ../lib
# Opzione di linking
LIBS = -L $(LIBDIR)

# Nome libreria progetto
LIBNAME1 = libWator.a

# Oggetti libreria $(LIBNAME1)
# DA COMPLETARE (se si usano altri file sorgente)
objects1 = wator.o





# Nome eseguibili primo frammento
EXE1=shark1
EXE2=shark2
EXE3=shark3

.PHONY: clean cleanall lib test11 test12 consegna1 docu


# creazione libreria 
lib:  $(objects1)
	-rm  -f $(LIBNAME1)
	-rm  -f $(LIBDIR)/$(LIBNAME1)
	ar -r $(LIBNAME1) $(objects1)
	cp $(LIBNAME1) $(LIBDIR)


###### Primo test 
shark1: test-one.o 
	$(CC) -o $@  $^ $(LIBS) -lWator

test-one.o: test-one.c wator.h 
	$(CC) $(CFLAGS) -c $<	 

###### Secondo test 
shark2: test-two.o 
	$(CC) -o $@  $^ $(LIBS) -lWator

test-two.o: test-two.c wator.h 
	$(CC) $(CFLAGS) -c $<	 

###### Terzo test 
shark3: test-three.o 
	$(CC) -o $@  $^ $(LIBS) -lWator

test-three.o: test-three.c wator.h 
	$(CC) $(CFLAGS) -c $<	 

#make rule per gli altri .o del primo frammento (***DA COMPLETARE***)








# make rule per gli altri .o del terzo frammento (***DA COMPLETARE***)




########### NON MODIFICARE DA QUA IN POI ################
# genera la documentazione con doxygen
docu: ../doc/Doxyfile
	make -C ../doc/

#ripulisce  l'ambiente 
clean:
	find . -name "*.o" -print -exec rm {} \; 

# cancella i file temporanei e gli eseguibili (targets)
cleanall: clean
	\rm -f *~
	\rm -f $(EXE1) $(EXE2) $(EXE3) $(LIBDIR)/$(LIBNAME1) $(LIBNAME1)

# target di test
FILE1=planet1.check
FILE2=planet2.check
FILE3=planet3.check
DATA1=planet1.dat
DATA2=planet2.dat
OUT1=out1.check
OUT2=out2.check
CONF1=wator.conf.1
CONF2=wator.conf.2
MTRACEFILE=./__wator.log
test11: 
	make clean
	make lib
	make $(EXE1)
	-rm -fr $(FILE1) $(FILE2)  $(OUT1) $(OUT2)
	cp DATA/$(OUT1) .
	cp DATA/$(OUT2) .
	cp DATA/$(CONF1) .
	chmod 644 $(OUT1) $(OUT2) $(CONF1)
	mv $(CONF1) wator.conf
	-rm -fr $(MTRACEFILE)
	MALLOC_TRACE=$(MTRACEFILE) ./$(EXE1)
	mtrace ./$(EXE1) $(MTRACEFILE)
	diff $(FILE1) $(OUT1)
	diff $(FILE2) $(OUT2)
	diff $(FILE3) $(OUT1)
	@echo "********** Test11 superato!"

DATA4=planet4.dat
CONF3=wator.conf.2
test12: 
	make clean
	make lib
	make $(EXE2)
	-rm -fr $(DATA4) $(DATA2) 
	cp DATA/$(DATA4) .
	cp DATA/$(DATA2) .
	cp DATA/$(CONF3) .
	chmod 644 $(DATA4) $(DATA2) $(CONF3)	
	mv $(CONF3) wator.conf
	-rm -fr $(MTRACEFILE)
	MALLOC_TRACE=$(MTRACEFILE) ./$(EXE2)
	mtrace ./$(EXE2) $(MTRACEFILE)
	@echo "********** Test12 superato!"

test13: 
	make clean
	make lib
	make $(EXE3)
	-rm -fr $(DATA1) $(DATA2) 
	cp DATA/$(DATA1) .
	cp DATA/$(DATA2) .
	cp DATA/$(CONF2) .
	chmod 644 $(DATA1) $(DATA2) $(CONF2)
	mv $(CONF2) wator.conf
	-rm -fr $(MTRACEFILE)
	MALLOC_TRACE=$(MTRACEFILE) ./$(EXE3)
	mtrace ./$(EXE3) $(MTRACEFILE)
	@echo "********** Test13 superato!"


SUBJECT1A="lso15: consegna primo frammento - corso A"
SUBJECT1B="lso15: consegna primo frammento - corso B"
ADDRESS="lso.di@listgateway.unipi.it"
# target di consegna del primo frammento
# effettua i test e prepara il tar per la consegna
consegna1:
	make clean
	make test11
	make test12
	make test13
	./gruppo-check.pl < gruppo.txt
	tar -cvf $(USER)-f1.tar ./gruppo.txt ./Makefile $(FILE_DA_CONSEGNARE1) 
	@echo "*** PRIMO FRAMMENTO: TAR PRONTO $(USER)-f1.tar "
	@echo "inviare come allegato a lso.di@listgateway.unipi.it con subject:"
	@echo "$(SUBJECT1A) oppure"
	@echo "$(SUBJECT1B)"


