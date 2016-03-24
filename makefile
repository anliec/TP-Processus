

EXE = Parking

# Compilateur et editeur de lien
COMP = @g++
LINK = @g++

# Options de compilation et editions de liens
CHEMIN = includes
INC = -I$(CHEMIN)
LIB = -L$(CHEMIN)
CPPFLAGS = -Wall -ansi -ggdb -std=c++11 -fpermissive -g $(INC)
EDLFLAGS = $(LIB) -ltp -lncurses -ltcl


#Fichiers

SRC =
INT = main.h Simulation.h Sortie.h #Mettre les .h ici
REAL = $(INT:.h=.cpp)
OBJ = $(INT:.h=.o) #Mettre le .o du programme de test la où ya le main

#Autres commandes et message
ECHO = @echo
RM = @rm
MESSAGE = "Compilation terminée"

$(EXE): $(OBJ)
	$(ECHO) "Édition des liens:"
	$(LINK)  -o $(EXE) $^ $(EDLFLAGS)
	$(ECHO) $(MESSAGE)

#Mettre les dependances particulieres ici

%.o:%.cpp
	$(ECHO) "Compilation de <$<>"
	$(COMP) -c $(CPPFLAGS) $<

clean:
	$(RM) -fv *.o $(EXE) *~
