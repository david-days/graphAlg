# Examples

## version_demo

Prints `graphAlgVersion()`. Built with the library; also registered as a CTest.

Against an install:

```sh
gcc version_demo.c $(pkg-config --cflags --libs graphalg)
```

`graphalg.pc` requires `graphdata`, so pkg-config pulls both.
