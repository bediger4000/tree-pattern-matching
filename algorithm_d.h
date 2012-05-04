/* $Id$ */

/*
 *
 */

void algorithm_d(struct gto *g, struct tree *subject, int pat_path_cnt);
void renumber(struct tree *node, int *n);
int set_pattern_paths(struct tree *pattern);
char **get_pat_paths(void);
