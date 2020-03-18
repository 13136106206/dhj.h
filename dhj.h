#ifndef _DHJ_H
#define _DHJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *get_hex(void *data, size_t len) {
	char buf[len + 1];
	char *ret = malloc(len * 2 + 1);
	memcpy(buf, data, len);
	memset(ret, 0, sizeof(ret));
	for(int i = 0; i < len; i++) {
		buf[i] = 0x00? sprintf(&ret[i * 2], "00") : (buf[i] < 0x10? sprintf(&ret[i * 2], "0%x", buf[i]) : sprintf(&ret[i * 2], "%x", buf[i]));

	}

	return ret;
}

char *get_format_hex(void *data, size_t len) {
	char buf[len + 1];
	char *ret = malloc(len * 2 + len / 4 + 1);
	memcpy(buf, data, len);
	memset(ret, 0, sizeof(ret));
	int count = 0;
	int j = 0;
	for(int i = 0; i < len; i++) {
		j = i * 2 + count;
		buf[i] = 0x00? sprintf(&ret[j], "00") : (buf[i] < 0x10? sprintf(&ret[j], "0%x", buf[i]) : sprintf(&ret[j], "%x", buf[i]));


		(i + 1) % 16 ? ((i + 1) % 4 ? : (sprintf(&ret[j + 2], " ") && count++)) : (sprintf(&ret[j + 2], "\n") && count++);
	}

	return ret;
}

bool printf_hex(void *data, size_t len) {
	char buf[len + 1];
	memcpy(buf, data, len);
	for(int i = 0; i < len; i++) {
		buf[i] = 0x00? fprintf(stdout, "00") : (buf[i] < 0x10? fprintf(stdout, "0%x", buf[i]) : fprintf(stdout, "%x", buf[i]));

		(i + 1) % 16 ? ((i + 1) % 4 ? : fprintf(stdout, " ")) : fprintf(stdout, "\n");
	}

	fprintf(stdout, "\n");
	return true;
}



#endif
