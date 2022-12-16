# libnp [![builds.sr.ht status](https://builds.sr.ht/~delthas/libnp.svg)](https://builds.sr.ht/~delthas/libnp)

A tiny cross-platform library for extracting information about the music/image/video that is **N**ow **P**laying on the system.

Supported OS:
- Windows (using GlobalSystemMediaTransportControlsSessionManager)
- Linux (using DBus/MPRIS)

## Building

```
cmake .
cmake --build .
```

## Usage

See [`include/np.h`](include/np.h) and [`np/main.c`](np/main.c) for an example.

The general idea is:
```c
np_init();

struct np_info *info = np_info_get();
if (info) {
    // do something with info->track, ...

    np_info_destroy(info);
}

np_destroy();
```

## License

MIT
