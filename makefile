

EXE = Parking

# Compilateur et editeur de lien
COMP = @g++
LINK = @g++

# Options de compilation et editions de liens
CHEMIN = includes
INC = -I$(CHEMIN)
LIB = -L$(CHEMIN)
CPPFLAGS = -Wall -ansi -ggdb -std=c++11 -g $(INC)
EDLFLAGS = $(LIB)


#Fichiers

SRC =
INT = main.h  #Mettre les .h ici
REAL = $(INT:.h=.cpp)
OBJ = $(INT:.h=.o) #Mettre le .o du programme de test la où ya le main

#Autres commandes et message
ECHO = @echo
RM = @rm
MESSAGE = "Compilation terminée"

$(EXE): $(OBJ)
	@echo "Édition des liens:"
	$(LINK)  -o $(EXE) $^ $(EDLFLAGS) -llibtp #-ltp -lncurses -ltcl
	$(ECHO) $(MESSAGE)

#Mettre les dependances particulieres ici

%.o:%.cpp
	@echo "Compilation de <$<>"
	$(COMP) -c $(CPPFLAGS) $<

clean:
	$(RM) -fv *.o $(EXE) *~
