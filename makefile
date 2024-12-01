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
      Section_buffer/section_buffer.c
OBJ = $(SRC:.c=.o)
CC = gcc
CFLAGS = -std=c89 -pedantic -Wextra -Wall
INCLUDE = -I./Generals -I./Bounds -I./File -I./Parse -I./Validate -I./LProblem -I./Objectives -I./Subject_to -I./Section_buffer

ifeq ($(OS),Windows_NT)
    OBJ := $(subst /,\\,$(OBJ))
endif

$(EXE): force $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(CFLAGS)

%.o: %.c force
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

force:

clean:
	$(RM) $(OBJ) $(EXE)
