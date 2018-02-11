/*
 *  version_sorter.c
 *  version_sorter
 *
 *  Created by K. Adam Christensen on 10/10/09.
 *  Copyright 2009. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <ruby.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
typedef int compare_callback_t(const void *, const void *);

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
				int64_t cmp64 = (int64_t)ca->number - (int64_t)cb->number;
				cmp = (int)max(-1, min(1, cmp64));
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
		(*(const struct version_number **)a),
		(*(const struct version_number **)b));
}

static int
version_compare_cb_r(const void *a, const void *b)
{
	return -compare_version_number(
		(*(const struct version_number **)a),
		(*(const struct version_number **)b));
}

static struct version_number *
grow_version_number(struct version_number *version, int new_size)
{
	return xrealloc(version,
			(sizeof(struct version_number) +
			 sizeof(union version_comp) * new_size));
}

static struct version_number *
parse_version_number(const char *string)
{
	struct version_number *version = NULL;
	uint64_t num_flags = 0x0;
	uint16_t offset;
	int comp_n = 0, comp_alloc = 4;

	version = grow_version_number(version, comp_alloc);

	for (offset = 0; string[offset] && comp_n < 64;) {
		if (comp_n >= comp_alloc) {
			comp_alloc += 4;
			version = grow_version_number(version, comp_alloc);
		}

		if (isdigit(string[offset])) {
			uint32_t number = 0;
			uint16_t start = offset;
			int overflown = 0;

			while (isdigit(string[offset])) {
				if (!overflown) {
					uint32_t old_number = number;
					number = (10 * number) + (string[offset] - '0');
					if (number < old_number) overflown = 1;
				}

				offset++;
			}

			if (overflown) {
				version->comp[comp_n].string.offset = start;
				version->comp[comp_n].string.len = offset - start;
			} else {
				version->comp[comp_n].number = number;
				num_flags |= (1ull << comp_n);
			}
			comp_n++;
			continue;
		}

		if (string[offset] == '-' || isalpha(string[offset])) {
			uint16_t start = offset;

			if (string[offset] == '-')
				offset++;

			while (isalpha(string[offset]))
				offset++;

			version->comp[comp_n].string.offset = start;
			version->comp[comp_n].string.len = offset - start;
			comp_n++;
			continue;
		}

		offset++;
	}

	version->original = string;
	version->num_flags = num_flags;
	version->size = comp_n;

	return version;
}

struct sort_context {
	VALUE rb_self;
	VALUE rb_version_array;
	compare_callback_t *cmp;
	struct version_number **versions;
};

static VALUE
rb_version_sort_1_cb(VALUE arg)
{
	struct sort_context *context = (struct sort_context *)arg;
	long length, i;
	VALUE *rb_version_ptr;

	length = RARRAY_LEN(context->rb_version_array);
	for (i = 0; i < length; ++i) {
		VALUE rb_version, rb_version_string;

		rb_version = rb_ary_entry(context->rb_version_array, i);
		if (rb_block_given_p())
			rb_version_string = rb_yield(rb_version);
		else
			rb_version_string = rb_version;

		context->versions[i] = parse_version_number(StringValueCStr(rb_version_string));
		context->versions[i]->rb_version = rb_version;
	}

	qsort(context->versions, length, sizeof(struct version_number *), context->cmp);
	rb_version_ptr = RARRAY_PTR(context->rb_version_array);

	for (i = 0; i < length; ++i) {
		rb_version_ptr[i] = context->versions[i]->rb_version;
	}

	return context->rb_version_array;
}

static VALUE
rb_version_sort_1(VALUE rb_self, VALUE rb_version_array, compare_callback_t cmp)
{
	long length, i;
	int exception;

	Check_Type(rb_version_array, T_ARRAY);

	length = RARRAY_LEN(rb_version_array);
	if (!length)
		return rb_ary_new();

	struct sort_context context = {
		rb_self,
		rb_version_array,
		cmp,
		xcalloc(length, sizeof(struct version_number *)),
	};

	VALUE result = rb_protect(rb_version_sort_1_cb, (VALUE)&context, &exception);

	for (i = 0; i < length; ++i) {
		xfree(context.versions[i]);
	}
	xfree(context.versions);

	if (exception) {
		rb_jump_tag(exception);
	}

	return result;
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

struct compare_context {
	VALUE rb_version_a, rb_version_b;
	struct version_number *version_a, *version_b;
};

static VALUE
rb_version_compare_cb(VALUE arg)
{
	struct compare_context *context = (struct compare_context *)arg;

	context->version_a = parse_version_number(StringValueCStr(context->rb_version_a));
	context->version_b = parse_version_number(StringValueCStr(context->rb_version_b));

	return INT2NUM(version_compare_cb(&context->version_a, &context->version_b));
}

static VALUE
rb_version_compare(VALUE rb_self, VALUE rb_version_a, VALUE rb_version_b)
{
	int exception;
	struct compare_context context = {
		rb_version_a, rb_version_b,
		NULL, NULL,
	};

	VALUE result = rb_protect(rb_version_compare_cb, (VALUE)&context, &exception);

	xfree(context.version_a);
	xfree(context.version_b);

	if (exception) {
		rb_jump_tag(exception);
	}

	return result;
}

void Init_version_sorter(void)
{
	VALUE rb_mVersionSorter = rb_define_module("VersionSorter");
	rb_define_module_function(rb_mVersionSorter, "sort", rb_version_sort, 1);
	rb_define_module_function(rb_mVersionSorter, "rsort", rb_version_sort_r, 1);
	rb_define_module_function(rb_mVersionSorter, "sort!", rb_version_sort_bang, 1);
	rb_define_module_function(rb_mVersionSorter, "rsort!", rb_version_sort_r_bang, 1);
	rb_define_module_function(rb_mVersionSorter, "compare", rb_version_compare, 2);
}
