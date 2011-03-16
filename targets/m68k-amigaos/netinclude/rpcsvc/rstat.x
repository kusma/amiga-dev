/* @(#)rstat.x	2.2 88/08/01 4.0 RPCSRC */
/* @(#)rstat.x 1.2 87/09/18 Copyr 1987 Sun Micro */


/*
 * Gather statistics on remote machines
 */

#ifdef RPC_HDR

%#ifndef FSCALE
%/*
% * Scale factor for scaled integers used to count load averages.
% */
%#define FSHIFT  8               /* bits to right of fixed binary point */
%#define FSCALE  (1<<FSHIFT)
%
%#endif /* ndef FSCALE */

#endif /* def RPC_HDR */

const CPUSTATES = 4;
const DK_NDRIVE = 4;

/*
 * GMT since 0:00, January 1, 1970
 */
struct rstat_timeval {
	unsigned int tv_sec;	/* seconds */
	unsigned int tv_usec;	/* and microseconds */
};

struct statstime {				/* RSTATVERS_TIME */
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	unsigned int v_pgpgin;	/* these are cumulative sum */
	unsigned int v_pgpgout;
	unsigned int v_pswpin;
	unsigned int v_pswpout;
	unsigned int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	unsigned int v_swtch;
	int avenrun[3];         /* scaled by FSCALE */
	rstat_timeval boottime;
	rstat_timeval curtime;
	int if_opackets;
};

struct statsswtch {			/* RSTATVERS_SWTCH */
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	unsigned int v_pgpgin;	/* these are cumulative sum */
	unsigned int v_pgpgout;
	unsigned int v_pswpin;
	unsigned int v_pswpout;
	unsigned int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	unsigned int v_swtch;
	unsigned int avenrun[3];/* scaled by FSCALE */
	rstat_timeval boottime;
	int if_opackets;
};

struct stats {				/* RSTATVERS_ORIG */
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	unsigned int v_pgpgin;	/* these are cumulative sum */
	unsigned int v_pgpgout;
	unsigned int v_pswpin;
	unsigned int v_pswpout;
	unsigned int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	int if_opackets;
};


program RSTATPROG {
	/*
	 * Newest version includes current time and context switching info
	 */
	version RSTATVERS_TIME {
		statstime
		RSTATPROC_STATS(void) = 1;

		unsigned int
		RSTATPROC_HAVEDISK(void) = 2;
	} = 3;
	/*
	 * Does not have current time
	 */
	version RSTATVERS_SWTCH {
		statsswtch
		RSTATPROC_STATS(void) = 1;

		unsigned int
		RSTATPROC_HAVEDISK(void) = 2;
	} = 2;
	/*
	 * Old version has no info about current time or context switching
	 */
	version RSTATVERS_ORIG {
		stats
		RSTATPROC_STATS(void) = 1;

		unsigned int
		RSTATPROC_HAVEDISK(void) = 2;
	} = 1;
} = 100001;
