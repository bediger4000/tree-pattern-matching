/* $Id$ */

/*
 *
 */

struct buffer {
	char *buffer;
	int   size;
	int   offset;
};

struct buffer *new_buffer(int desired_size);
void           resize_buffer(struct buffer *b, int increment);
void           buffer_append(struct buffer *b, char *bytes, int length);
void           delete_buffer(struct buffer *b);
