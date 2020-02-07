// lac_parse.c - parsing functions
#include <assert.h>
#include "lac_parse.h"

bool token_view_empty(const token_view t)
{
	return t.b == t.e;
}

// return information if error
const char* token_view_error(const token_view t)
{
	return t.e == 0 ? t.b : 0;
}

// skip space or nonspace
const char* skip_space(const char* b, const char* e, bool space)
{
	if (b >= e) {
		//throw std::runtime_error("skip_space: invalid character range");
		return 0;
	}

	while (b < e && (space ? isspace(*b) : !isspace(*b))) {
		++b;
	}

	assert (b <= e);

	return b;
}
// match delimiter at same nesting level
// return pointer past matching right delimiter
const char* next_match(const char* b, const char* e, 
	char l /*= '{'*/, char r /*= '}'*/)
{
	if (b >= e) {
		//throw std::runtime_error("nest_match: invalid character range");
		return 0;
	}

	if (*b != l) {
		//throw std::runtime_error("next_match: token does not start with left delimiter");
		return 0;
	}

	size_t level = 1;
	while (++b < e && level != 0) {
		if (*b == '\\') {
			++b;
			if (b == e) {
				//throw std::runtime_error("next_match: unmatched right delimiter");
				return 0;
			}
		}
		else {
			if (*b == r)
				--level;
			else if (*b == l)
				++level;
		}
	}

	if (level != 0) {
		//throw std::runtime_error("next_match: delimiter level mismatch");
		return 0;
	}

	return b;
}
const char* next_quote(const char* b, const char* e,
	const char q /*= '"'*/)
{
	return next_match(b, e, q, q);
}
// copy stream into static buffer and return view
token_view token_view_next(const char* b, const char* e)
{
	const char* e_;

	b = skip_space(b, e, true);
	if (!b || b == e) {
		return (token_view){e,e}; // empty
	}

	if (*b == '"') {
		e_ = next_quote(b, e, '"');
	}
	else if (*b == '{') {
		e_ = next_match(b, e, *b, '}');
	}
	else {
		e_ = skip_space(b, e, false);
	}

	if (e_ == 0) {
		return (token_view){b,0}; // error
	}

	// tokens must be space separated
	if (e_ != e && !isspace(*e_)) {
		return (token_view){b,0};
	}

	return (token_view){b, e_};
}
