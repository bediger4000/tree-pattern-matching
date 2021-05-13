/* $Id: tree.c,v 1.1.1.1 2012/05/04 23:44:35 bediger Exp $ */
/*
 * Parse a fully-parenthesized, lisp-like representation of binary trees
 * into a malloc-allocated C struct tree.
 * Not the most robust thing in the world, "a b" (rather than "(a b)")
 * crashes it.  Still, it's compact and fast.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tree.h>

void print_node(struct tree *n, FILE *output_stream);
struct tree *new_node(void);

struct tree *
read_tree(char **str)
{
	int looping = 1;
	struct tree *r = new_node();

	while (looping)
	{
		switch (**str)
		{
		case ' ':  case '\t': case '\n':
			switch (r->type)
			{
			case UNSET:
			case INTERIOR:  /* eat whitespace between leaf-node names */
				break;

			case LEAF:
				r->label[r->labelsz] = '\0';
				looping = 0;
				break;
			}
			++*str;
			break;

		case '(':  /* create an INTERIOR node */
			switch (r->type)
			{
			case UNSET:
				r->type = INTERIOR;
				strcpy(r->label, "@");
				++*str;
				r->left = read_tree(str);
				r->right = read_tree(str);
				break;

			case LEAF:
				r->label[r->labelsz++] = '\0';
				looping = 0;
				/* Doesn't advance *str, so that the calling read_tree()
				 * creates a new INTERIOR node. */
				break;

			case INTERIOR:
				r->right = read_tree(str);
				looping = 0;
				break;
			}
			break;

		case ')':
			switch (r->type)
			{
			case UNSET:
				fprintf(stderr, "): syntax error\n");
				break;

			case LEAF:
				r->label[r->labelsz++] = '\0';
				break;

			case INTERIOR:
				++*str;
				break;
			}
			looping = 0;
			break;

		default:
			/* Not '(', ')' or whitespace */
			switch (r->type)
			{
			/* Cases UNSET and LEAF end up reading alll bytes of a multi-
			 * character leaf-node string into the struct node's label field.
			 */
			case UNSET:  /* hits this first time through */
				r->type = LEAF;
				/* It's supposed to fall through to case LEAF */
			case LEAF:   /* hits this every other time through */
				r->label[r->labelsz++] = **str;
				if (**str == '\0')
					looping = 0;
				(*str)++;
				break;

			case INTERIOR:
				fprintf(stderr, "'%c': syntax error\n", **str);
				++*str;
				break;
			}
			break;
		}

	}

	r->tree_size = 1;
	if (INTERIOR == r->type)
		r->tree_size += r->left->tree_size + r->right->tree_size;

	return r;
}

void
print_tree(struct tree *n, FILE *output_stream)
{
	print_node(n, output_stream);
	printf("\n");
}

void
print_node(struct tree *n, FILE *output_stream)
{
	switch (n->type)
	{
	case LEAF:
		fprintf(output_stream, "%s", n->label);
		break;
	case INTERIOR:
		printf("(");
		print_node(n->left, output_stream);
		printf(" ");
		print_node(n->right, output_stream);
		printf(")");
		break;
	case UNSET:
		fprintf(stderr, "Syntax error: printing UNSET node\n");
		break;
	}
}

struct tree *
new_node(void)
{
	struct tree *r = malloc(sizeof(*r));

	r->type = UNSET;
	r->left = r->right = NULL;
	r->label[0] = '\0';
	r->labelsz = 0;
	r->tree_size = 0;

	return r;
}

void
free_tree(struct tree *node)
{
	if (node->left) free_tree(node->left);
	if (node->right) free_tree(node->right);
	node->left = node->right = NULL;
	node->label[0] = '\0';
	free(node);
}

