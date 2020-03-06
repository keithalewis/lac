// lac_dict.h - abstract dictionary interface

// key, value, open, close, get, put, remove, find, iterate
#define LAC_DICT(K, V, O, C, G, P, D, F, I) \
struct { \
	void* (*open)(const char*); \
	void (*close)(void*); \
	V* (*get)(K); \
	void (*put)(K,V); \
	void (*del)(K); \
	int (*each)(K*,V*); \
};
