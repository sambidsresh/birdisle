# Birdisle - an in-process redis

Birdisle (an anagram of "lib redis") is a modified version of
[redis](https://redis.io) that run as a library inside another process. The
primary aim is to simplify unit testing by providing a way to run tests
against what appears to be a redis server, but without the hassle of starting a
separate process and ensuring that it is torn down correctly.

Because this repository is a clone of the redis
[repository](https://github.com/antirez/redis), many of the files, including
documentation, are relevant to redis rather than birdisle.

## Bindings

- Python: [birdisle-py](https://github.com/bmerry/birdisle-py)

## Limitations

Redis was never designed to be embedded in another process. To make it behave
nicely with other code in the same process, some parts of redis that make it
robust in a production environment need to be disabled. It should thus **not**
be used in a production environment where data persistence is a concern. It
should also not be used with sentinel or in cluster mode as this is totally
untested and I have no idea what will happen.

Redis was also never designed to have multiple instances running in the same
address space (it has a lot of global state). It's been made to work by making
it strictly single-threaded and making all the state thread-local, but modules
that create background threads should **not** be used.

## Status

Birdisle is still in **very** early development, and is not yet ready for use.
Documentation will only be written once the interfaces have been worked out.
