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
    char *config;
};

void *serverThread(void *arg) {
    int exit_code;
    birdisleServer *handle = (birdisleServer *) arg;
    char *argv[] = {"redis", NULL};

    exit_code = redisMain(
        handle->metafd[0], sizeof(argv) / sizeof(argv[0]) - 1, argv, handle->config);
    return (void *) (intptr_t) exit_code;
}

birdisleServer *birdisleStartServer(const char *config) {
    int err;
    int close_fd = 1;   /* Whether we should close the server's side of metafd */

    if (!config)
        config = "";
    birdisleServer *handle = zmalloc(sizeof(birdisleServer));
    handle->config = zstrdup(config);
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, handle->metafd) == -1) {
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
    close_fd = 0;

    /* Wait for the server to indicate it is ready for connections, or die */
    while (1) {
        char ready;
        int status;
        status = anetRead(handle->metafd[1], &ready, 1);
        if (status == 1) {
            break;
        } else if (status != -1) {
            /* The server closed the metasocket without indicating readiness */
            errno = 0;
            goto error4;
        } else if (errno != EINTR) {
            goto error4;
        }
    }

    return handle;

error4:
    pthread_join(handle->thread, NULL);
error3:
    pthread_mutex_destroy(&handle->mutex);
error2:
    if (close_fd) {
        close(handle->metafd[0]);
    }
    close(handle->metafd[1]);
error1:
    zfree(handle->config);
    zfree(handle);
    return NULL;
}

int birdisleStopServer(birdisleServer *handle) {
    int stop = -1, err;
    void *exit_code;

    anetWrite(handle->metafd[1], (char *) &stop, sizeof(stop));
    if ((err = pthread_join(handle->thread, &exit_code)) != 0)
        return -1;
    close(handle->metafd[1]);
    zfree(handle->config);
    zfree(handle);
    return (int) (intptr_t) exit_code;
}

void birdisleAddConnection(birdisleServer *handle, int fd) {
    /* TODO: if metafd was a datagram socket, the lock wouldn't be needed. */
    pthread_mutex_lock(&handle->mutex);
    anetWrite(handle->metafd[1], (char *) &fd, sizeof(fd));
    pthread_mutex_unlock(&handle->mutex);
}
