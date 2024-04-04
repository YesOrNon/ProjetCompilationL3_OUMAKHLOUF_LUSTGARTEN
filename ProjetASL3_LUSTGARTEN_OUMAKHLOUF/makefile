# Makefile Projet Analyse Syntaxique
# LUSTGARTEN Leo | OUMAKHLOUF Selym

# $@ : the current target
# $^ : the current prerequisites
# $< : the first current prerequisite

CC=gcc
LEXFLAGS=
CFLAGS=-Wall -g -Wextra #-fsanitize=undefined
LDFLAGS=-Wall -ll #-fsanitize=undefined
BISONFLAGS=-d
EXEC=tpcas
PARS=$(EXEC)
LEX=$(EXEC)
BINDIR=bin
OBJDIR=obj
SRCDIR=src

$(BINDIR)/$(EXEC): $(OBJDIR)/$(PARS).o $(OBJDIR)/$(LEX).yy.o $(OBJDIR)/tree.o $(OBJDIR)/Symbols_Table.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/$(PARS).o: $(OBJDIR)/$(PARS).c $(SRCDIR)/tree.h $(SRCDIR)/Symbols_Table.h
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)/$(LEX).yy.o: $(OBJDIR)/$(LEX).yy.c $(OBJDIR)/$(PARS).h $(SRCDIR)/tree.h $(SRCDIR)/Symbols_Table.h
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)/Symbols_Table.o: $(SRCDIR)/Symbols_Table.c $(SRCDIR)/Symbols_Table.h
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)/tree.o: $(SRCDIR)/tree.c $(SRCDIR)/tree.h
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJDIR)/$(PARS).c $(OBJDIR)/$(PARS).h: $(SRCDIR)/$(PARS).y
	bison $(BISONFLAGS) -o $(OBJDIR)/$(PARS).c $(SRCDIR)/$(PARS).y

$(OBJDIR)/$(LEX).yy.c: $(SRCDIR)/$(LEX).lex
	flex $(LEXFLAGS) -o $(OBJDIR)/$(LEX).yy.c $(SRCDIR)/$(LEX).lex

clean:
	rm -f $(BINDIR)/* $(OBJDIR)/* rep/rapport.txt bin/* obj/*

