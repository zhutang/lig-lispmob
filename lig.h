/*
 *	lig.h --
 *
 *	Definitions for lig
 *
 *
 *	David Meyer
 *	dmm@1-4-5.net
 *	Thu Apr 16 14:50:33 2009
 *
 *	$Header: /home/dmm/lisp/lig/RCS/lig.h,v 1.35 2009/07/06 17:46:46 dmm Exp $
 *
 */


#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<ctype.h>
#include        <netdb.h>
#include	<strings.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in_systm.h>
#include	<netinet/in.h>
#include	<netinet/udp.h>
#include	<netinet/ip.h>
#include	<netinet/ip6.h>
#include	<arpa/inet.h>
#include	<net/if.h>
#include	<sys/ioctl.h>

typedef enum			{FALSE,TRUE} boolean;
#define GOOD			  0
#define BAD			  -1
#define	MAX_IP_PACKET		4096
#define	COUNT			3
#define MIN_COUNT		1
#define	MAX_COUNT		5
#define MAP_REPLY_TIMEOUT	2*1000	/* milliseconds */
#define	MIN_MR_TIMEOUT		1	/* seconds */
#define	MAX_MR_TIMEOUT		5	/* seconds */
#define	NINTERFACES		10
#define	LOOPBACK		"127.0.0.1"

#define	USAGE	"usage: %s [-d] <eid> -m <map resolver> \
[-c <count>] [-t <timeout>]\n"

/*
 *	CO --
 *
 *	Calculate Offset
 *
 *	Try not to make dumb mistakes with
 *	pointer arithmetic
 *
 */

#define	CO(addr,len) (((char *) addr + len))

/*
 * LISP Types
 */

#define	LISP_MAP_REQUEST	1
#define	LISP_MAP_REPLY		2
#define	LISP_DATA_PORT		4341
#define	LISP_CONTROL_PORT	4342
#define LISP_AFI_IP		1
#define LISP_AFI_IPV6		2
#define	LISP_IP_MASK_LEN	32

/*
 * Map-Request Message Format
 *  
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |S|                     Locator Reach Bits                      |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                             Nonce                             |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |Type=1 |A|R|            Reserved               | Record Count  |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |         Source-EID-AFI        |            ITR-AFI            |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                   Source EID Address  ...                     |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                Originating ITR RLOC Address ...               |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       / |   Reserved    | EID mask-len  |        EID-prefix-AFI         |
 *  Rec <  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *       \ |                       EID-prefix  ...                         |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                   Map-Reply Record  ...                       |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                     Mapping Protocol Data                     |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */


struct map_request_pkt {
#ifdef __LITTLE_ENDIAN
    unsigned int	lisp_loc_reach_bits:31;
    unsigned int	smr_bit:1;
#else
    unsigned int	smr_bit:1;
    unsigned int	lisp_loc_reach_bits:31;
#endif
    unsigned int	lisp_nonce;
#ifdef __LITTLE_ENDIAN
    u_char		rsvd:2;
    u_char		map_data_present:1;
    u_char		auth_bit:1;
    u_char		lisp_type:4;
#else
    u_char		lisp_type:4;
    u_char		auth_bit:1;
    u_char		map_data_present:1;
    u_char		rsvd:2;
#endif
    ushort		reserved0;
    u_char		record_count;
    ushort		source_eid_afi;
    ushort		itr_afi;
    struct in_addr	source_eid;
    struct in_addr	originating_itr_rloc;
    u_char		reserved1;
    u_char		eid_mask_len;
    ushort		eid_prefix_afi;
    struct in_addr	eid_prefix;
} __attribute__ ((__packed__));


struct lisphdr {
#ifdef __LITTLE_ENDIAN
    unsigned int          lisp_loc_reach_bits:31;
    unsigned int          smr_bit:1;
#else
    unsigned int          smr_bit:1;
    unsigned int          lisp_loc_reach_bits:31;
#endif
    unsigned int          lisp_nonce;
} __attribute__ ((__packed__));


/*
 * Map-Reply Message Format
 *   
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |x|                     Locator Reach Bits                      |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                             Nonce                             |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |Type=2 |              Reserved                 | Record Count  |
 *  +----> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |      |                          Record  TTL                          |
 *  |      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  R      | Locator Count | EID mask-len  |A| act |       Reserved        |
 *  e      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  c      |           Reserved            |            EID-AFI            |
 *  o      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  r      |                          EID-prefix                           |
 *  d      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |     /|    Priority   |    Weight     |  M Priority   |   M Weight    |
 *  |    / +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |  Loc |           Unused Flags      |R|           Loc-AFI             | 
 *  |    \ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |     \|                             Locator                           |
 *  +--->  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *         |                     Mapping Protocol Data                     |
 *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct map_reply_pkt {
#ifdef __LITTLE_ENDIAN
    unsigned int   lisp_loc_reach_bits:31;
    unsigned int   not_used:1;
#else
    unsigned int   not_used:1;
    unsigned int   lisp_loc_reach_bits:31;
#endif
    unsigned int   lisp_nonce;
#ifdef __LITTLE_ENDIAN
    int            rsvd:4;
    int            lisp_type:4;
#else
    int            lisp_type:4;
    int            rsvd:4;
#endif
    ushort         reserved;
    u_char         record_count;
    u_char         data[0];
}  __attribute__ ((__packed__));


struct lisp_map_reply_eidtype {
    unsigned int    record_ttl;
    u_char          loc_count;
    u_char          eid_mask_len;
#ifdef __LITTLE_ENDIAN
    int             reserved:12;
    int             action:3;
    int             auth_bit:1;
#else
    int             auth_bit:1;
    int             action:3;
    int             reserved:12;
#endif
    ushort          reserved2;
    ushort          eid_afi;
    u_char          eid_prefix[0];             /* ITR-locator than EID-prefix */
} __attribute__ ((__packed__));

struct lisp_map_reply_loctype {
    u_char  priority;
    u_char  weight;
    u_char  mpriority;
    u_char  mweight;
    u_char  unused_flags1;
#ifdef __LITTLE_ENDIAN
    u_char  reach_bit:1;
    u_char  unused_flags2:7;
#else
    u_char  unused_flags2:7;
    u_char  reach_bit:1;
#endif
    ushort loc_afi;
    u_char  locator[0];
} __attribute__ ((__packed__));

