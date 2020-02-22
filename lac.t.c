// lac.t.cpp
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "lac.h"

jmp_buf lac_jmp_buf;
const char* lac_strerror;

int test_lac_ffi(void);
int test_lac_parse(void);
int test_lac_map(void);
int test_lac(void);
int test_lac_variant(void);

// call f n times an return millisecnds
int timer(void(*f)(void), int n)
{
	clock_t b = clock();
	for (int i = 0; i < n; ++i) {
		f();
	}
	clock_t e = clock();
	
	return ((e - b)*1000)/CLOCKS_PER_SEC;
}

char buf[10000] = "abcd\n"; 

void test_tmpfile()
{
	FILE* fp = tmpfile();

	fwrite(buf, 1, 100, fp);
	fread(buf, 1, 100, fp);

	fclose(fp);
}
void test_memfile()
{
	int fd = open("/sys/fs/cgroup/foo", O_RDWR/*|O_TMPFILE*/);

	write(fd, buf, 100);
	read(fd, buf, 100);

	close(fd);
}
void test_memfile2()
{
	int fd = open("foo", O_RDWR|O_CREAT/*|O_TMPFILE*/, S_IRWXU);

	perror(strerror(errno));
	ensure(fd >= 0);

	write(fd, buf, 100);
	fsync(fd);
	close(fd);
	//read(fd, buf, 100);

	int fd2 = open("foo", O_RDONLY);
	perror(strerror(errno));
	ensure(fd2 >= 0);
	char buf2[10];
	read(fd2, buf2, 10);
	write(1, buf2, 5);

	close(fd2);
}
void test_memmove()
{
	memmove(buf, buf, 100);
}

int test_lac()
{
	return 0;
}

int main()
{
	if (setjmp(lac_jmp_buf)) {
		fputs(lac_strerror, stderr);
		
		return -1;
	}

	test_memfile2();
	//printf("file: %i ms\n", timer(test_tmpfile, 1000));
	//printf("open: %i ms\n", timer(test_memfile, 1));
	//printf("open2: %i ms\n", timer(test_memfile2, 1));

	//test_lac_ffi();
	test_lac_parse();
	test_lac_map();
    test_lac();
	test_lac_variant();

	return 0;
}
