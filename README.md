# graphAlg

Graph algorithms that use only the **installed** [graphData](https://github.com/david-days/graphdata) public API.

This repository is part of the personal [gc-nav](https://github.com/david-days) program. It must not include graphData `impl/` headers or link an in-tree graphData build.

## Requirements

- CMake 3.25+
- A C17 compiler
- [Check](https://libcheck.github.io/check/) (`check-devel` on Fedora)
- **graphData 1.0.0+** installed so `pkg-config` can find `graphdata`

If `pkg-config --exists graphdata` fails after a `/usr` install, the `.pc` file may live under `/usr/lib/pkgconfig` while the tool searches `lib64`. `./buildLinux.sh` prepends `/usr/lib/pkgconfig` to `PKG_CONFIG_PATH`.

## Build

```sh
./buildLinux.sh        # configure and build
./buildLinux.sh -t     # build and run tests
./buildLinux.sh -p     # also build RPM / tar.gz under output/
./buildLinux.sh -i     # install the library that was just built
./buildLinux.sh -h
```

Version for this stage is **0.1.0** (trial series until the Alg public surface settles).

## Usage

```sh
gcc demo.c $(pkg-config --cflags --libs graphalg)
```

```c
#include <graphAlg.h>
#include <graphInit.h>
#include <graphCrud.h>

const char *v = graphAlgVersion();
/* create graphs with initGraph / getOperations from graphData */
```

## Layout

Flat repo (improvement over graphData’s nested `graphDataLib/`):

| Path | Role |
|------|------|
| `include/` | Installed public headers only |
| `src/` | Library sources (private headers stay here) |
| `tests/` | Check / CTest |
| `examples/` | Small demos |
| `cmake/` | Toolchain and packaging |

## Contributing

Pull requests target `main`. From the repo root, `./buildLinux.sh -t` must pass.
