#include "dhj.h"
int main(void) {
	char *p = "123\0";
	char *q = "234\n11";

	puts(xstrdup(p));
	puts(xstrdup(q));
	puts(xstrdup(p));
	puts(xstrdup(q));
	return 0;
}
