ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
    RM = del /Q
else
    EXE_EXT =
    RM = rm -f
endif

EXE = lp$(EXE_EXT)
SRC = main.c \
      File/file.c \
      Parse/parse.c \
      Validate/validate.c \
      Generals/generals.c \
      Bounds/bounds.c \
      LProblem/lp.c \
      Objectives/objectives.c \
      Subject_to/subject_to.c \
      Section_buffer/section_buffer.c \
      Memory_manager/memory_manager.c
CC = gcc
OPT = -std=c89 -pedantic -Wextra -Wall
INCLUDE = -I./Generals./Bounds./File./Parse./Validate./LProblem./Objectives./Subject_to./Section_buffer./Memory_manager

$(EXE): clean $(OBJ)
	$(CC) $(SRC) -o $(EXE) $(OPT)

c.o:
	$(CC) -c $^ -o $(OPT)

clean:
	$(RM) $(OBJ) $(EXE)

rebuild: clean
	$(MAKE) $(EXE)

.PHONY: clean rebuild