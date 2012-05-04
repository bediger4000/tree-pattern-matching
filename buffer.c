/* $Id$ */

/*
 *
 */

#include <stdio.h>    /* NULL manifest constant */
#include <stdlib.h>   /* malloc(), free(), realloc() */
#include <string.h>   /* memcpy() */

#include <buffer.h>

struct buffer *
new_buffer(int desired_size)
{
	struct buffer *r = malloc(sizeof *r);

	if (r)
	{
		r->buffer = malloc(desired_size);
		if (r->buffer)
			r->size = desired_size;
		else
			r->size = 0;
		r->offset = 0;
	}

	return r;
}

void
delete_buffer(struct buffer *b)
{
	if (b)
	{
		if (b->buffer)
		{
			free(b->buffer);
			b->buffer = NULL;
		}
		b->offset = b->size = 0;
		free(b);
		b = NULL;
	}
}

void
resize_buffer(struct buffer *b, int increment)
{
	if (b)
	{
		char *reallocated_buffer;
		int new_size = b->size + increment;

		reallocated_buffer = realloc(b->buffer, new_size);

		if (reallocated_buffer)
		{
			b->buffer = reallocated_buffer;
			b->size   = new_size;
		}
	}
}

void
buffer_append(struct buffer *b, char *bytes, int length)
{
	if (NULL != b && NULL != bytes && 0 < length)
	{
		if (length >= (b->size - b->offset))
			resize_buffer(b, length);

		if (length < b->size - b->offset)
		{
			/* resize_buffer() might fail */
			memcpy(&b->buffer[b->offset], bytes, length);
			b->offset += length;
		}
	}
}
