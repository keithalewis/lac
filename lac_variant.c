// lac_variant.c - variant type
#include "lac_variant.h"

// pointer types
struct struct_align_pointer {
	char c;
	void* x;
};

#define FFI_POINTER { \
	sizeof(void*), \
	offsetof(struct struct_align_pointer, x), \
	FFI_TYPE_POINTER, NULL}

ffi_type ffi_type_string = FFI_POINTER;
ffi_type ffi_type_string_malloc = FFI_POINTER;
ffi_type ffi_type_string_cif = FFI_POINTER;
ffi_type ffi_type_string_cif_malloc = FFI_POINTER;

static ffi_type *ffi_variant_union_elements[] =
    { &ffi_type_pointer, NULL };

static ffi_type ffi_type_variant_union = {
	.size = 0,
	.alignment = 0,
	.type = FFI_TYPE_STRUCT,
	.elements = ffi_variant_union_elements
};

#define X(A,B,C,D) &ffi_type_ ## B ,
ffi_type *ffi_type_variant_union_elements[] = {
	FFI_TYPE_TABLE(X)
	NULL
};
#undef X

// assume union_type->elements initialized by ffi_type[2] = { ?, NULL}
static void prep_union_type(ffi_type * union_type, ffi_type ** union_elements)
{
	while (*union_elements) {
		ffi_cif cif;
		if (FFI_OK ==
		    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, *union_elements,
				 NULL)) {
			if ((*union_elements)->size > union_type->size) {
				union_type->size = (*union_elements)->size;
			}
			if ((*union_elements)->alignment >
			    union_type->alignment) {
				union_type->alignment =
				    (*union_elements)->alignment;
			}
		}
		++union_elements;
	}
}

void ffi_type_variant_prep(void)
{
	prep_union_type(&ffi_type_variant_union,
			ffi_type_variant_union_elements);
}

struct struct_align_variant {
	char c;
	lac_variant x;
};
static ffi_type *ffi_variant_elements[] = {
	&ffi_type_variant_union,
	&ffi_type_pointer,
	NULL
};

ffi_type ffi_type_variant = {
	.size = sizeof(lac_variant),
	.alignment = offsetof(struct struct_align_variant, x),
	.type = FFI_TYPE_STRUCT,
	.elements = ffi_variant_elements
};

