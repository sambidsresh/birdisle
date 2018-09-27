#ifndef BIRDISLE_H
#define BIRDISLE_H

typedef struct birdisleServer birdisleServer;

#define BIRDISLE_EXPORT __attribute__((visibility("default")))

birdisleServer BIRDISLE_EXPORT *birdisleStartServer(void);
int BIRDISLE_EXPORT birdisleStopServer(birdisleServer *handle);
void BIRDISLE_EXPORT birdisleAddConnection(birdisleServer *handle, int fd);

#endif
