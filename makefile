EXE=lp.exe
OBJ=main.o File/file.o Parse/parse.o Validate/validate.o Generals/generals.o Bounds/bounds.o LProblem/lp.o Objectives/objectives.o Subject_to/subject_to.o Section_buffer/section_buffer.o
OPT=-std=c89 -pedantic -Wextra -Wall

$(EXE): $(OBJ)
	gcc $(OBJ) -o $(EXE) $(OPT)

%.o: %.c
	gcc -c $< -o $@ $(OPT)

INCLUDE=-I./Generals./Bounds./File./Parse./Validate./LProblem./Objectives./Subject_to./Section_buffer

.c.o:
	gcc -c $^ $(OPT) $(INCLUDE)