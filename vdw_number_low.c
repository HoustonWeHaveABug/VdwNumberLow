#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	int choices_start;
	int choices_end;
	int color_max;
	int color_prev;
	int root_idx;
	int r;
}
roll_t;

typedef struct {
	int is_valid_color_start;
	int color;
	int rolls_idx;
}
unroll1_t;

typedef struct {
	int choices_start;
	int choices_end;
	int rolls_root_idx;
	int rolls_idx;
}
unroll2_t;

typedef enum {
	CALL_ROLL,
	CALL_UNROLL1,
	CALL_UNROLL2
}
call_t;

int add_roll(int, int, int, int, int);
void set_roll(roll_t *, int, int, int, int, int);
int add_unroll1(int, int, int);
void set_unroll1(unroll1_t *, int, int, int);
int add_unroll2(int, int, int, int);
void set_unroll2(unroll2_t *, int, int, int, int);
int add_call(call_t);
int perform_call(call_t *);
int perform_roll(int, int, int, int, int, int);
int fatal_progression(int, int, int, int, int);
int last_progression(int, int, int);
int perform_unroll1(int, int, int);
int perform_unroll2(int, int, int, int);

int colors_n, progression_len, calls_max, calls_size, rolls_max, rolls_size, unrolls1_max, unrolls1_size, unrolls2_max, unrolls2_size, choices_max, *choices, *is_valid_color, *color_switch, *valid_colors_n;
unsigned time0;
unsigned long long nodes;
roll_t *rolls;
unroll1_t *unrolls1;
unroll2_t *unrolls2;
call_t *calls;

int main(void) {
	int r;
	if (scanf("%d%d", &colors_n, &progression_len) != 2 || colors_n < 1 || progression_len < 2) {
		fprintf(stderr, "Invalid parameters\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	calls_max = 0;
	calls_size = 0;
	rolls_max = 0;
	rolls_size = 0;
	unrolls1_max = 0;
	unrolls1_size = 0;
	unrolls2_max = 0;
	unrolls2_size = 0;
	choices_max = 0;
	nodes = 0;
	time0 = (unsigned)time(NULL);
	if (!add_roll(0, 0, 1, -1, -1)) {
		return EXIT_FAILURE;
	}
	do {
		calls_size--;
		r = perform_call(calls+calls_size);
	}
	while (r && calls_size > 0);
	printf("W(%d, %d) = %d Nodes %llu Time %us\n", colors_n, progression_len, choices_max, nodes, (unsigned)time(NULL)-time0);
	fflush(stdout);
	if (choices_max > 0) {
		free(valid_colors_n);
		free(color_switch);
		free(is_valid_color);
		free(choices);
	}
	if (unrolls2_max > 0) {
		free(unrolls2);
	}
	if (unrolls1_max > 0) {
		free(unrolls1);
	}
	if (rolls_max > 0) {
		free(rolls);
	}
	if (calls_max > 0) {
		free(calls);
	}
	return EXIT_SUCCESS;
}

int add_roll(int choices_start, int choices_end, int color_max, int color_prev, int root_idx) {
	if (!add_call(CALL_ROLL)) {
		return 0;
	}
	if (rolls_size == rolls_max) {
		if (rolls_max == 0) {
			rolls = malloc(sizeof(roll_t));
			if (!rolls) {
				fprintf(stderr, "Could not allocate memory for rolls\n");
				fflush(stderr);
				return 0;
			}
		}
		else {
			roll_t *rolls_tmp = realloc(rolls, sizeof(roll_t)*(size_t)(rolls_max+1));
			if (!rolls_tmp) {
				fprintf(stderr, "Could not reallocate memory for rolls\n");
				fflush(stderr);
				return 0;
			}
			rolls = rolls_tmp;
		}
		rolls_max++;
	}
	set_roll(rolls+rolls_size, choices_start, choices_end, color_max, color_prev, root_idx);
	rolls_size++;
	return 1;
}

void set_roll(roll_t *roll, int choices_start, int choices_end, int color_max, int color_prev, int root_idx) {
	roll->choices_start = choices_start;
	roll->choices_end = choices_end;
	roll->color_max = color_max;
	roll->color_prev = color_prev;
	roll->root_idx = root_idx;
}

int add_unroll1(int is_valid_color_start, int color, int rolls_idx) {
	if (!add_call(CALL_UNROLL1)) {
		return 0;
	}
	if (unrolls1_size == unrolls1_max) {
		if (unrolls1_max == 0) {
			unrolls1 = malloc(sizeof(unroll1_t));
			if (!unrolls1) {
				fprintf(stderr, "Could not allocate memory for unrolls1\n");
				fflush(stderr);
				return 0;
			}
		}
		else {
			unroll1_t *unrolls1_tmp = realloc(unrolls1, sizeof(unroll1_t)*(size_t)(unrolls1_max+1));
			if (!unrolls1_tmp) {
				fprintf(stderr, "Could not reallocate memory for unrolls1\n");
				fflush(stderr);
				return 0;
			}
			unrolls1 = unrolls1_tmp;
		}
		unrolls1_max++;
	}
	set_unroll1(unrolls1+unrolls1_size, is_valid_color_start, color, rolls_idx);
	unrolls1_size++;
	return 1;
}

void set_unroll1(unroll1_t *unroll1, int is_valid_color_start, int color, int rolls_idx) {
	unroll1->is_valid_color_start = is_valid_color_start;
	unroll1->color = color;
	unroll1->rolls_idx = rolls_idx;
}

int add_unroll2(int choices_start, int choices_end, int rolls_root_idx, int rolls_idx) {
	if (!add_call(CALL_UNROLL2)) {
		return 0;
	}
	if (unrolls2_size == unrolls2_max) {
		if (unrolls2_max == 0) {
			unrolls2 = malloc(sizeof(unroll2_t));
			if (!unrolls2) {
				fprintf(stderr, "Could not allocate memory for unrolls2\n");
				fflush(stderr);
				return 0;
			}
		}
		else {
			unroll2_t *unrolls2_tmp = realloc(unrolls2, sizeof(unroll2_t)*(size_t)(unrolls2_max+1));
			if (!unrolls2_tmp) {
				fprintf(stderr, "Could not reallocate memory for unrolls2\n");
				fflush(stderr);
				return 0;
			}
			unrolls2 = unrolls2_tmp;
		}
		unrolls2_max++;
	}
	set_unroll2(unrolls2+unrolls2_size, choices_start, choices_end, rolls_root_idx, rolls_idx);
	unrolls2_size++;
	return 1;
}

void set_unroll2(unroll2_t *unroll2, int choices_start, int choices_end, int rolls_root_idx, int rolls_idx) {
	unroll2->choices_start = choices_start;
	unroll2->choices_end = choices_end;
	unroll2->rolls_root_idx = rolls_root_idx;
	unroll2->rolls_idx = rolls_idx;
}

int add_call(call_t call) {
	if (calls_size == calls_max) {
		if (calls_max == 0) {
			calls = malloc(sizeof(call_t));
			if (!calls) {
				fprintf(stderr, "Could not allocate memory for calls\n");
				fflush(stderr);
				return 0;
			}
		}
		else {
			call_t *calls_tmp = realloc(calls, sizeof(call_t)*(size_t)(calls_max+1));
			if (!calls_tmp) {
				fprintf(stderr, "Could not reallocate memory for calls\n");
				fflush(stderr);
				return 0;
			}
			calls = calls_tmp;
		}
		calls_max++;
	}
	calls[calls_size++] = call;
	return 1;
}

int perform_call(call_t *call) {
	int r;
	switch (*call) {
	case CALL_ROLL:
		rolls_size--;
		r = perform_roll(rolls[rolls_size].choices_start, rolls[rolls_size].choices_end, rolls[rolls_size].color_max, rolls[rolls_size].color_prev, rolls[rolls_size].root_idx, rolls_size);
		break;
	case CALL_UNROLL1:
		unrolls1_size--;
		r = perform_unroll1(unrolls1[unrolls1_size].is_valid_color_start, unrolls1[unrolls1_size].color, unrolls1[unrolls1_size].rolls_idx);
		break;
	case CALL_UNROLL2:
		unrolls2_size--;
		r = perform_unroll2(unrolls2[unrolls2_size].choices_start, unrolls2[unrolls2_size].choices_end, unrolls2[unrolls2_size].rolls_root_idx, unrolls2[unrolls2_size].rolls_idx);
		break;
	default:
		fprintf(stderr, "Invalid call\n");
		fflush(stderr);
		r = 0;
	}
	return r;
}

int perform_roll(int choices_start, int choices_end, int color_max, int color_prev, int root_idx, int rolls_idx) {
	int choices_size, is_valid_color_start, choices_offset, delta_lo, delta_hi, delta_hi_mod, delta_max, color_hi, choices_idx, is_valid_color_idx;
	if (color_prev > -1) {
		if (root_idx > -1 && rolls[root_idx].r < choices_start-1) {
			return 1;
		}
		choices[choices_start-1] = color_prev;
	}
	nodes++;
	if (choices_start == choices_max) {
		if (choices_max == 0) {
			choices = malloc(sizeof(int));
			if (!choices) {
				fprintf(stderr, "Could not allocate memory for choices\n");
				fflush(stderr);
				return 0;
			}
			is_valid_color = malloc(sizeof(int)*(size_t)colors_n);
			if (!is_valid_color) {
				fprintf(stderr, "Could not allocate memory for is_valid_color\n");
				fflush(stderr);
				free(choices);
				return 0;
			}
			color_switch = malloc(sizeof(int)*(size_t)colors_n);
			if (!color_switch) {
				fprintf(stderr, "Could not allocate memory for color_switch\n");
				fflush(stderr);
				free(is_valid_color);
				free(choices);
				return 0;
			}
			valid_colors_n = malloc(sizeof(int));
			if (!valid_colors_n) {
				fprintf(stderr, "Could not allocate memory for valid_colors_n\n");
				fflush(stderr);
				free(color_switch);
				free(is_valid_color);
				free(choices);
				return 0;
			}
		}
		else {
			int *choices_tmp, *is_valid_color_tmp, *color_switch_tmp, *valid_colors_n_tmp;
			choices_tmp = realloc(choices, sizeof(int)*(size_t)(choices_max+1));
			if (!choices_tmp) {
				fprintf(stderr, "Could not reallocate memory for choices\n");
				fflush(stderr);
				return 0;
			}
			choices = choices_tmp;
			is_valid_color_tmp = realloc(is_valid_color, sizeof(int)*(size_t)((choices_max+1)*colors_n));
			if (!is_valid_color_tmp) {
				fprintf(stderr, "Could not reallocate memory for is_valid_color\n");
				fflush(stderr);
				return 0;
			}
			is_valid_color = is_valid_color_tmp;
			color_switch_tmp = realloc(color_switch, sizeof(int)*(size_t)((choices_max+1)*colors_n));
			if (!color_switch_tmp) {
				fprintf(stderr, "Could not reallocate memory for color_switch\n");
				fflush(stderr);
				return 0;
			}
			color_switch = color_switch_tmp;
			valid_colors_n_tmp = realloc(valid_colors_n, sizeof(int)*(size_t)(choices_max+1));
			if (!valid_colors_n_tmp) {
				fprintf(stderr, "Could not reallocate memory for valid_colors_n\n");
				fflush(stderr);
				return 0;
			}
			valid_colors_n = valid_colors_n_tmp;
		}
		printf("W(%d, %d) > %d Nodes %llu Time %us\n", colors_n, progression_len, choices_max++, nodes, (unsigned)time(NULL)-time0);
		fflush(stdout);
	}
	choices_size = choices_max-choices_start;
	is_valid_color_start = choices_start*colors_n;
	choices_offset = 0;
	delta_lo = 1;
	delta_hi = choices_start/(progression_len-1);
	delta_hi_mod = choices_start%(progression_len-1);
	if (delta_hi_mod == 0) {
		delta_max = delta_hi;
	}
	else {
		delta_max = delta_hi+1;
	}
	color_hi = color_max;
	for (choices_idx = 0, is_valid_color_idx = 0; choices_idx < choices_end; choices_idx++, is_valid_color_idx += colors_n) {
		int delta, color;
		for (delta = delta_lo; delta <= choices_idx+1; delta++) {
			if (fatal_progression(choices_start, choices_idx, is_valid_color_start, is_valid_color_idx, delta)) {
				rolls[rolls_idx].r = last_progression(is_valid_color_start, is_valid_color_idx, color_hi);
				return perform_unroll2(choices_start, choices_start+choices_idx+1, root_idx, rolls_idx);
			}
		}
		if (choices_idx+1 == delta_max) {
			delta_max++;
		}
		for (delta = delta_max; delta <= delta_hi; delta++) {
			if (fatal_progression(choices_start, choices_idx, is_valid_color_start, is_valid_color_idx, delta)) {
				rolls[rolls_idx].r = last_progression(is_valid_color_start, is_valid_color_idx, color_hi);
				return perform_unroll2(choices_start, choices_start+choices_idx+1, root_idx, rolls_idx);
			}
		}
		if (valid_colors_n[choices_start+choices_idx] == 1 && delta_lo == 1) {
			for (color = 0; color < color_hi && is_valid_color[is_valid_color_start+is_valid_color_idx+color] > -1; color++);
			choices[choices_start+choices_idx] = color;
			choices_offset++;
			delta_hi_mod++;
			if (delta_hi_mod == progression_len-1) {
				delta_hi++;
				delta_hi_mod = 0;
			}
			if (color_max < colors_n && color+1 == color_max) {
				color_max++;
			}
		}
		else {
			choices[choices_start+choices_idx] = colors_n;
			delta_lo++;
		}
		if (color_hi < colors_n && is_valid_color[is_valid_color_start+is_valid_color_idx+color_hi-1] == -1) {
			color_hi++;
		}
	}
	for (; choices_idx < choices_size && delta_lo <= delta_hi; choices_idx++, is_valid_color_idx += colors_n) {
		int color, delta;
		for (color = 0; color < color_hi; color++) {
			is_valid_color[is_valid_color_start+is_valid_color_idx+color] = -1;
			color_switch[is_valid_color_start+is_valid_color_idx+color] = -1;
		}
		for (; color < colors_n; color++) {
			is_valid_color[is_valid_color_start+is_valid_color_idx+color] = choices_start+choices_idx;
			color_switch[is_valid_color_start+is_valid_color_idx+color] = choices_start;
		}
		valid_colors_n[choices_start+choices_idx] = color_hi;
		for (delta = delta_lo; delta <= delta_hi; delta++) {
			if (fatal_progression(choices_start, choices_idx, is_valid_color_start, is_valid_color_idx, delta)) {
				rolls[rolls_idx].r = last_progression(is_valid_color_start, is_valid_color_idx, color_hi);
				return perform_unroll2(choices_start, choices_start+choices_idx+1, root_idx, rolls_idx);
			}
		}
		if (valid_colors_n[choices_start+choices_idx] == 1 && delta_lo == 1) {
			for (color = 0; color < color_hi && is_valid_color[is_valid_color_start+is_valid_color_idx+color] > -1; color++);
			choices[choices_start+choices_idx] = color;
			choices_offset++;
			delta_hi_mod++;
			if (delta_hi_mod == progression_len-1) {
				delta_hi++;
				delta_hi_mod = 0;
			}
			if (color_max < colors_n && color+1 == color_max) {
				color_max++;
			}
		}
		else {
			choices[choices_start+choices_idx] = colors_n;
			delta_lo++;
		}
		if (color_hi < colors_n && is_valid_color[is_valid_color_start+is_valid_color_idx+color_hi-1] == -1) {
			color_hi++;
		}
	}
	if (choices_idx == 0) {
		int color;
		for (color = 0; color < color_hi; color++) {
			is_valid_color[is_valid_color_start+color] = -1;
			color_switch[is_valid_color_start+color] = -1;
		}
		for (; color < colors_n; color++) {
			is_valid_color[is_valid_color_start+color] = choices_start;
			color_switch[is_valid_color_start+color] = choices_start;
		}
		valid_colors_n[choices_start] = color_hi;
		if (valid_colors_n[choices_start] == 1) {
			choices[choices_start] = 0;
			choices_offset++;
			if (color_max < colors_n && 1 == color_max) {
				color_max++;
			}
		}
		else {
			choices[choices_start] = colors_n;
		}
		choices_idx = 1;
	}
	if (!add_unroll2(choices_start, choices_start+choices_idx, root_idx, rolls_idx)) {
		return 0;
	}
	choices_start += choices_offset;
	if (choices_offset < choices_idx) {
		int color;
		is_valid_color_start += choices_offset*colors_n;
		choices_offset++;
		rolls[rolls_idx].r = choices_start;
		if (!add_unroll1(is_valid_color_start, color_max-1, rolls_idx)) {
			return 0;
		}
		color = color_max-1;
		if (is_valid_color[is_valid_color_start+color] == -1) {
			if (color_max < colors_n) {
				if (!add_roll(choices_start+1, choices_idx-choices_offset, color_max+1, color, rolls_idx)) {
					return 0;
				}
			}
			else {
				if (!add_roll(choices_start+1, choices_idx-choices_offset, color_max, color, rolls_idx)) {
					return 0;
				}
			}
		}
		for (color--; color >= 0; color--) {
			if (!add_unroll1(is_valid_color_start, color, rolls_idx)) {
				return 0;
			}
			if (is_valid_color[is_valid_color_start+color] == -1) {
				if (!add_roll(choices_start+1, choices_idx-choices_offset, color_max, color, rolls_idx)) {
					return 0;
				}
			}
		}
	}
	else {
		if (!add_roll(choices_start, choices_idx-choices_offset, color_max, -1, rolls_idx)) {
			return 0;
		}
	}
	return 1;
}

int fatal_progression(int choices_start, int choices_idx, int is_valid_color_start, int is_valid_color_idx, int delta) {
	int color = choices[choices_start+choices_idx-delta];
	if (is_valid_color[is_valid_color_start+is_valid_color_idx+color] == -1) {
		int i;
		for (i = 2; i < progression_len && choices[choices_start+choices_idx-delta*i] == color; i++);
		if (i == progression_len) {
			is_valid_color[is_valid_color_start+is_valid_color_idx+color] = choices_start+choices_idx-delta;
			color_switch[is_valid_color_start+is_valid_color_idx+color] = choices_start;
			valid_colors_n[choices_start+choices_idx]--;
		}
	}
	return valid_colors_n[choices_start+choices_idx] == 0;
}

int last_progression(int is_valid_color_start, int is_valid_color_idx, int color_hi) {
	int r = is_valid_color[is_valid_color_start+is_valid_color_idx], color;
	for (color = 1; color < color_hi; color++) {
		if (r < is_valid_color[is_valid_color_start+is_valid_color_idx+color]) {
			r = is_valid_color[is_valid_color_start+is_valid_color_idx+color];
		}
	}
	return r;
}

int perform_unroll1(int is_valid_color_start, int color, int rolls_idx) {
	if (rolls[rolls_idx].r < is_valid_color[is_valid_color_start+color]) {
		rolls[rolls_idx].r = is_valid_color[is_valid_color_start+color];
	}
	return 1;
}

int perform_unroll2(int choices_start, int choices_end, int root_idx, int rolls_idx) {
	int choices_idx, is_valid_color_idx;
	for (choices_idx = choices_start, is_valid_color_idx = choices_start*colors_n; choices_idx < choices_end; choices_idx++, is_valid_color_idx += colors_n) {
		int color;
		for (color = 0; color < colors_n; color++) {
			if (color_switch[is_valid_color_idx+color] == choices_start) {
				valid_colors_n[choices_idx]++;
				color_switch[is_valid_color_idx+color] = -1;
				is_valid_color[is_valid_color_idx+color] = -1;
			}
		}
	}
	if (root_idx > 1) {
		rolls[root_idx].r = rolls[rolls_idx].r;
	}
	return 1;
}
