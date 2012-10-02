#include <stdio.h>

#include <tree.h>

int count_nodes(struct tree *root);

int
main(int ac, char **av)
{
	int n;
	struct tree *root = read_tree(&av[1]);

	n = count_nodes(root);

	printf("Found %d nodes\n", n);

	free_tree(root);

	return 0;
}

int
count_nodes(struct tree *root)
{
	struct {
		struct tree *node;
		int visit_count;
	} stack[1024];
	int stack_top = 0; 
	int count = 0;

	stack[stack_top].node = root;
	stack[stack_top].visit_count = 0;

	while (stack_top >= 0)
	{
		struct tree *curr = stack[stack_top].node;

		if (curr->type == LEAF)
		{
			--stack_top;
			++count;
		} else {
			switch (stack[stack_top].visit_count)
			{
			case 0:
				stack[stack_top].visit_count = 1;
				++stack_top;
				stack[stack_top].node = curr->left;
				stack[stack_top].visit_count = 0;
				break;
			case 1:
				stack[stack_top].visit_count = 2;
				++stack_top;
				stack[stack_top].node = curr->right;
				stack[stack_top].visit_count = 0;
				break;
			default:
				++count;
				--stack_top;
				break;
			}
		}
	}

	return count;
}
