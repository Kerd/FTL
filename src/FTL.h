/* Pi-hole: A black hole for Internet advertisements
*  (c) 2017 Pi-hole, LLC (https://pi-hole.net)
*  Network-wide ad blocking via your own hardware.
*
*  FTL Engine
*  Global definitions
*
*  This file is copyright under the latest version of the EUPL.
*  Please see LICENSE file for your rights under this license. */
#ifndef FTL_H
#define FTL_H

#define __USE_XOPEN
#define _GNU_SOURCE
#include <stdio.h>
// variable argument lists
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
// struct sockaddr_in
#include <netinet/in.h>
// char* inet_ntoa(struct in_addr in)
#include <arpa/inet.h>
// getnameinfo();
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#ifdef __FreeBSD__
// setproctitle()
#include <sys/types.h>
#include <unistd.h>
#endif
#ifdef __linux__
// prctl()
#include <sys/prctl.h>
#endif
//#include <math.h>
#include <pwd.h>
// syslog
#include <syslog.h>
// tolower()
#include <ctype.h>
// Unix socket
#include <sys/un.h>
// Interfaces
#include <ifaddrs.h>
#include <net/if.h>

// Define MIN and MAX macros, use them only when x and y are of the same type
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
// MIN(x,y) is already defined in dnsmasq.h

#define SOCKETBUFFERLEN 1024

// How often do we garbage collect (to ensure we only have data fitting to the MAXLOGAGE defined above)? [seconds]
// Default: 3600 (once per hour)
#define GCinterval 3600

// Delay applied to the garbage collecting [seconds]
// Default: -60 (one minute before a full hour)
#define GCdelay (-60)

// How many client connection do we accept at once?
#define MAXCONNS 255

// Over how many queries do we iterate at most when trying to find a match?
#define MAXITER 1000

// How many hours do we want to store in FTL's memory? [hours]
#define MAXLOGAGE 24

// Interval for overTime data [seconds]
// Default: 600 (10 minute intervals)
#define OVERTIME_INTERVAL 600

// How many overTime slots do we need?
// (24+1) hours * number of intervals per hour
// We need to be able to hold 25 hours as we need some reserve
// due to that GC is only running once an hours so the shown data
// can be 24 hours + 59 minutes
#define OVERTIME_SLOTS ((MAXLOGAGE+1)*3600/OVERTIME_INTERVAL)

// Interval for resolving NEW client and upstream server host names [seconds]
// Default: 60 (once every minute)
#define RESOLVE_INTERVAL 60

// Interval for re-resolving ALL known host names [seconds]
// Default: 3600 (once every hour)
#define RERESOLVE_INTERVAL 3600

// Privacy mode constants
#define HIDDEN_DOMAIN "hidden"
#define HIDDEN_CLIENT "0.0.0.0"

// Used to check memory integrity in various structs
#define MAGICBYTE 0x57

// Some magic database constants
#define DB_FAILED -2
#define DB_NODATA -1

// Add a timeout for the pihole-FTL.db database connection [milliseconds]
// This prevents immediate failures when the database is busy for a short time.
// Default: 1000 (one second)
#define DATABASE_BUSY_TIMEOUT 1000

// Use out own memory handling functions that will detect possible errors
// and report accordingly in the log. This will make debugging FTL crashs
// caused by insufficient memory or by code bugs (not properly dealing
// with NULL pointers) much easier.
#define free(param) FTLfree(param, __FILE__,  __FUNCTION__,  __LINE__)
#define lib_strdup() strdup()
#undef strdup
#define strdup(param) FTLstrdup(param, __FILE__,  __FUNCTION__,  __LINE__)
#define calloc(p1,p2) FTLcalloc(p1,p2, __FILE__,  __FUNCTION__,  __LINE__)
#define realloc(p1,p2) FTLrealloc(p1,p2, __FILE__,  __FUNCTION__,  __LINE__)

extern pthread_t telnet_listenthreadv4;
extern pthread_t telnet_listenthreadv6;
extern pthread_t socket_listenthread;
extern pthread_t DBthread;
extern pthread_t GCthread;
extern pthread_t DNSclientthread;

#if defined(__FreeBSD__)
#include <sys/thr.h>
static inline void
set_thread_name(const char *name,
    int x __unused, int y __unused, int z __unused)
{
	long tid;
	if (thr_self(&tid) == 0)
		(void)thr_set_name(tid, name);
}
#elif defined(__linux__)
#define set_thread_name(name, x, y, z) prctl(PR_SET_NAME, name, x, y, z)
#endif

#endif // FTL_H
