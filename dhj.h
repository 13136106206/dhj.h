#ifndef _DHJ_H
#define _DHJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <net/if.h>       /* ifreq struct */
#include <netdb.h>


#define ROUTE_ADD 1
#define ROUTE_DEL 2

#define FFL __FILE__, __FUNCTION__, __LINE__
#define logl() fprintf(stderr, "(%s)%s[%d]...\n", FFL)
#define logs(x) fprintf(stderr, "(%s)%s[%d]: %s\n", FFL, x)
#define logi(x) fprintf(stderr, "(%s)%s[%d]: %ld\n", FFL, x)

static double timeval_difference(struct timeval *t1, struct timeval *t2) {
	fprintf(stderr, "%ld %ld\n", t2->tv_sec, t2->tv_usec);
	fprintf(stderr, "%ld %ld\n", t1->tv_sec, t1->tv_usec);
	return ((double) (((t2->tv_sec - t1->tv_sec) * 1.0) + ((t2->tv_usec - t1->tv_usec) / 1000000.0)));
}

static inline void *xnstrdup(void *data, size_t n) {
	char *p = calloc(n, sizeof(char *));

	if(!p) {
		return NULL;
	}

	memcpy(p, data, n);

	return p;
}

static inline void *xrealloc(void *p, size_t n) {
	p = realloc(p, n);

	if(!p) {
		abort();
	}

	return p;
}

#ifdef _WIN32
	#define get_pointer_useable_size(x) _msize(x)
#else
	#define get_pointer_useable_size(x) malloc_usable_size(x)
#endif

static inline void *xnstrcat(void *p, void *data, size_t n) {
	size_t len = strlen((char *)p);
	size_t need = len - get_pointer_useable_size(p) + n + 1;
	if(len + n + 1 > get_pointer_useable_size(p)) {
		p = realloc(p, len + n + 1);
	}

	if(!p) {
		abort();
	}

	memcpy(p + len, data, n);

	return p;
}

static inline void *xstrdup(void *data) {
	char *str = (char *)data;
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

#ifdef _WIN32
int windows_control_route(int action, const char *ip, const char *mask, const char *gateway) {
	NET_LUID luid;
	NET_IFINDEX idx;

	wchar_t *walias;
	int len = MultiByteToWideChar( CP_ACP ,0,iface ,strlen( iface), NULL, 0);
	walias = (wchar_t *)malloc(len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, iface, strlen(iface), walias, len);
	walias[len]= '\0' ;

	ConvertInterfaceAliasToLuid(walias, &luid);
	ConvertInterfaceLuidToIndex(&luid, &idx);

	MIB_IPFORWARDROW  row = { 0 };
	row.dwForwardDest = inet_addr(ip); //目标网络
	row.dwForwardMask = inet_addr(mask); //掩码
	row.dwForwardProto = MIB_IPPROTO_NETMGMT;
	row.dwForwardType = MIB_IPROUTE_TYPE_DIRECT;
	row.dwForwardMetric1 = 311; //route print 里的Metric
	row.dwForwardIfIndex = idx; //网卡索引,用arp -a,route print可以查看本机网卡的索引
	row.dwForwardNextHop = inet_addr(gateway); //网关

	DWORD dwRet;
	if(action == ROUTE_ADD) {
		dwRet = CreateIpForwardEntry(&row);
	} else {
		dwRet = DeleteIpForwardEntry(&row);
	}

	free(walias);
	return dwRet;
}
#endif

#ifdef __linux__
/*
 *  IPv4 add/del route item in route table
    refer to https://www.cnblogs.com/wangshide/archive/2012/10/25/2740410.html
 */
bool io_control_route(int action, char *ip, char *mask, char *iface, char *gw) {
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

	if(gw) {
		strcpy(gateway, gw);
		addr = (struct sockaddr_in*) &route.rt_gateway;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = inet_addr(gateway);
		route.rt_flags |= RTF_GATEWAY;
	}

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
		return false;
	}

	close(skfd);
	return true;
}

/*
* following functions are ipv6 address transfrom
* (from string to address struct and so on.)
* these functions from net-tools inet6.c file.
*/

int INET6_resolve(char *name, struct sockaddr_in6 *sin6) {

	struct addrinfo req, *ai;
	int s;

	memset (&req, '\0', sizeof req);
	req.ai_family = AF_INET6;
	if((s = getaddrinfo(name, NULL, &req, &ai)))  {
		return -1;
	}

	memcpy(sin6, ai->ai_addr, sizeof(struct sockaddr_in6));

	freeaddrinfo(ai);

	return 0;
}

int INET6_getsock(char *bufp, struct sockaddr *sap) {
	struct sockaddr_in6 *sin6;

	sin6 = (struct sockaddr_in6 *) sap;
	sin6->sin6_family = AF_INET6;
	sin6->sin6_port = 0;

	if(inet_pton(AF_INET6, bufp, sin6->sin6_addr.s6_addr) <= 0) {
		return -1;
	}

	return 16;
}

int INET6_input(int type, char *bufp, struct sockaddr *sap) {
	switch (type)  {
		case 1:
			return (INET6_getsock(bufp, sap));
		default:
			return (INET6_resolve(bufp, (struct sockaddr_in6 *) sap));
	}
}


/* IPv6 add/del route item in route table */
/* main part of this function is from net-tools inet6_sr.c file */
bool io_control_route6(int action, char *ipv6, char *mask, char *iface, char *gw) {
	struct in6_rtmsg rt;          /* ipv6 route struct */
	struct ifreq ifr;             /* interface request struct */
	struct sockaddr_in6 sa6;      /* ipv6 socket address */
	char target[128];
	//char gateway[128] = "NONE";
	int prefix_len;               /* network prefix length */
	char *devname = NULL;         /* device name */
	char *cp;
	int mtu = 0;

	int skfd = -1;

	strcpy(target, ipv6);

	if(!strcmp(target, "default")) {
		prefix_len = 0;
		memset(&sa6, 0, sizeof(sa6));
	} else {
		if ((cp = strchr(target, '/'))) {
			prefix_len = atol(cp + 1);
			sprintf(cp, "1");
			*cp = '\0';
			if(prefix_len < 0 || prefix_len > 128) {
				return false;
			}

		} else {
			prefix_len = 128;
		}

		if(INET6_input(1, target, (struct sockaddr *) &sa6) < 0 && INET6_input(0, target, (struct sockaddr *) &sa6) < 0) {
			return false;
		}
	}

	/* Clean out the RTREQ structure. */
	memset((char *) &rt, 0, sizeof(struct in6_rtmsg));

	memcpy(&rt.rtmsg_dst, sa6.sin6_addr.s6_addr, sizeof(struct in6_addr));

	/* Fill in the other fields. */
	rt.rtmsg_flags = RTF_UP;
	if (prefix_len == 128) {
		rt.rtmsg_flags |= RTF_HOST;
	}

	rt.rtmsg_metric = 1;
	rt.rtmsg_dst_len = prefix_len;

/*
	if(gw) {
		if(rt.rtmsg_flags & RTF_GATEWAY) {
			return fasle;
		}

		strcpy(gateway, gw);

		if(INET6_input(1, gateway, (struct sockaddr *) &sa6) < 0) {
			return false;
		}

		memcpy(&rt.rtmsg_gateway, sa6.sin6_addr.s6_addr, sizeof(struct in6_addr));
		rt.rtmsg_flags |= RTF_GATEWAY;
	}

	if(mod) {
		rt.rtmsg_flags |= RTF_MODIFIED;
	}

	if(MTU)  {
		mtu = MTU;
	}
*/

	if(iface) {
		devname = iface;
	}

	/* Create a socket to the INET6 kernel. */
	if((skfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return false;
	}

	memset(&ifr, 0, sizeof(ifr));

	if(action == ROUTE_ADD) {
		if(devname) {/* device setting */
			strcpy(ifr.ifr_name, devname);

			if(ioctl(skfd, SIOGIFINDEX, &ifr) < 0) {
				perror("SIOGIFINDEX");
				return false;
			}

			rt.rtmsg_ifindex = ifr.ifr_ifindex;
		}

		if(mtu) {/* mtu setting */
			ifr.ifr_mtu = mtu;

			if (ioctl(skfd, SIOCSIFMTU, &ifr) < 0) {
				perror("SIOCGIFMTU");
				return false;
			}
		}

		if(ioctl(skfd, SIOCADDRT, &rt) < 0) {
			perror("SIOCADDRT");
			close(skfd);
			return false;
		}

	}

	/* Tell the kernel to accept this route. */
	if(action == ROUTE_DEL) {
		if(ioctl(skfd, SIOCDELRT, &rt) < 0) {
			perror("SIOCDELRT");
			close(skfd);
			return false;
		}

	}

	/* Close the socket. */
	close(skfd);
	return true;
}
#endif


#endif
