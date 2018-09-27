#include "server.h"
#include "anet.h"
#include "birdisle.h"
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

struct birdisleServer {
    int metafd[2];
    pthread_t thread;
    pthread_mutex_t mutex;  /* Serialises calls to birdisleAddConnection */
};

void *serverThread(void *arg) {
    int exit_code;
    birdisleServer *handle = (birdisleServer *) arg;
    char *argv[4] = {"redis", "--port", "0", NULL};

    exit_code = redisMain(handle->metafd[0], 3, argv);
    return (void *) (intptr_t) exit_code;
}

birdisleServer *birdisleStartServer(void) {
    int err;

    birdisleServer *handle = zmalloc(sizeof(birdisleServer));
    if (pipe(handle->metafd) == -1) {
        goto error1;
    }
    if ((err = pthread_mutex_init(&handle->mutex, NULL)) != 0) {
        errno = err;
        goto error2;
    }
    if ((err = pthread_create(&handle->thread, NULL, serverThread, handle)) != 0) {
        errno = err;
        goto error3;
    }
    return handle;

error3:
    pthread_mutex_destroy(&handle->mutex);
error2:
    close(handle->metafd[0]);
    close(handle->metafd[1]);
error1:
    zfree(handle);
    return NULL;
}

int birdisleStopServer(birdisleServer *handle) {
    int stop = -1, err;
    void *exit_code;

    anetWrite(handle->metafd[1], (char *) &stop, sizeof(stop));
    if ((err = pthread_join(handle->thread, &exit_code)) != 0)
        return -1;
    close(handle->metafd[0]);
    close(handle->metafd[1]);
    zfree(handle);
    return (int) (intptr_t) exit_code;
}

void birdisleAddConnection(birdisleServer *handle, int fd) {
    /* TODO: if metafd was a datagram socket, the lock wouldn't be needed. */
    pthread_mutex_lock(&handle->mutex);
    anetWrite(handle->metafd[1], (char *) &fd, sizeof(fd));
    pthread_mutex_unlock(&handle->mutex);
}
