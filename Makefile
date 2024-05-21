CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

.PHONY: all build clean

all: build

build: friends posts feed

# Object files
UTILS = users.o
COMMON_OBJS = posts.o
FRIENDS_OBJS = $(UTILS) friends.o social_media_friends.o $(COMMON_OBJS)
POSTS_OBJS = $(UTILS) posts.o social_media_posts.o
FEED_OBJS = $(UTILS) posts.o friends.o feed.o social_media_feed.o $(COMMON_OBJS)

friends: $(FRIENDS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

posts: $(POSTS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

feed: $(FEED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile social_media.c with different TASK macros
social_media_friends.o: social_media.c
	$(CC) $(CFLAGS) -c -D TASK_1 -o $@ $<

social_media_posts.o: social_media.c
	$(CC) $(CFLAGS) -c -D TASK_2 -o $@ $<

social_media_feed.o: social_media.c
	$(CC) $(CFLAGS) -c -D TASK_1 -D TASK_2 -D TASK_3 -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o friends posts feed
