CC = gcc
CFLAGS = -Wall -Wextra -std=c99

MSG_FICH = fich_mensagens
export MSG_FICH

# Fontes e cabeçalhos
MANAGER_SOURCES = $(wildcard Manager/*.c) $(wildcard Utils/*.c)
MANAGER_HEADERS = $(wildcard Manager/*.h) $(wildcard Utils/*.h)

FEED_SOURCES = $(wildcard Feed/*.c) $(wildcard Utils/*.c)
FEED_HEADERS = $(wildcard Feed/*.h) $(wildcard Utils/*.h)

# Binários
MANAGER_BIN = manager
FEED_BIN = feed

# Alvo padrão
all: $(MANAGER_BIN) $(FEED_BIN)

# Regras para compilar o manager
$(MANAGER_BIN): $(MANAGER_SOURCES) $(MANAGER_HEADERS)
	$(CC) $(CFLAGS) -o $@ $(MANAGER_SOURCES) -lpthread

# Regras para compilar o feed
$(FEED_BIN): $(FEED_SOURCES) $(FEED_HEADERS)
	$(CC) $(CFLAGS) -o $@ $(FEED_SOURCES) -lpthread

# Limpar arquivos compilados
clean:
	rm -f $(MANAGER_BIN) $(FEED_BIN)
