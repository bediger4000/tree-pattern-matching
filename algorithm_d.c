/* $Id$ */

/*
 *
 */

#include <stdio.h>
#include <stdlib.h> /* malloc(), realloc() */
#include <string.h> /* strcpy() */

#include <tree.h>
#include <buffer.h>
#include <aho_corasick.h>
#include <algorithm_d.h>


struct stack_elem {
	struct tree *n; /* 1 */
	int state_at_n; /* 2 */
	int visited;    /* 3 */
};

void tabulate(struct gto *g, struct stack_elem *stack,
	int top, int state, int pat_leaf_count, int *count);
void calculate_strings(struct tree *node, struct buffer *buf);

static char **paths = NULL;
static int path_cnt = 0;
static int paths_used = 0;

void
algorithm_d(struct gto *g, struct tree *t, int pat_path_cnt)
{
	int top = 1;
	int next_state;
	int i, node_cnt = 0;
	int subject_node_count = t->tree_size;
	int *count = malloc(subject_node_count * sizeof(int));
	struct stack_elem *stack
		= malloc(subject_node_count * sizeof(struct stack_elem));
	char *p;

	for (i = 0; i < subject_node_count; ++i)
		count[i] = 0;

	renumber(t, &node_cnt);

	next_state = 0;
	p = t->label;
	while ('\0' != *p)
		next_state = g->delta[next_state][(int)*p++];

	stack[top].n = t;
	stack[top].state_at_n = next_state;
	stack[top].visited = 0;

	tabulate(g, stack, top, next_state, pat_path_cnt, count);

	while (top > 0)
	{
		struct tree *next_node, *this_node = stack[top].n;
		int intstate, next_state, this_state = stack[top].state_at_n;
		int visited = stack[top].visited;

		if (visited == 2 || this_node->type == LEAF)
			--top;
		else {
			++visited;
			stack[top].visited = visited;
			intstate = g->delta[this_state][visited == 1?'1':'2'];

			tabulate(g, stack, top, intstate, pat_path_cnt, count);

			next_node = (visited == 1)? this_node->left: this_node->right;
			/* next_state = g->delta[intstate][(int)next_node->label]; */
			next_state = intstate;
			p = next_node->label;
			while ('\0' != *p)
				next_state = g->delta[next_state][(int)*p++];

			++top;
			stack[top].n = next_node;
			stack[top].state_at_n = next_state;
			stack[top].visited = 0;

			tabulate(g, stack, top, next_state, pat_path_cnt, count);
		}
	}

	free(stack);
	free(count);
}
void
renumber(struct tree *node, int *n)
{
	node->tree_size = *n;
	++*n;
	if (INTERIOR == node->type)
	{
		renumber(node->left, n);
		renumber(node->right, n);
	}
}
void
tabulate(struct gto *g, struct stack_elem *stack, int top, int state, int pat_leaf_count, int *count)
{
	int i;
	struct output_extent *oxt;

	if (state < 0)
		return;

	oxt = &(g->output[state]);

	for (i = 0; i < oxt->len; ++i)
	{
		int s = oxt->out[i];
		struct tree *n = stack[top - s + 1].n;
		count[n->tree_size] += 1;

		if (count[n->tree_size] == pat_leaf_count)
		{
			printf("Match pattern with subtree:\n");
			print_tree(n, stdout);
			fputc('\n', stdout);
		}
	}
}

char **
get_pat_paths(void)
{
	return paths;
}

int
set_pattern_paths(struct tree *pattern)
{
	struct buffer *buf = new_buffer(512);
	calculate_strings(pattern, buf);
	delete_buffer(buf);
	return paths_used;
}

void
calculate_strings(struct tree *node, struct buffer *b)
{
	int curr_offset, orig_offset = b->offset;
	char *buf;
	char *pattern_string;

	switch (node->type)
	{
	case INTERIOR:
		buffer_append(b, node->label, strlen(node->label));
		curr_offset = b->offset;

		buffer_append(b, "1", 1);
		calculate_strings(node->left, b);

		b->offset = curr_offset;
		buffer_append(b, "2", 1);

		calculate_strings(node->right, b);

		b->offset = orig_offset;
		break;
	case LEAF:
		buf = b->buffer;
		buf[b->offset] = '\0';
		pattern_string = malloc(b->offset + 1 + 1);

		if ('*' != node->label[0])
			sprintf(pattern_string, "%s%s", buf, node->label);
		else
			sprintf(pattern_string, "%s", buf);

		if (paths_used >= path_cnt)
		{
			char **tmp;
			int alloc_bytes = (sizeof(char *))*(path_cnt + 4);

			if (paths)
				tmp = realloc(paths, alloc_bytes);
			else
				tmp = malloc(alloc_bytes);

			if (tmp)
			{
				paths = tmp;
				path_cnt += 4;
			} else
				fprintf(stderr, "Failed to alloc/realloc pattern paths array, size %d\n", path_cnt + 4);
		}

		/* XXX - If a realloc() fails, this could overwrite paths[] */
		paths[paths_used++] = pattern_string;

		break;
	case UNSET:
		break;
	}
}
