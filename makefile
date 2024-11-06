EXE=lp.exe
OBJ=main.o idklib.o
OPT=-std=c89 -pedantic -Wextra -Wall

$(EXE): $(OBJ)
	gcc $(OBJ) -o $(EXE) $(OPT)

.c.o:
	gcc -c $^ $(OPT)