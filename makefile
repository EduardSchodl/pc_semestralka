EXE=lp.exe
OBJ=main.o file.o parse.o validate.o stack.o Generals/generals.o Bounds/bounds.o lp.o
OPT=-std=c89 -pedantic -Wextra -Wall

$(EXE): $(OBJ)
	gcc $(OBJ) -o $(EXE) $(OPT)

%.o: %.c
	gcc -c $< -o $@ $(OPT)

INCLUDE=-I./Generals./Bounds

.c.o:
	gcc -c $^ $(OPT) $(INCLUDE)