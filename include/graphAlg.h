/**
 * @file graphAlg.h
 * @brief Public entry for graphAlg — algorithms over the installed graphData API.
 *
 * Consumers include this header (and graphData's public headers as needed) and
 * link with pkg-config: `pkg-config --cflags --libs graphalg`.
 * Algorithm scratch space belongs to the caller or to graphAlg internals — never
 * on graph_t via impl/ headers.
 */

#ifndef GRAPHALG_H
#define GRAPHALG_H

#include "graphAlgCut.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Library version string, e.g. "0.1.0". */
const char *graphAlgVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* GRAPHALG_H */
