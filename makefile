ifeq ($(OS),Windows_NT)
    RM = del /Q
else
    RM = rm -f
endif

EXE = lp.exe
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
OBJ = $(SRC:.c=.o)
CC = gcc
OPT = -std=c89 -pedantic -Wextra -Wall
INCLUDE = -I./Generals -I./Bounds -I./File -I./Parse -I./Validate -I./LProblem -I./Objectives -I./Subject_to -I./Section_buffer -I./Memory_manager

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CFLAGS) $(INCLUDE)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	$(RM) $(subst /,\,$(OBJ)) $(subst /,\,$(EXE))
else
	$(RM) $(OBJ) $(EXE)
endif

rebuild: clean $(EXE)

.PHONY: clean rebuild