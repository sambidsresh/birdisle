#include "server.h"
#include "birdisle.h"

int main(int argc, char **argv)
{
    return redisMain(-1, argc, argv, NULL);
}
