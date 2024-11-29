EXE=lp.exe
OBJ=main.o File/file.o Parse/parse.o Validate/validate.o Generals/generals.o Bounds/bounds.o LP/lp.o
OPT=-std=c89 -pedantic -Wextra -Wall

$(EXE): $(OBJ)
	gcc $(OBJ) -o $(EXE) $(OPT)

%.o: %.c
	gcc -c $< -o $@ $(OPT)

INCLUDE=-I./Generals./Bounds./File./Parse./Validate./LP

.c.o:
	gcc -c $^ $(OPT) $(INCLUDE)