CFLAGS = -Wall -Wextra -std=c11
SRC = src/main.c src/admin.c src/user.c src/loan.c src/utils.c src/demo.c
OBJ = $(SRC:.c=.o)

flowcash: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

demo: flowcash
	./flowcash --demo

clean:
	rm -f $(OBJ) flowcash
