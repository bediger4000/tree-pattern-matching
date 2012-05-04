/* $Id$ */

/*
 *
 */

enum node_type {UNSET, LEAF, INTERIOR};

struct tree {
    enum node_type type;
    char   label[64];
	int    labelsz;
	int tree_size;
    struct tree *left;
    struct tree *right;
};

struct tree *read_tree(char **pointer_to_buffer);
void free_tree(struct tree *node);
void print_tree(struct tree *root, FILE *output_stream);
