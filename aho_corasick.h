/* $Id$ */

/*
 *
 */
struct output_extent {
	int   *out;   /* array of int, lengths of matched paths */
	int    len;   /* next array element to fill in */
	int    max;   /* number of elements in array */
};

struct gto {
	int **ary;                     /* transition table */
	int   ary_len;                 /* max state currently in table */
	int  *failure;                 /* failure states */
	int **delta;
	struct output_extent *output;  /* output for output states */
	int   output_len;              /* max state for output states */
};

#define FAIL -1

void add_state(struct gto *p, int state, char input, int new_state);
void set_output(struct gto *p, int state, char *keyword);
void construct_goto(char *keywords[], int k, struct gto *g);
void construct_failure(struct gto *g);
void construct_delta(struct gto *g);
struct gto *init_goto(void);
void        destroy_goto(struct gto *);

void perform_match(struct gto *g, FILE *in_stream);
