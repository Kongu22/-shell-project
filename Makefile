TARGET = MyShell
CC = gcc

PREF_SRC = ./src/
PREF_OBJ = ./obj/

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(SRC:$(PREF_SRC)%.c=$(PREF_OBJ)%.o)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

$(PREF_OBJ)%.o: $(PREF_SRC)%.c
	$(CC) -c $< -o $@

clean:
	rm -f $(TARGET) $(PREF_OBJ)*.o