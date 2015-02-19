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
				num_flags |= (1 << comp_n);
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

static VALUE
rb_version_sort_1(VALUE rb_self, VALUE rb_version_array, compare_callback_t cmp)
{
	struct version_number **versions;
	long length, i;
	VALUE *rb_version_ptr;

	Check_Type(rb_version_array, T_ARRAY);

	length = RARRAY_LEN(rb_version_array);
	if (!length)
		return rb_ary_new();

	versions = xcalloc(length, sizeof(struct version_number *));

	for (i = 0; i < length; ++i) {
		VALUE rb_version = rb_ary_entry(rb_version_array, i);
		versions[i] = parse_version_number(StringValuePtr(rb_version));
		versions[i]->rb_version = rb_version;
	}

	qsort(versions, length, sizeof(struct version_number *), cmp);
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

void Init_version_sorter(void)
{
	VALUE rb_mVersionSorter = rb_define_module("VersionSorter");
	rb_define_module_function(rb_mVersionSorter, "sort", rb_version_sort, 1);
	rb_define_module_function(rb_mVersionSorter, "rsort", rb_version_sort_r, 1);
	rb_define_module_function(rb_mVersionSorter, "sort!", rb_version_sort_bang, 1);
	rb_define_module_function(rb_mVersionSorter, "rsort!", rb_version_sort_r_bang, 1);
}
