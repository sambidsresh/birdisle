# Birdisle - an in-process redis

Birdisle (an anagram of "lib redis") is a modified version of
[redis](https://redis.io) that runs as a library inside another process. The
primary aim is to simplify unit testing by providing a way to run tests
against what appears to be a redis server, but without the hassle of starting a
separate process and ensuring that it is torn down correctly.

Because this repository is a clone of the redis
[repository](https://github.com/antirez/redis), many of the files, including
documentation, are relevant to redis rather than birdisle.

## API

Birdisle has a very simple API, but as it is in early development it is still
subject to breaking changes. The API is in `birdisle.h`.

Call `birdisleStartServer(config)` to start a thread running redis. It takes the contents
of a config file (optional) and returns an opaque handle of type `birdisleServer *`, or
`NULL` if startup failed. Note that not all configuration is safe to set — see the
[Limitations](#limitations) below.

To create a connection to the server, call `birdisleAddConnection(handle, fd)`
where `handle` is the handle returned by `birdisleStartServer`, and `fd` is the
end of a socket that birdisle will use for communication. It could, for
example, be one end of a socketpair(2).

To terminate and clean up a server, call `birdisleStopServer(handle)`.

## Bindings

- Python: [birdisle-py](https://github.com/bmerry/birdisle-py)

## Limitations

Redis was never designed to be embedded in another process. To make it behave
nicely with other code in the same process, some parts of redis that make it
robust in a production environment have been disabled. It should thus **not**
be used in a production environment where data persistence is a concern.

Specific variances from stock redis include

- Some actions that would normally run in the background now run in the
  foreground, blocking the entire server.
- The number of open file handles is not adjusted. Attempting to make large
  numbers of connections may have dire consequences.
- Lua debugging is not supported (although ``SCRIPT DEBUG sync`` might work).
- Sentinel is not supported.
- Cluster mode is not supported.
- Modules that create background threads are not supported.
- RDB files have limited support. The ``BGSAVE`` command is not supported at
  all, and the RDB file will only be updated on explicit request via ``SAVE``
  or ``SHUTDOWN``.
- AOF files have limited support. It cannot be enabled after the server has
  started, and rewriting (e.g. with ``BGREWRITEAOF``) is not supported.
- It does not catch signals. If a graceful shutdown is required, the host
  process must catch and handle the signal.
- Logging is disabled by default.
- Server shutdown is slower, because it takes some care to release all the
  allocated resources, rather than leaving it to the OS to clean up.

For the moment, "not supported" means "may segfault or catch fire if you try to
use it." In future these features will be properly disabled so that attempts to
use them fail gracefully.

## Status

Birdisle is still in very early development, and is likely to still contain
bugs and rough edges. Bug reports and pull requests are welcome.
