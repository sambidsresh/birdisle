#ifndef BIRDISLE_H
#define BIRDISLE_H

typedef struct birdisleServer birdisleServer;

#define BIRDISLE_EXPORT __attribute__((visibility("default")))

/* Start a thread running a server. The config may be NULL, which is equivalent
 * to an empty string.
 */
birdisleServer BIRDISLE_EXPORT *birdisleStartServer(const char *config);

/* Stop a server previously started with birdisleStartServer. The return value
 * is the exit code of the server (so 0 means a clean shutdown).
 */
int BIRDISLE_EXPORT birdisleStopServer(birdisleServer *handle);

/* Connect to the a running server. The fd is the end of a stream socket that
 * the server will read/write.
 */
void BIRDISLE_EXPORT birdisleAddConnection(birdisleServer *handle, int fd);

#endif
