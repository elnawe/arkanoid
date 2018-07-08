SRC = main.c

OBJ = $(SRC:.c=.o)

build: $(OBJ)
	gcc $(SRC) -lSDL2_ttf -lSDL2_gfx -lSDL2_mixer -lSDL2 -o main.out

run: $(OBJ)
	./main.out
