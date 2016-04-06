
EXE = Parking

#Compilateur et editeur de lien:
COMP = @g++
LINK = @g++

#Options de compilation et editions de liens:
CHEMIN = /shares/public/tp/tp-multitache
INC = -I$(CHEMIN)
LIB = -L$(CHEMIN)
CPPFLAGS = -Wall -ansi -ggdb -std=c++11 -g $(INC)
EDLFLAGS = $(LIB) -ltp -lncurses -ltcl


#Fichiers

SRC =
INT = main.h Simulation.h Sortie.h Entree.h #.h
REAL = $(INT:.h=.cpp) #.cpp
OBJ = $(INT:.h=.o) #.o

#Autres commandes et message
ECHO = @echo
RM = @rm
MESSAGE = "Compilation terminée"

$(EXE): $(OBJ)
	$(ECHO) "Édition des liens:"
	$(LINK)  -o $(EXE) $^ $(EDLFLAGS)
	$(ECHO) $(MESSAGE)

#Dependances particulieres ici:

%.o:%.cpp
	$(ECHO) "Compilation de <$<>"
	$(COMP) -c $(CPPFLAGS) $<

main.o:main.cpp main.h config.h
Sortie.o:Sortie.cpp Sortie.h config.h
Entree.o:Entree.cpp Entree.h config.h
Simulation.o:Simulation.cpp Simulation.h config.h

clean:
	$(RM) -fv *.o $(EXE) *~
