.PHONY: all clean

# Disable implicit rules
.SUFFIXES:

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
CPPFLAGS=

SRCDIR=src
INCLUDEDIR=include
OBJDIR=obj
DEPSDIR=dep
BINDIR=bin

EXEC=$(BINDIR)/shell

#VPATH=src/

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
DEPS=$(SRCS:$(SRCDIR)/%.c=$(DEPSDIR)/%.dep)

# Note: -lnsl does not seem to work on Mac OS but will
# probably be necessary on Solaris for linking network-related functions 
#LIBS += -lsocket -lnsl -lrt
LIBS+=-lpthread #-lncurses
INCLDIR=-I $(INCLUDEDIR)

all: $(EXEC)

# Création de l'exécutable
$(EXEC): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)

# Création des fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) $(INCLDIR) $< -o $@

-include $(DEPS)

# Génération des fichiers de dépendances
$(DEPSDIR)/%.dep: $(SRCDIR)/%.c
	@mkdir -p $(DEPSDIR)
	$(CC) -MM $(CFLAGS) $(CPPFLAGS) $(INCLDIR) $< -o $@

clean:
	rm -f $(EXEC)
	rm -f $(OBJDIR)/*.o
	-rm -r dep
