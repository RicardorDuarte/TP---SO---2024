CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: manager feed

manager: 
	$(CC) $(CFLAGS) -o manager Manager/manager.c Manager/processocom.c Utils/utils.h -pthread

feed: 
	$(CC) $(CFLAGS) -o feed Feed/feed.c Utils/utils.h -pthread

clean:
	rm -f manager feed
	rm FEED*
	rm MANAGER_FI*
