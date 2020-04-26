#ifndef _DHJ_H
#define _DHJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROUTE_ADD 1
#define ROUTE_DEL 2

char *xstrdup(char *str) {
	char *p;
	int len = 0;
	if(p = strchr(str, '\n')) {
		len = p - str;
	} else {
		len = strlen(str);
	}

	if(p) p = NULL;

	if(!(p = malloc(len + 1))) {
		return NULL;
	}

	memcpy(p, str, len);
	return p;
}

/*
 *  IPv4 add/del route item in route table
    refer to https://www.cnblogs.com/wangshide/archive/2012/10/25/2740410.html
 */
bool io_route(int action, char *ip, char *mask, char *iface) {
#if defined (HAVE_LINUX)
	struct rtentry route;  /* route item struct */
	char target[128] = {0};
	char gateway[128] = {0};
	char netmask[128] = {0};

	struct sockaddr_in *addr;

	int skfd;

	/* clear route struct by 0 */
	memset((char *)&route, 0x00, sizeof(route));

	/* default target is net (host)*/
	route.rt_flags = RTF_UP ;

	if(ip) {   // default is a network target
		strcpy(target, ip);
		fprintf(stderr, "target: %s\n", target);
		addr = (struct sockaddr_in*) &route.rt_dst;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = inet_addr(target);

	}
 
	if(mask) {   // netmask setting
		strcpy(netmask, mask);
		addr = (struct sockaddr_in*) &route.rt_genmask;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = inet_addr(netmask);

	}
/*
	if(gw) {  //
		strcpy(gateway, gw);
		addr = (struct sockaddr_in*) &route.rt_gateway;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = inet_addr(gateway);
		route.rt_flags |= RTF_GATEWAY;
	}
*/

	if(iface) {  /* device setting */
		route.rt_dev = iface;
	}

/*
	if(mtu) {  //mtu setting
		route.rt_flags |= RTF_MTU;
		route.rt_mtu = atoi(*args);
	}
*/

	/* create a socket */
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0) {
		perror("socket");
		return false;
	}

	/* tell the kernel to accept this route */
	if(action == ROUTE_DEL) {  /* del a route item */
		if(ioctl(skfd, SIOCDELRT, &route) < 0) {
			perror("SIOCDELRT");
			close(skfd);
			return false;
		}

	} else if(action == ROUTE_ADD) {  /* add a route item */
		if(ioctl(skfd, SIOCADDRT, &route) < 0) {
			perror("SIOCADDRT");
			close(skfd);
			return false;
		}

	} else {
		log0("Error route action(%d)", action);	
		return false;

	}

	close(skfd);
#else
	fprintf(stderr, "io_route() has no effect on your ostype !!!\n");
	
#endif
	return true;
}

/* get_hex:  huangjue.deng  2020.3.18
 *	while let data to get struct, 
 * 	s_t *s = malloc(sizeof(*s)); 
 * 	printf_hex(s, sizeof(s_t)); 
 *	
 */
char *get_hex(void *data, size_t len) {
	char buf[len + 1];
	char *ret = malloc(len * 2 + 1);
	memcpy(buf, (char *) data, len);
	memset(ret, 0, sizeof(ret));
	for(int i = 0; i < len; i++) {
		buf[i] = 0x00? sprintf(&ret[i * 2], "00") : (buf[i] < 0x10? sprintf(&ret[i * 2], "0%x", buf[i]) : sprintf(&ret[i * 2], "%x", buf[i]));

	}

	return ret;
}


/* get_format_hex:  huangjue.deng  2020.3.18
 *	while let data to get struct, 
 * 	s_t *s = malloc(sizeof(*s)); 
 * 	printf_hex(s, sizeof(s_t)); 
 *	
 */
char *get_format_hex(void *data, size_t len) {
	char buf[len + 1];
	char *ret = malloc(len * 2 + len / 4 + 1);
	memcpy(buf, (char *) data, len);
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


/* printf_hex:  huangjue.deng  2020.3.18
 *	while let data to get struct, 
 *	s_t *s = malloc(sizeof(*s)); 
 * 	printf_hex(s, sizeof(s_t)); 
 *	
 */
bool printf_hex(void *data, size_t len) {
	char buf[len + 1];
	memcpy(buf, (char *)data, len);
	for(int i = 0; i < len; i++) {
		buf[i] = 0x00? fprintf(stdout, "00") : (buf[i] < 0x10? fprintf(stdout, "0%x", buf[i]) : fprintf(stdout, "%x", buf[i]));

		(i + 1) % 16 ? ((i + 1) % 4 ? : fprintf(stdout, " ")) : fprintf(stdout, "\n");
	}

	fprintf(stdout, "\n");
	return true;
}



#endif
