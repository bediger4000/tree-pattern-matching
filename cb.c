/* $Id$ */

/*
 * A first-in, first-out (FIFO) queue, implemented
 * as a circular buffer with a 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <cb.h>

struct queue *
queueinit()
{
	struct queue *r;

	r = malloc(sizeof(*r));
	assert(NULL != r);

	r->cbuf = malloc(CBUFSIZE*sizeof(int));
	assert(NULL != r->cbuf);

	r->in = 0;
	r->out = 0;
	r->ovfhead = r->ovftail = NULL;

	return r;
}

void
empty_error(struct queue *q)
{
	fprintf(stderr, "Tried to get off empty queue: in %d, out %d\n", q->in, q->out);
	exit(9);
}

void
enqueue(struct queue *q, int state)
{
	struct queue_node *newnode;

	assert(NULL != q);

	if (((q->in + 1)&CBUFMASK) != q->out)
	{
		assert(NULL != q->cbuf);
		q->cbuf[q->in] = state;
		q->in = (q->in+1)&CBUFMASK;

	} else {

		newnode = malloc(sizeof *newnode);
		assert(NULL != newnode);
		newnode->state = state;
		newnode->next = NULL;

		if (NULL == q->ovftail)
		{
			q->ovfhead = q->ovftail = newnode;
		} else {
			q->ovftail->next = newnode;
			q->ovftail = newnode;
		}
	}
}

int
queueempty(struct queue *q)
{
	assert(NULL != q);
	return (q->in == q->out);
}

int
dequeue(struct queue *q)
{
	int r;

	assert(NULL != q);
	if (q->in == q->out) empty_error(q);

	assert(NULL != q->cbuf);
	r = q->cbuf[q->out];

	q->out = (q->out + 1)&CBUFMASK;

	if (NULL != q->ovfhead)
	{
		struct queue_node *tmp;

		q->cbuf[q->in]= q->ovfhead->state;
		q->in = (q->in+1)&CBUFMASK;
		tmp = q->ovfhead;
		q->ovfhead = q->ovfhead->next;
		if (NULL == q->ovfhead) q->ovftail = NULL;
		free(tmp);
	}

	return r;
}

void
queuedestroy(struct queue *q)
{
	assert(NULL != q);

	if (NULL != q->cbuf) free(q->cbuf);

	while (NULL != q->ovfhead)
	{
		struct queue_node *t = q->ovfhead->next;

		free(q->ovfhead);

		q->ovfhead = t;
	}

	free(q);
}
