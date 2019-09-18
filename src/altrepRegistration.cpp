#include "altrep.h"
#include "altrepNumericCommonFunc.h"
#include "tools.h"


const void* getPointer(SEXP x) {
	const void* ptr;
	switch (TYPEOF(x))
	{
	case INTSXP:
	case REALSXP:
	case LGLSXP:
	case RAWSXP:
		ptr = DATAPTR_OR_NULL(x);
		if (ptr == NULL) {
			return DATAPTR(x);
		}
		else {
			return ptr;
		}
	case STRSXP:
		return x;
	default:
		errorHandle("Unexpected SEXP of type %d\n", TYPEOF(x));
		// Just for suppressing the annoying warning, it should never be excuted
		return nullptr;
	}
}

R_altrep_class_t getAltClass(int type) {
	switch (type) {
	case REAL_TYPE:
		return shared_real_class;
	case INT_TYPE:
		return shared_integer_class;
	case LOGICAL_TYPE:
		return shared_logical_class;
	case RAW_TYPE:
		return shared_raw_class;
	case STR_TYPE:
		//return shared_str_class;
	default: errorHandle("Type of %d is not supported yet", type);
	}
	// Just for suppressing the annoying warning, it should never be excuted
	return shared_real_class;
}


/*
Register ALTREP class
*/

#define ALT_NUM_COMMOM_REG(ALT_CLASS,ALT_TYPE,C_TYPE,R_TYPE)\
	ALT_CLASS =R_make_##ALT_TYPE##_class(class_name, PACKAGE_NAME, dll);\
	/* override ALTREP methods */\
	R_set_altrep_Inspect_method(ALT_CLASS, sharedVector_Inspect);\
	R_set_altrep_Length_method(ALT_CLASS, sharedVector_length);\
	R_set_altrep_Duplicate_method(ALT_CLASS, sharedVector_duplicate);\
	/*R_set_altrep_Coerce_method(ALT_CLASS, real_coerce);*/\
	R_set_altrep_Unserialize_method(ALT_CLASS, sharedVector_unserialize);\
	R_set_altrep_Serialized_state_method(ALT_CLASS, sharedVector_serialized_state);\
	/* override ALTVEC methods */\
	R_set_altvec_Dataptr_method(ALT_CLASS, sharedVector_dataptr);\
	R_set_altvec_Dataptr_or_null_method(ALT_CLASS, sharedVector_dataptr_or_null);\
	R_set_altvec_Extract_subset_method(ALT_CLASS, numeric_subset<R_TYPE, C_TYPE>);\
	/* override ALTREAL methods */\
	R_set_##ALT_TYPE##_Elt_method(ALT_CLASS, numeric_Elt<C_TYPE>);\
	R_set_##ALT_TYPE##_Get_region_method(ALT_CLASS, numeric_region<C_TYPE>);



R_altrep_class_t shared_real_class;
#define C_TYPE double
#define R_TYPE REALSXP
//[[Rcpp::init]]
void init_real_class(DllInfo* dll)
{
	char class_name[] = "shared_real";
	ALT_NUM_COMMOM_REG(shared_real_class, altreal, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


R_altrep_class_t shared_integer_class;
#define C_TYPE int
#define R_TYPE INTSXP
//[[Rcpp::init]]
void init_integer_class(DllInfo* dll) {
	char class_name[] = "shared_int";
	ALT_NUM_COMMOM_REG(shared_integer_class, altinteger, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE



R_altrep_class_t shared_logical_class;
#define C_TYPE int
#define R_TYPE LGLSXP
//[[Rcpp::init]]
void init_logical_class(DllInfo* dll) {
	char class_name[] = "shared_logical";
	ALT_NUM_COMMOM_REG(shared_logical_class, altlogical, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


R_altrep_class_t shared_raw_class;
#define C_TYPE Rbyte
#define R_TYPE RAWSXP
//[[Rcpp::init]]
void init_raw_class(DllInfo* dll) {
	char class_name[] = "shared_raw";
	ALT_NUM_COMMOM_REG(shared_raw_class, altraw, C_TYPE, R_TYPE)
}
#undef C_TYPE
#undef R_TYPE


// The string ALTREP is not matured so that I decide
// to wait and not to implement it

/*
SEXP altstring_elt(SEXP x, R_xlen_t i);
void* altstring_dataptr(SEXP x, Rboolean writable);
const void* altstring_dataptr_or_null(SEXP x);
R_altrep_class_t shared_str_class;
//[[Rcpp::init]]
void init_str_class(DllInfo* dll) {
	shared_str_class = R_make_altstring_class("shared_str", PACKAGE_NAME, dll);
	// override ALTREP methods
	R_set_altrep_Inspect_method(shared_str_class, sharedVector_Inspect);
	R_set_altrep_Length_method(shared_str_class, sharedVector_length);
	R_set_altrep_Duplicate_method(shared_str_class, sharedVector_duplicate);
	//R_set_altrep_Coerce_method(ALT_CLASS, real_coerce);
	R_set_altrep_Unserialize_method(shared_str_class, sharedVector_unserialize);
	R_set_altrep_Serialized_state_method(shared_str_class, sharedVector_serialized_state);

	// override ALTVEC methods
	R_set_altvec_Dataptr_method(shared_str_class, altstring_dataptr);
	R_set_altvec_Dataptr_or_null_method(shared_str_class, altstring_dataptr_or_null);
	//R_set_altvec_Extract_subset_method(shared_str_class, numeric_subset<R_TYPE, C_TYPE>);
	R_set_altstring_Elt_method(shared_str_class, altstring_elt);
}


SEXP altstring_elt(SEXP x, R_xlen_t i) {
	DEBUG(messageHandle("string subset:%llu\n", i));
	char* source = (char*)SV_PTR(x);
	ULLong* offset = (ULLong*)source;
	ULLong curOffset = *(offset + i);
	char* data = source + curOffset;
	return(Rf_mkChar(data));
}

void* altstring_dataptr(SEXP x, Rboolean writable) {
	DEBUG(messageHandle("accessing string pointer\n"));

	ULLong n = SV_LENGTH(x);
	SEXP* res = (SEXP*)malloc(n * sizeof(SEXP*));
	SEXP char_res = Rf_mkChar("NA");
	for (ULLong i = 0; i < n; i++) {
		res[i] = char_res;
	}
	return(res);
}

const void* altstring_dataptr_or_null(SEXP x) {
	DEBUG(messageHandle("accessing string pointer or null\n"));
	return(NULL);
}

*/

