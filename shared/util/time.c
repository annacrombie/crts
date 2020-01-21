#include "util/time.h"
#include "util/log.h"

long
sleep_remaining(struct timespec *start, long dur, long slept_ns)
{
	long elapsed_ns;
	struct timespec now;

	clock_gettime(CLOCK_REALTIME, &now);

	elapsed_ns =
		((now.tv_sec - start->tv_sec) * NS_IN_S) +
		(now.tv_nsec - start->tv_nsec) - slept_ns;
	//fprintf(stderr, "%ld dur, %ld ns elapsed, sleeping for %ld", dur, elapsed_ns, dur - elapsed_ns);

	*start = now;

	if (elapsed_ns > 0 && dur > elapsed_ns) {
		now.tv_sec = 0;
		slept_ns = now.tv_nsec = dur - elapsed_ns;
		nanosleep(&now, NULL);
	} else {
		slept_ns = 0;
	}

	return slept_ns;
}
