#include "server.h"
#include "libredis.h"

int main(int argc, char **argv)
{
    return redisMain(argc, argv);
}
