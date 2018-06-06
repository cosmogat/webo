# 01-01-2016
# billy

EXE = webo # Executable
OBJ = webo.o # Objectes
COM = g++ # Compilador
LIB = # Llibreries (-l*, -L*, -I*)
MAC = # Macros (-D*)
AVS = -W -Wall -Wextra -ansi -pedantic # Avisos
OPT = -march=native -O2 -pipe # Optimitzacio
DEP = -g -DDEBUG # Depuracio, no recomanable junt amb $(OPT)
OPC = $(DEP) $(AVS) $(MAC) -std=c++14 # Opcions
DIR = /usr/local/bin # Directori per a instalar

all: $(EXE)

$(EXE): $(OBJ)
	@echo Enllaçant $(OBJ) per a crear $(EXE)
	$(COM) $(LIB) $(OBJ) -o $@

webo.o: webo.c++
	@echo Compilant $<
	$(COM) $(OPC) -c $<

exe: all
	./$(EXE)

install: all
	mkdir -p $(DIR)
	cp $(EXE) $(DIR)
	chown root:staff $(EXE)

clean:
	@echo Netejant...
	rm -rf $(EXE) $(OBJ) *~
