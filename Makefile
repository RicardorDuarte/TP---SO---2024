CC=gcc
CFLAGS=-pthread -Wall -g

# Diretórios de fontes e binários
SRC_DIR=.
OBJ_DIR=obj
EXE=manager

# Arquivos fonte
SOURCES=$(SRC_DIR)/manager.c $(SRC_DIR)/processocom.c $(SRC_DIR)/processoman.c 
OBJECTS=$(SOURCES:.c=.o)

# Regra principal
all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXE)

# Gerar dependências
%.d: %.c
	$(CC) $(CFLAGS) -M $< > $(OBJ_DIR)/$@

-include $(OBJ_DIR)/*.d
