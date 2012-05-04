/* $Id$ */

/*
 *
 */

#define CBUFSIZE 0x200  /* 512 */
#define CBUFMASK 0x1ff  /* 512 - 1 */

struct queue_node {
	int state;
	struct queue_node *next;
};

struct queue {
	int *cbuf;
	int  in;
	int  out;
	struct queue_node *ovfhead;
	struct queue_node *ovftail;
};


struct queue *queueinit(void);
void queuedestroy(struct queue *);
void enqueue(struct queue *, int);
int  dequeue(struct queue *);
int  queueempty(struct queue *);
void empty_error(struct queue *);
