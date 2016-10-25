
#line 1 "version_sorter.rl"
/*
 *  version_sorter.c
 *  version_sorter
 *
 * Created with Ragel.
 *   ragel -C -G2 version_sorter.rl
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ruby.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
typedef int compare_callback_t(const void *, const void *);

void Init_version_sorter(void);


#line 27 "version_sorter.c"
static const int parser_start = 1;
static const int parser_first_final = 1;
static const int parser_error = -1;

static const int parser_en_main = 1;


#line 26 "version_sorter.rl"


struct version_number {
	const char *original;
	VALUE rb_version;
	uint64_t num_flags;
	int32_t size;
	union version_comp {
		uint32_t number;
		struct strchunk {
			uint16_t offset;
			uint16_t len;
		} string;
	} comp[1];
};

static int
strchunk_cmp(const char *original_a, const struct strchunk *a,
		const char *original_b, const struct strchunk *b)
{
	size_t len = min(a->len, b->len);
	int cmp = memcmp(original_a + a->offset, original_b + b->offset, len);
	return cmp ? cmp : (int)(a->len - b->len);
}

static int
compare_version_number(const struct version_number *a,
		const struct version_number *b)
{
	int n, max_n = min(a->size, b->size);

	for (n = 0; n < max_n; ++n) {
		int num_a = (a->num_flags & (1ull << n)) != 0;
		int num_b = (b->num_flags & (1ull << n)) != 0;

		if (num_a == num_b) {
			const union version_comp *ca = &a->comp[n];
			const union version_comp *cb = &b->comp[n];
			int cmp = 0;

			if (num_a) {
				cmp = (int)ca->number - (int)cb->number;
			} else {
				cmp = strchunk_cmp(
						a->original, &ca->string,
						b->original, &cb->string);
			}

			if (cmp) return cmp;
		} else {
			return num_a ? 1 : -1;
		}
	}

	if (a->size < b->size)
		return (b->num_flags & (1ull << n)) ? -1 : 1;

	if (a->size > b->size)
		return (a->num_flags & (1ull << n)) ? 1 : -1;

	return 0;
}

static int
version_compare_cb(const void *a, const void *b)
{
	return compare_version_number(
		(*(const struct version_number * const *)a),
		(*(const struct version_number * const *)b));
}

static int
version_compare_cb_r(const void *a, const void *b)
{
	return -compare_version_number(
		(*(const struct version_number * const *)a),
		(*(const struct version_number * const *)b));
}

static struct version_number *
grow_version_number(struct version_number *version, uint new_size)
{
	return xrealloc(version,
			(sizeof(struct version_number) +
			 sizeof(union version_comp) * (new_size - 1)));
}

static struct version_number *
parse_version_number(char *string, long len)
{
	uint32_t num_flags = 0x0;
	uint32_t number = 0;
	uint comp_n = 0, comp_alloc = 4;
	int overflown = 0;
	char *p = string, *pe = string + len, *eof = pe;  // Ragel variables.
	int cs;
	char *start = NULL;

	struct version_number *version = grow_version_number(NULL, comp_alloc);

	
#line 137 "version_sorter.c"
	{
	cs = parser_start;
	}

#line 142 "version_sorter.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 45 )
		goto tr1;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr1;
	} else
		goto tr1;
	goto st0;
tr3:
#line 159 "version_sorter.rl"
	{
			version->comp[comp_n].string.offset = (uint16_t)(start - string);
			version->comp[comp_n].string.len = (uint16_t)(p - start);
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
	goto st0;
tr7:
#line 149 "version_sorter.rl"
	{
			if (overflown) {
				version->comp[comp_n].string.offset = (uint16_t)(start - string);
				version->comp[comp_n].string.len = (uint16_t)(p - start);
			} else {
				version->comp[comp_n].number = number;
				num_flags |= (1 << comp_n);
			}
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
#line 191 "version_sorter.c"
	if ( (*p) == 45 )
		goto tr1;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr1;
	} else
		goto tr1;
	goto st0;
tr1:
#line 127 "version_sorter.rl"
	{
			if (comp_n >= comp_alloc) {
				comp_alloc += 4;
				version = grow_version_number(version, comp_alloc);
			}
			start = p;
		}
	goto st2;
tr4:
#line 159 "version_sorter.rl"
	{
			version->comp[comp_n].string.offset = (uint16_t)(start - string);
			version->comp[comp_n].string.len = (uint16_t)(p - start);
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
#line 127 "version_sorter.rl"
	{
			if (comp_n >= comp_alloc) {
				comp_alloc += 4;
				version = grow_version_number(version, comp_alloc);
			}
			start = p;
		}
	goto st2;
tr8:
#line 149 "version_sorter.rl"
	{
			if (overflown) {
				version->comp[comp_n].string.offset = (uint16_t)(start - string);
				version->comp[comp_n].string.len = (uint16_t)(p - start);
			} else {
				version->comp[comp_n].number = number;
				num_flags |= (1 << comp_n);
			}
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
#line 127 "version_sorter.rl"
	{
			if (comp_n >= comp_alloc) {
				comp_alloc += 4;
				version = grow_version_number(version, comp_alloc);
			}
			start = p;
		}
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 260 "version_sorter.c"
	if ( (*p) == 45 )
		goto tr4;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st2;
	} else
		goto st2;
	goto tr3;
tr2:
#line 127 "version_sorter.rl"
	{
			if (comp_n >= comp_alloc) {
				comp_alloc += 4;
				version = grow_version_number(version, comp_alloc);
			}
			start = p;
		}
#line 135 "version_sorter.rl"
	{
			number = 0;
			overflown = 0;
		}
#line 140 "version_sorter.rl"
	{
			if (!overflown) {
				uint32_t old_number = number;
				number = (10 * number) + (uint32_t)(*p - '0');
				if (number < old_number)
					overflown = 1;
			}
		}
	goto st3;
tr5:
#line 159 "version_sorter.rl"
	{
			version->comp[comp_n].string.offset = (uint16_t)(start - string);
			version->comp[comp_n].string.len = (uint16_t)(p - start);
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
#line 127 "version_sorter.rl"
	{
			if (comp_n >= comp_alloc) {
				comp_alloc += 4;
				version = grow_version_number(version, comp_alloc);
			}
			start = p;
		}
#line 135 "version_sorter.rl"
	{
			number = 0;
			overflown = 0;
		}
#line 140 "version_sorter.rl"
	{
			if (!overflown) {
				uint32_t old_number = number;
				number = (10 * number) + (uint32_t)(*p - '0');
				if (number < old_number)
					overflown = 1;
			}
		}
	goto st3;
tr9:
#line 140 "version_sorter.rl"
	{
			if (!overflown) {
				uint32_t old_number = number;
				number = (10 * number) + (uint32_t)(*p - '0');
				if (number < old_number)
					overflown = 1;
			}
		}
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 344 "version_sorter.c"
	if ( (*p) == 45 )
		goto tr8;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr9;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr8;
	} else
		goto tr8;
	goto tr7;
	}
	_test_eof0: cs = 0; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 3: 
#line 149 "version_sorter.rl"
	{
			if (overflown) {
				version->comp[comp_n].string.offset = (uint16_t)(start - string);
				version->comp[comp_n].string.len = (uint16_t)(p - start);
			} else {
				version->comp[comp_n].number = number;
				num_flags |= (1 << comp_n);
			}
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
	break;
	case 2: 
#line 159 "version_sorter.rl"
	{
			version->comp[comp_n].string.offset = (uint16_t)(start - string);
			version->comp[comp_n].string.len = (uint16_t)(p - start);
		}
#line 164 "version_sorter.rl"
	{
			comp_n++;
		}
	break;
#line 392 "version_sorter.c"
	}
	}

	}

#line 175 "version_sorter.rl"


	version->original = string;
	version->num_flags = num_flags;
	version->size = (int32_t)comp_n;

	return version;
}

static VALUE
rb_version_sort_1(VALUE rb_self, VALUE rb_version_array, compare_callback_t cmp)
{
	(void)rb_self;  // Unused.

	struct version_number **versions;
	long length, i;
	VALUE *rb_version_ptr;

	Check_Type(rb_version_array, T_ARRAY);

	length = RARRAY_LEN(rb_version_array);
	if (!length)
		return rb_ary_new();

	versions = xcalloc(length, sizeof(struct version_number *));

	for (i = 0; i < length; ++i) {
		VALUE rb_version, rb_version_string;

		rb_version = rb_ary_entry(rb_version_array, i);
		if (rb_block_given_p())
			rb_version_string = rb_yield(rb_version);
		else
			rb_version_string = rb_version;

		versions[i] = parse_version_number(
				RSTRING_PTR(rb_version_string), RSTRING_LEN(rb_version_string));
		versions[i]->rb_version = rb_version;
	}

	qsort(versions, (size_t)length, sizeof(struct version_number *), cmp);
	rb_version_ptr = RARRAY_PTR(rb_version_array);

	for (i = 0; i < length; ++i) {
		rb_version_ptr[i] = versions[i]->rb_version;
		xfree(versions[i]);
	}
	xfree(versions);
	return rb_version_array;
}

static VALUE
rb_version_sort(VALUE rb_self, VALUE rb_versions)
{
	return rb_version_sort_1(rb_self, rb_ary_dup(rb_versions), version_compare_cb);
}

static VALUE
rb_version_sort_r(VALUE rb_self, VALUE rb_versions)
{
	return rb_version_sort_1(rb_self, rb_ary_dup(rb_versions), version_compare_cb_r);
}

static VALUE
rb_version_sort_bang(VALUE rb_self, VALUE rb_versions)
{
	return rb_version_sort_1(rb_self, rb_versions, version_compare_cb);
}

static VALUE
rb_version_sort_r_bang(VALUE rb_self, VALUE rb_versions)
{
	return rb_version_sort_1(rb_self, rb_versions, version_compare_cb_r);
}

void
Init_version_sorter(void)
{
	VALUE rb_mVersionSorter = rb_define_module("VersionSorter");
	rb_define_module_function(rb_mVersionSorter, "sort", rb_version_sort, 1);
	rb_define_module_function(rb_mVersionSorter, "rsort", rb_version_sort_r, 1);
	rb_define_module_function(rb_mVersionSorter, "sort!", rb_version_sort_bang, 1);
	rb_define_module_function(rb_mVersionSorter, "rsort!", rb_version_sort_r_bang, 1);
}
