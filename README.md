#### Echipa tema 3 SD:
* tudor_andrei.matei
* id.2

## Task 2
* We will store our posts into an array of posts.
The post structure consists of a generic tree of reposts,
an user id, an id, a title, a like counter and
an array that store the uid's of those who liked
the post.


### Search function
* Searches recursively for a node in a tree and
returns a pointer to it. It's a DFS search.
So it goes through the children of every node until
it gets to a leaf, then goes back (a level up).

### Create post
* Adds a new entry into the posts array, by allocating memory
for a new element in the array and for a new post. Creates a copy
of a the new post, then adds in the new tree that needs to be 
created because of the new post, passing the address of the copy.

### Repost
* It has a similar logic to the previous command, but it will
not add it as the root of a new tree, but as a child of
an already existing node in an already existing tree.

### Common repost
* It looks for the lowest common ancestor in a general tree.
It receives the two report id's, then search the needed
ancestor. We will start our search in the root, then check each
subtree corresponding to its children. If we find the BOTH 
nodes in that subtree, then we continue our search there by
applying the same logic. If we can't find both nodes
in any of those subtrees, then the common repost has already
been found.

### Like
* First of all, we check if the person has already liked
that post, by checking the array of people who liked
the post/repost. If they are not there, then add their
user id in that array and increment the like counter.
It does the opposite if they had already liked the post.

### Ratio
* Uses a function that returns -1 if not ratio'd or
the id of the repost otherwise. The function
goes through every child of the initial post, then compare
their like counter to the initial post's.

### Delete
* It frees the memory of the node corresponding to the
post/repost and its subtree. If it's a post, then
also removes it from the posts array.

### Get likes
* Uses the search function to find the node, then
access it's data field and get the like counter and
print it.

### Get reposts
* Uses the search function to find the node, then
prints the reposts (they are the children of that node).


## Task 3


### Feed

* The program iterates through the posts array, and
checks if there is a connection between the user
and the one who posted or if it was posted
by the user, themselves. The search starts from
the last post (being the most recent).

### View profile

* The program goes through the posts array and
prints them if they were posted by the wanted user.
* Then it searches for other reposts by the same user
in every subtree and prints them as well.

### Friends' repost

* The program will search between the children
of the node that stores the given post id and will
create an array of the uid's of people that reposted.
Then it will check if there is a connection in the graph
between the given user and the person who reposted and
print their name if that's the case.



