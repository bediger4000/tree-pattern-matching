/* $Id$ */

/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <aho_corasick.h>
#include <cb.h>

void set_output_length(struct gto *p, int state, int node_count);

struct gto *
init_goto()
{
	int i;
	struct gto *g = NULL;

	g = malloc(sizeof(*g));

	g->ary = malloc(sizeof(int *));

	g->ary[0] = malloc(128*sizeof(int));

	g->ary_len = 1;

	for (i = 0; i < 128; ++i)
		g->ary[0][i] = FAIL;

	g->output = malloc(sizeof(*g->output));
	g->output_len = 1;

	g->output->len = g->output->max = 0;
	g->output->out = NULL;

	g->delta = NULL;
	g->failure = NULL;

	return g;
}

void
construct_goto(char *keywords[], int k, struct gto *g)
{
	int newstate = 0;
	int i;

	for (i = 0; i < k; ++i)
	{
		int state, j, p;

		/* procedure enter() */

		state = 0;
		j = 0;

		while (
			FAIL != (
				(state<g->ary_len) ?
					g->ary[state][(int)keywords[i][j]] :
					(assert(state>=g->ary_len),FAIL)
				)
			)
		{
			state = ((state<g->ary_len) ?
						g->ary[state][(int)keywords[i][j]] :
						(assert(state>=g->ary_len),FAIL)
					);
			++j;
		}

		for (p = j; '\0' != keywords[i][p]; ++p)
		{
			++newstate;
			add_state(g, state, keywords[i][p], newstate);
			state = newstate;
		}

		/* end procedure enter() */

		set_output(g, state, keywords[i]);
	}

	for (i = 0; i < 128; ++i)
	{
		if (FAIL == g->ary[0][i])
			g->ary[0][i] = 0;
	}
}

void
set_output(struct gto *p, int state, char *keyword)
{
	size_t kwl = strlen(keyword);
	unsigned int i;
	int node_count = 0;

	/* Slightly weird: count the number of nodes in
	 * the "keyword", which is actually a string, from
	 * root to leaf of a path through a pattern tree. */
	for (i = 0; i < kwl; ++i)
	{
		/* Skip the characters '1' and '2': they're
		 * the characters in the string indicating left
		 * or right branch at an application node. */
		if ('1' != keyword[i] && '2' != keyword[i])
		{
			++node_count;

			/* If you hit a keyword that doesn't begin
			 * with '@', you've hit a leaf node.  We've
			 * counted it, so now we break out of the loop. */
			if (keyword[i] != '@')
				break;
		}
	}

	set_output_length(p, state, node_count);
}

void
set_output_length(struct gto *p, int state, int node_count)
{
	struct output_extent *oxt;

	/* value of node_count stored against the match */

	if (state >= p->output_len)
	{
		int n = state - p->output_len + 1;  /* how many more to add */
		int l = p->output_len + n;          /* how many total after add */
		int i;

		p->output = realloc(p->output, l*sizeof(struct output_extent));

		/* init only the new structs output_extent, leave previous ones alone */
		for (i = p->output_len; i < l; ++i)
		{
			p->output[i].len = 0;
			p->output[i].max = 0;
			p->output[i].out = NULL;
		}

		p->output_len += n;  /* bumped up the number of structs output_extent */
	}

	/* o comprises the lengths of paths in pattern matched in subject */
	oxt = &(p->output[state]);

	if (oxt->len >= oxt->max)
	{
		oxt->max += 4;
		if (oxt->out)
			oxt->out = realloc(oxt->out, oxt->max * sizeof(int));
		else
			oxt->out = malloc(oxt->max * sizeof(int));
	}

	oxt->out[oxt->len++] = node_count;

	/* state has o->len matches now */
}


void
add_state(struct gto *p, int state, char input, int new_state)
{
	if (state >= p->ary_len || new_state >= p->ary_len)
	{
		int i, n;

		/* by using something other than 1, would it jack up array length
		 * more than a single entry at a time? */
		n = 1 + ((new_state > state? new_state: state) - p->ary_len);

		p->ary = realloc(p->ary, sizeof(int *)*(p->ary_len + n));

		for (i = p->ary_len; i < p->ary_len + n; ++i)
		{
			int j;


			/* if this ends up being too costly, could increase array size
			 * by more than one each time, and malloc loads of rows at once */
			p->ary[i] = malloc(sizeof(int)*128);

			for (j = 0; j < 128; ++j)
				p->ary[i][j] = FAIL;
		}

		p->ary_len += n;
	}

	p->ary[state][(int)input] = new_state;
}

void
construct_failure(struct gto *g)
{
	int i;
	struct queue *q;


	g->failure = malloc(g->ary_len*sizeof(int));

	for (i = 0; i < g->ary_len; ++i)
		g->failure[i] = 0;

	q = queueinit();

	for (i = 0; i < 128; ++i)
	{
		int s = g->ary[0][i];

		if (0 != s)
		{
			enqueue(q, s);
			g->failure[s] = 0;
		}
	}

	while (!queueempty(q))
	{
		int a;
		int r = dequeue(q);

		for (a = 0; a < 128; ++a)
		{
			int s = g->ary[r][a];

			if (FAIL != s)
			{
				int state;
				struct output_extent *p;

				enqueue(q, s);

				state = g->failure[r];
				while (FAIL == g->ary[state][a])
					state = g->failure[state];

				g->failure[s] = g->ary[state][a];

				/* output(s) <- output(s) U output(f(s)) */
				p = &g->output[g->failure[s]];

				for (i = 0; i < p->len; ++i)
					set_output_length(g, s, p->out[i]);
			}
		}
	}

	queuedestroy(q);
}

void
perform_match(struct gto *g, FILE *in)
{
	int state = 0;
	int n = 1;
	int a;

	while ((a = fgetc(in)) != -1)
	{
		state = g->delta[state][a];

		if (0 < g->output[state].len)
		{
			int i;

			for (i = 0; i < g->output[state].len; ++i)
				printf("match path of length %d at line %d\n",
					g->output[state].out[i], n);
		}
	}
}

void
destroy_goto(struct gto *p)
{
	int i;

	for (i = 0; i < p->ary_len; ++i)
		free(p->ary[i]);

	free(p->ary);

	for (i = 0; i < p->output_len; ++i)
		free(p->output[i].out);

	if (NULL != p->output) free(p->output);
	if (NULL != p->failure) free(p->failure);
	if (NULL != p->delta)
	{
		if (NULL != p->delta[0]) free(p->delta[0]);
		free(p->delta);
	}

	free(p);
}

void
construct_delta(struct gto *g)
{
	struct queue *q;
	int i, a;

	g->delta = malloc(sizeof(int *)*g->ary_len);

	g->delta[0] = malloc(sizeof(int)*g->ary_len*128);

	memset(g->delta[0], 0, sizeof(int)*g->ary_len*128);

	for (i = 0; i < g->ary_len; ++i)
		g->delta[i] = g->delta[0] + i*128;

	q = queueinit();

	for (a = 0; a < 128; ++a)
	{
		g->delta[0][a] = g->ary[0][a];

		if (0 != g->ary[0][a])
			enqueue(q, g->ary[0][a]);
	}

	while (!queueempty(q))
	{
		int r = dequeue(q);

		for (a = 0; a < 128; ++a)
		{
			int s = g->ary[r][a];

			if (FAIL != s)
			{
				enqueue(q, s);
				g->delta[r][a] = s;
			} else
				g->delta[r][a] = g->delta[g->failure[r]][a];
		}
	}

	queuedestroy(q);
}
