CC=gcc
CFLAGS=-Wall -Wextra -Werror -g
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

.PHONY: build clean
.PHONY: all build clean

all: build

build: friends posts feed

# Any other object file that is needed for the other files
# add it here, separated by space
# Object files
UTILS = users.o
COMMON_OBJS = posts.o
FRIENDS_OBJS = $(UTILS) friends.o social_media_friends.o $(COMMON_OBJS)
POSTS_OBJS = $(UTILS) posts.o social_media_posts.o
FEED_OBJS = $(UTILS) posts.o friends.o feed.o social_media_feed.o $(COMMON_OBJS)

friends: $(UTILS) friends.o social_media_friends.o
friends: $(FRIENDS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

posts: $(UTILS) posts.o social_media_posts.o
posts: $(POSTS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	
feed: $(UTILS) posts.o friends.o feed.o social_media_feed.o

feed: $(FEED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

social_media_friends.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -o $@ social_media.c
# Compile social_media.c with different TASK macros
social_media_friends.o: social_media.c
	$(CC) $(CFLAGS) -c -D TASK_1 -o $@ $<

social_media_posts.o: social_media.c
	$(CC) $(CFLAGS) -c -D TASK_2 -o $@ $<

social_media_posts.o:
	$(CC) $(CFLAGS) -c -D TASK_2 -o $@ social_media.c
social_media_feed.o: social_media.c
	$(CC) $(CFLAGS) -c -D TASK_1 -D TASK_2 -D TASK_3 -o $@ $<

social_media_feed.o:
	$(CC) $(CFLAGS) -c -D TASK_1 -D TASK_2 -D TASK_3 -o $@ social_media.c
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o friends posts feed
	rm -f *.o friends posts feed