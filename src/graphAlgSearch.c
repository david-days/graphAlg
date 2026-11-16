#include <graphAlgSearch.h>

#include <stdlib.h>
#include <string.h>
#include <graphCrud.h>
#include <stdbool.h>
#include <math.h>

#include "util/hashset.h"
#include "util/hashtable.h"
#include "util/heap.h"
#include "util/lists.h"
#include "util/queue.h"
#include "util/stack.h"

static void freeScratchPads(size_t *node_scratch, size_t *idxscratch, size_t *parents, size_t *dist, hashset seen,
                            queue q, stack s, hashtable id_map) {
    free(node_scratch);
    free(idxscratch);
    free(parents);
    free(dist);
    hs_free(seen);
    s_free(s);
    q_free(q);

    ht_free(id_map);
}

void graphAlgPathResultDestroy(graphAlgPathResult *r) {
    if (r == NULL) {
        return;
    }

    free(r->nodes);
    free(r->dist);
    free(r->parent);
    memset(r, 0, sizeof(*r));
}

int graphAlgBfsPath(struct graphops_t *ops, struct graph_t *g,
                    size_t source, size_t target,
                    graphAlgPathResult *out) {
    if (ops == NULL || g == NULL || out == NULL
        || ops->getNeighbors == NULL || ops->listNodes == NULL) {
        return GRAPH_ERR;
    }

    // get count and dense list of id values
    size_t keysize = sizeof(size_t);
    struct node_t *nlist = ops->listNodes(g);
    if (nlist == NULL) {
        destroyNodes((void **) &nlist);
        return GRAPH_ERR;
    }
    size_t ncount = 1;
    struct node_t *curr = nlist;
    while (curr->next != NULL) {
        curr = curr->next;
        ncount++;
    }

    //tracking structures
    size_t *parents = calloc(ncount, keysize);
    size_t *dist = calloc(ncount, keysize);
    size_t *nids = calloc(ncount, sizeof(size_t));
    size_t *nindices = calloc(ncount, sizeof(size_t));
    hashtable id_map = ht_create(ncount);

    //null check for all these creations
    if (parents == NULL || dist == NULL || nids == NULL || nindices == NULL || id_map == NULL) {
        free(parents);
        free(dist);
        free(nids);
        free(nindices);
        ht_free(id_map);
        destroyNodes((void **) &nlist);
        return GRAPH_ERR;
    }
    curr = nlist;
    size_t nidx = 0;
    while (curr != NULL) {
        nids[nidx] = curr->nodeid;
        nindices[nidx] = nidx;
        ht_put(id_map, &nids[nidx], keysize, &nindices[nidx]);
        curr = curr->next;
        nidx++;
    }
    destroyNodes((void **) &nlist);


    size_t d = 0;

    //init arrays. SIZE_MAX is our no-entry value
    for (size_t i = 0; i < ncount; i++) {
        parents[i] = SIZE_MAX;
        dist[i] = SIZE_MAX;
    }

    hashset seen = hs_create(ncount);
    queue q = q_create();
    if (seen == NULL || q == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        return GRAPH_ERR;
    }

    //starter set
    size_t *v = NULL;
    size_t *idx = NULL;
    if (ht_get(id_map, &source, keysize, (void **) &idx)) {
        hs_add(seen, &nids[*idx], keysize);
        parents[*idx] = *idx;
        q_enqueue(q, &nids[*idx]);
        dist[*idx] = d;
    }  else {
        //error--didn't get the id that we originally put in
        memset(out, 0, sizeof(*out));
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        return GRAPH_ERR;
    }

    //search
    bool found = false;
    size_t *pidx = NULL;
    while (!q_empty(q) && !found) {
        q_dequeue(q, (void **) &v);
        if (*v == target) {
            found = true;
            break;
        }
        if (!ht_get(id_map, v, keysize, (void **) &pidx)) {
            //unknown in our list--continue
            continue;
        }
        struct node_t *neighbors = ops->getNeighbors(v, g);
        if (neighbors == NULL) {
            //no children/neighbors
            continue;
        }
        struct node_t *nbr = neighbors;
        d = dist[*pidx] + 1;
        // walk the neighbors, setting parentage, dist, and seen, then adding to the queue
        while (nbr != NULL) {
            if (!ht_get(id_map, &nbr->nodeid, keysize, (void **) &idx)) {
                //unknown in our list--continue
                nbr = nbr->next;
                continue;
            }
            //check if we've seen this one--skip if so
            if (hs_contains(seen, &nbr->nodeid, keysize)) {
                nbr = nbr->next;
                continue;
            }
            //not seen--process
            hs_add(seen, &nids[*idx], keysize);
            q_enqueue(q, &nids[*idx]);
            parents[*idx] = *pidx;
            dist[*idx] = d;
            nbr = nbr->next;
        }
        destroyNodes((void **) &neighbors);
    }
    //check if stopped because target was found
    if (found) {
        size_t *tidx = NULL;
        if (!ht_get(id_map, &target, keysize, (void **) &tidx)) {
            freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
            return GRAPH_ERR;
        }
        size_t hops = dist[*tidx];
        out->n = hops + 1;
        out->dist = (size_t *) malloc(sizeof(size_t) * out->n);
        if (out->dist == NULL) {
            out->dist = NULL;
            out->n = 0;
            freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
            memset(out, 0, sizeof(*out));
            return GRAPH_ERR;
        }
        out->parent = (size_t *) malloc(sizeof(size_t) * out->n);
        if (out->parent == NULL) {
            freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
            out->parent = NULL;
            free(out->dist);
            out->dist = NULL;
            out->n = 0;
            memset(out, 0, sizeof(*out));
            return GRAPH_ERR;
        }
        out->nodes = (size_t *) malloc(sizeof(size_t) * out->n);
        if (out->nodes == NULL) {
            freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
            out->nodes = NULL;
            free(out->parent);
            out->parent = NULL;
            free(out->dist);
            out->dist = NULL;
            out->n = 0;
            memset(out, 0, sizeof(*out));
            return GRAPH_ERR;
        }

        size_t currn = *tidx;
        for (size_t i = hops; ; i--) {
            out->nodes[i] = nids[currn];
            out->dist[i] = dist[currn];
            if (i == 0) {
                out->parent[i] = nids[currn];
                break;
            }
            out->parent[i] = nids[parents[currn]];
            currn = parents[currn];
        }
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        return GRAPH_OK;
    }
    freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
    memset(out, 0, sizeof(*out));
    return GRAPH_ERR;
}

int graphAlgBfsFrom(struct graphops_t *ops, struct graph_t *g,
                    size_t source,
                    graphAlgPathResult *out) {
    if (ops == NULL || g == NULL || out == NULL
        || ops->getNeighbors == NULL || ops->listNodes == NULL) {
        return GRAPH_ERR;
        }

    // get count and dense list of id values
    size_t keysize = sizeof(size_t);
    struct node_t *nlist = ops->listNodes(g);
    if (nlist == NULL) {
        destroyNodes((void **) &nlist);
        return GRAPH_ERR;
    }
    size_t ncount = 1;
    struct node_t *curr = nlist;
    while (curr->next != NULL) {
        curr = curr->next;
        ncount++;
    }

    //tracking structures
    size_t *parents = calloc(ncount, keysize);
    size_t *dist = calloc(ncount, keysize);
    size_t *nids = calloc(ncount, sizeof(size_t));
    size_t *nindices = calloc(ncount, sizeof(size_t));
    hashtable id_map = ht_create(ncount);
    list seenOrder = list_create();

    //null check for all these creations
    if (parents == NULL || dist == NULL || nids == NULL || nindices == NULL || id_map == NULL
        || seenOrder == NULL) {
        free(parents);
        free(dist);
        free(nids);
        free(nindices);
        ht_free(id_map);
        list_free(seenOrder);
        destroyNodes((void **) &nlist);
        return GRAPH_ERR;
    }
    curr = nlist;
    size_t nidx = 0;
    while (curr != NULL) {
        nids[nidx] = curr->nodeid;
        nindices[nidx] = nidx;
        ht_put(id_map, &nids[nidx], keysize, &nindices[nidx]);
        curr = curr->next;
        nidx++;
    }
    destroyNodes((void **) &nlist);


    size_t d = 0;

    //init arrays. SIZE_MAX is our no-entry value
    for (size_t i = 0; i < ncount; i++) {
        parents[i] = SIZE_MAX;
        dist[i] = SIZE_MAX;
    }

    hashset seen = hs_create(ncount);
    queue q = q_create();
    if (seen == NULL || q == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        list_free(seenOrder);
        return GRAPH_ERR;
    }

    //starter set
    size_t *v = NULL;
    size_t *idx = NULL;
    if (ht_get(id_map, &source, keysize, (void **) &idx)) {
        hs_add(seen, &nids[*idx], keysize);
        list_add(seenOrder, &nids[*idx], 0.0);
        parents[*idx] = *idx;
        q_enqueue(q, &nids[*idx]);
        dist[*idx] = d;
    } else {
        //error--didn't get the id that we originally put in
        memset(out, 0, sizeof(*out));
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        list_free(seenOrder);
        return GRAPH_ERR;
    }


    size_t *pidx = NULL;
    while (!q_empty(q)) { //start treewalk loop
        q_dequeue(q, (void **) &v);
        if (!ht_get(id_map, v, keysize, (void **) &pidx)) {
            //unknown in our list--continue
            continue;
        }
        struct node_t *neighbors = ops->getNeighbors(v, g);
        if (neighbors == NULL) {
            //no children/neighbors
            continue;
        }
        struct node_t *nbr = neighbors;
        d = dist[*pidx] + 1;
        // walk the neighbors, setting parentage, dist, and seen, then adding to the queue
        while (nbr != NULL) {
            if (!ht_get(id_map, &nbr->nodeid, keysize, (void **) &idx)) {
                //unknown in our list--continue
                nbr = nbr->next;
                continue;
            }
            //check if we've seen this one--skip if so
            if (hs_contains(seen, &nbr->nodeid, keysize)) {
                nbr = nbr->next;
                continue;
            }
            //not seen--process
            hs_add(seen, &nids[*idx], keysize);
            list_add(seenOrder, &nids[*idx], 0.0);
            q_enqueue(q, &nids[*idx]);
            parents[*idx] = *pidx;
            dist[*idx] = d;
            nbr = nbr->next;
        }
        destroyNodes((void **) &neighbors);
    } // end treewalk-loop

    // Create compact output for result
    size_t lcount = seenOrder->count;
    out->n = lcount;
    out->dist = (size_t *) malloc(sizeof(size_t) * out->n);
    if (out->dist == NULL) {
        out->dist = NULL;
        out->n = 0;
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        list_free(seenOrder);
        memset(out, 0, sizeof(*out));
        return GRAPH_ERR;
    }
    out->parent = (size_t *) malloc(sizeof(size_t) * out->n);
    if (out->parent == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        out->parent = NULL;
        free(out->dist);
        out->dist = NULL;
        out->n = 0;
        list_free(seenOrder);
        memset(out, 0, sizeof(*out));
        return GRAPH_ERR;
    }
    out->nodes = (size_t *) malloc(sizeof(size_t) * out->n);
    if (out->nodes == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
        out->nodes = NULL;
        free(out->parent);
        out->parent = NULL;
        free(out->dist);
        out->dist = NULL;
        out->n = 0;
        list_free(seenOrder);
        memset(out, 0, sizeof(*out));
        return GRAPH_ERR;
    }

    //walk the id array, setting the values
    list_item currn = seenOrder->head;
    size_t curridx = 0;
    size_t currval;
    size_t *currp = NULL;
    while (currn != NULL) {
        currval = *(size_t *)(currn->data);
        out->nodes[curridx] = currval;
        if (!ht_get(id_map, &currval, keysize, (void **)&currp)) {
            //clean up and throw error
            freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
            free(out->nodes);
            out->nodes = NULL;
            free(out->parent);
            out->parent = NULL;
            free(out->dist);
            out->dist = NULL;
            out->n = 0;
            list_free(seenOrder);
            memset(out, 0, sizeof(*out));
            return GRAPH_ERR;
        }
        out->dist[curridx] = dist[*currp];
        out->parent[curridx] = nids[parents[*currp]];
        curridx++;
        currn = currn->next;
    }
    freeScratchPads(nids, nindices, parents, dist, seen, q, NULL, id_map);
    list_free(seenOrder);
    return GRAPH_OK;
}


int graphAlgDfsFrom(struct graphops_t *ops, struct graph_t *g,
                    size_t source,
                    graphAlgPathResult *out) {
    if (ops == NULL || g == NULL || out == NULL
        || ops->getNeighbors == NULL || ops->listNodes == NULL) {
        return GRAPH_ERR;
        }

    // get count and dense list of id values
    size_t keysize = sizeof(size_t);
    struct node_t *nlist = ops->listNodes(g);
    if (nlist == NULL) {
        destroyNodes((void **) &nlist);
        return GRAPH_ERR;
    }
    size_t ncount = 1;
    struct node_t *curr = nlist;
    while (curr->next != NULL) {
        curr = curr->next;
        ncount++;
    }

    //tracking structures
    size_t *parents = calloc(ncount, keysize);
    size_t *dist = calloc(ncount, keysize);
    size_t *nids = calloc(ncount, sizeof(size_t));
    size_t *nindices = calloc(ncount, sizeof(size_t));
    hashtable id_map = ht_create(ncount);
    list seenOrder = list_create();

    //null check for all these creations
    if (parents == NULL || dist == NULL || nids == NULL || nindices == NULL || id_map == NULL
        || seenOrder == NULL) {
        free(parents);
        free(dist);
        free(nids);
        free(nindices);
        ht_free(id_map);
        list_free(seenOrder);
        destroyNodes((void **) &nlist);
        return GRAPH_ERR;
    }
    curr = nlist;
    size_t nidx = 0;
    while (curr != NULL) {
        nids[nidx] = curr->nodeid;
        nindices[nidx] = nidx;
        ht_put(id_map, &nids[nidx], keysize, &nindices[nidx]);
        curr = curr->next;
        nidx++;
    }
    destroyNodes((void **) &nlist);


    size_t d = 0;

    //init arrays. SIZE_MAX is our no-entry value
    for (size_t i = 0; i < ncount; i++) {
        parents[i] = SIZE_MAX;
        dist[i] = SIZE_MAX;
    }

    hashset seen = hs_create(ncount);
    stack s = s_create();
    if (seen == NULL || s == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
        list_free(seenOrder);
        return GRAPH_ERR;
    }

    //starter set
    size_t *v = NULL;
    size_t *idx = NULL;
    if (ht_get(id_map, &source, keysize, (void **) &idx)) {
        hs_add(seen, &nids[*idx], keysize);
        list_add(seenOrder, &nids[*idx], 0.0);
        parents[*idx] = *idx;
        s_push(s, &nids[*idx]);
        dist[*idx] = d;
    } else {
        //error--didn't get the id that we originally put in
        memset(out, 0, sizeof(*out));
        freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
        list_free(seenOrder);
        return GRAPH_ERR;
    }


    size_t *pidx = NULL;
    while (!s_empty(s)) { //start treewalk loop
        s_pop(s, (void **) &v);
        if (!ht_get(id_map, v, keysize, (void **) &pidx)) {
            //unknown in our list--continue
            continue;
        }
        struct node_t *neighbors = ops->getNeighbors(v, g);
        if (neighbors == NULL) {
            //no children/neighbors
            continue;
        }
        struct node_t *nbr = neighbors;
        d = dist[*pidx] + 1;
        // walk the neighbors, setting parentage, dist, and seen, then adding to the stack
        while (nbr != NULL) {
            if (!ht_get(id_map, &nbr->nodeid, keysize, (void **) &idx)) {
                //unknown in our list--continue
                nbr = nbr->next;
                continue;
            }
            //check if we've seen this one--skip if so
            if (hs_contains(seen, &nbr->nodeid, keysize)) {
                nbr = nbr->next;
                continue;
            }
            //not seen--process
            hs_add(seen, &nids[*idx], keysize);
            list_add(seenOrder, &nids[*idx], 0.0);
            s_push(s, &nids[*idx]);
            parents[*idx] = *pidx;
            dist[*idx] = d;
            nbr = nbr->next;
        }
        destroyNodes((void **) &neighbors);
    } // end treewalk-loop

    // Create compact output for result
    size_t lcount = seenOrder->count;
    out->n = lcount;
    out->dist = (size_t *) malloc(sizeof(size_t) * out->n);
    if (out->dist == NULL) {
        out->dist = NULL;
        out->n = 0;
        freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
        list_free(seenOrder);
        memset(out, 0, sizeof(*out));
        return GRAPH_ERR;
    }
    out->parent = (size_t *) malloc(sizeof(size_t) * out->n);
    if (out->parent == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
        out->parent = NULL;
        free(out->dist);
        out->dist = NULL;
        out->n = 0;
        list_free(seenOrder);
        memset(out, 0, sizeof(*out));
        return GRAPH_ERR;
    }
    out->nodes = (size_t *) malloc(sizeof(size_t) * out->n);
    if (out->nodes == NULL) {
        freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
        out->nodes = NULL;
        free(out->parent);
        out->parent = NULL;
        free(out->dist);
        out->dist = NULL;
        out->n = 0;
        list_free(seenOrder);
        memset(out, 0, sizeof(*out));
        return GRAPH_ERR;
    }

    //walk the id array, setting the values
    list_item currn = seenOrder->head;
    size_t curridx = 0;
    size_t currval;
    size_t *currp = NULL;
    while (currn != NULL) {
        currval = *(size_t *)(currn->data);
        out->nodes[curridx] = currval;
        if (!ht_get(id_map, &currval, keysize, (void **)&currp)) {
            //clean up and throw error
            freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
            free(out->nodes);
            out->nodes = NULL;
            free(out->parent);
            out->parent = NULL;
            free(out->dist);
            out->dist = NULL;
            out->n = 0;
            list_free(seenOrder);
            memset(out, 0, sizeof(*out));
            return GRAPH_ERR;
        }
        out->dist[curridx] = dist[*currp];
        out->parent[curridx] = nids[parents[*currp]];
        curridx++;
        currn = currn->next;
    }
    freeScratchPads(nids, nindices, parents, dist, seen, NULL, s, id_map);
    list_free(seenOrder);
    return GRAPH_OK;
}

void graphAlgComponentsResultDestroy(graphAlgComponentsResult *r) {
    if (r == NULL) {
        return;
    }
    if (r->nodes != NULL) {
        for (size_t i=0; i <r->n; i++) {
            free(r->nodes[i]);
        }
    }

    free(r->nodes);
    free(r->sizes);
    memset(r, 0, sizeof(*r));
}

static void freeCCompTracking(list clist, struct node_t *nlist, graphAlgComponentsResult *ccomps, hashset seen) {
    void *item = NULL;
    if (clist != NULL) {
        while (clist->head != NULL) {
            if (list_removeAt(clist, 0, &item)) {
                graphAlgPathResultDestroy((graphAlgPathResult *)item);
                free(item);
            }
        }
    }
    list_free(clist);
    destroyNodes((void **)&nlist);
    hs_free(seen);
    graphAlgComponentsResultDestroy(ccomps);
}

int graphAlgConnectedComponents(struct graphops_t *ops, struct graph_t *graph,
                                graphAlgComponentsResult *out) {
    if (ops == NULL || graph == NULL || out == NULL || ops->listNodes == NULL
        || ops->getNeighbors == NULL || ops->nodeCount == NULL) {
        return GRAPH_ERR;
    }

    list ccomp_list = list_create();
    if (ccomp_list == NULL) {
        return GRAPH_ERR;
    }
    struct node_t *nlist = ops->listNodes(graph);
    if (nlist == NULL) {
        memset(out, 0, sizeof(*out));
        list_free(ccomp_list);
        return GRAPH_OK;
    }

    size_t ncount = ops->nodeCount(graph);
    hashset seen = hs_create(ncount);
    if (seen == NULL) {
        freeCCompTracking(ccomp_list, nlist, NULL, NULL);
        return GRAPH_ERR;
    }


    struct node_t *curr = nlist;
    size_t keysize = sizeof(size_t);
    while (curr != NULL) {
        if (hs_contains(seen, &curr->nodeid, keysize)) {
            curr = curr->next;
            continue;
        }
        graphAlgPathResult *ccomp = malloc(sizeof(graphAlgPathResult));
        if (ccomp == NULL) {
            freeCCompTracking(ccomp_list, nlist, NULL, seen);
            return GRAPH_ERR;
        }
        memset(ccomp, 0, sizeof(graphAlgPathResult));

        if (graphAlgDfsFrom(ops, graph, curr->nodeid, ccomp) == GRAPH_OK) {
            for (size_t i=0; i<ccomp->n; i++) {
                //add to list, no need to check
                hs_add(seen, &ccomp->nodes[i], keysize);
            }
            list_add(ccomp_list, ccomp, 0.0);
        } else {
            free(ccomp);
            freeCCompTracking(ccomp_list, nlist, NULL, seen);
            return GRAPH_ERR;
        }
        curr = curr->next;
    }

    memset(out, 0, sizeof(graphAlgComponentsResult));
    out->n = ccomp_list->count;
    graphAlgPathResult *ccomp = NULL;
    if (out->n > 0) {
        out->nodes = calloc(out->n, sizeof(size_t *));
        out->sizes = malloc(out->n * sizeof(size_t));
        if (out->nodes == NULL || out->sizes == NULL) {
            graphAlgComponentsResultDestroy(out);
            freeCCompTracking(ccomp_list, nlist, NULL, seen);
            return GRAPH_ERR;
        }
        for (size_t i=0; i< out->n; i++) {
            if (!list_getAt(ccomp_list, i, (void **)&ccomp)) {
                graphAlgComponentsResultDestroy(out);
                freeCCompTracking(ccomp_list, nlist, NULL, seen);
                return GRAPH_ERR;
            }
            size_t *cnodes = malloc(sizeof(size_t) * ccomp->n);
            if (cnodes == NULL) {
                graphAlgComponentsResultDestroy(out);
                freeCCompTracking(ccomp_list, nlist, NULL, seen);
                return GRAPH_ERR;
            }
            memcpy(cnodes, ccomp->nodes, sizeof(size_t) * ccomp->n);
            out->nodes[i] = cnodes;
            out->sizes[i] = ccomp->n;
            graphAlgPathResultDestroy(ccomp);
            ccomp = NULL;
        }
    }
    freeCCompTracking(ccomp_list, nlist, NULL, seen);
    return GRAPH_OK;
}

void graphAlgWeightedPathResultDestroy(graphAlgWeightedPathResult *r) {
    if (r == NULL) {
        return;
    }
    free(r->nodes);
    free(r->dist);
    free(r->parent);
    memset(r, 0, sizeof(*r));
}


int graphAlgDijkstraPath(struct graphops_t *ops, struct graph_t *g,
                         size_t source, size_t target,
                         graphAlgEdgeWeightFn weight_fn, void *ctx,
                         graphAlgWeightedPathResult *out) {
    if (ops == NULL || g == NULL || out == NULL
    || ops->getNeighbors == NULL || ops->getEdges == NULL
    || ops->listNodes == NULL) {
        return GRAPH_ERR;
    }

    //set to known state
    memset(out, 0, sizeof(*out));

    // tracking structures and counts
    size_t keysize = sizeof(size_t);
    struct node_t *nlist = ops->listNodes(g);
    if (nlist == NULL) {
        destroyNodes((void **)&nlist);
        return GRAPH_ERR;
    }

    size_t ncount = 1;
    struct node_t *curr = nlist;
    while (curr->next != NULL) {
        curr = curr->next;
        ncount++;
    }

    struct edge_t *nbr = NULL;
    size_t *parents = calloc(ncount, keysize);
    heap pq = NULL;
    double *dist = calloc(ncount, sizeof(double));
    size_t *nids = calloc(ncount, sizeof(size_t));
    size_t *nindices = calloc(ncount, sizeof(size_t));
    size_t *hops = calloc(ncount, sizeof(size_t));
    bool *done = calloc(ncount, sizeof(bool));
    hashtable id_map = ht_create(ncount);
    //null check for all these creations
    if (parents == NULL || dist == NULL || nids == NULL || nindices == NULL
        || hops == NULL || done == NULL || id_map == NULL) {
        goto fail;
    }

    //fill the id mapping and the arrays
    curr = nlist;
    size_t nidx = 0;
    while (curr != NULL) {
        nids[nidx] = curr->nodeid;
        nindices[nidx] = nidx;
        dist[nidx] = INFINITY;
        hops[nidx] = SIZE_MAX;
        ht_put(id_map, &nids[nidx], keysize, &nindices[nidx]);
        curr = curr->next;
        nidx++;
    }
    //no longer need to hold the original node listing
    destroyNodes((void **)&nlist);
    nlist = NULL;

    size_t *idx = NULL;
    pq = h_create(ncount * 3, true);
    if (pq == NULL) {
        //allocation error. clean up and throw error code
        goto fail;
    }
    //special--mark source dist 0.0;
    if (ht_get(id_map, &source, keysize, (void **)&idx)) {
        parents[*idx] = source;
        dist[*idx] = 0.0;
        hops[*idx] = 0;
        if (!h_push(pq, &source, 0.0)) {
            //had an error--clear out and return err code
            goto fail;
        }
    } else {
        //error--source wasn't in the list or we had some other inconsistency
        goto fail;
    }

    //begin loop
    size_t *u = NULL;
    size_t *uidx = NULL;
    size_t *vidx = NULL;
    struct edge_t *curr_edge;
    bool found = false;
    while (pq->size > 0) {
        h_pop(pq, (void **)&u);
        if (!ht_get(id_map, u, keysize, (void **)&uidx)) {
            goto fail;
        }
        /* Stale: worse earlier push of the same node. */
        if (done[*uidx]) {
            continue;
        }
        done[*uidx] = true; /* first extract ⇒ optimal (non-negative weights) */

        if (*u == target) {
            found = true;
            break;
        }

        nbr = ops->getEdges(u, g);
        curr_edge = nbr;
        while (curr_edge != NULL) {
            size_t vid = curr_edge->v;
            if (ht_get(id_map, &vid, keysize, (void **)&vidx)) {
                double wt_val = weight_fn != NULL ? weight_fn(*u, vid, curr_edge->cap, curr_edge->flow, ctx) : curr_edge->cap;
                if (wt_val == INFINITY) {
                    // inf = skip
                    curr_edge = curr_edge->next;
                    continue;
                }
                if (isnan(wt_val) || wt_val < 0 || wt_val == -INFINITY) {
                    // we had an error--clean up and return error code
                    goto fail;
                }
                if (dist[*vidx] > dist[*uidx] + wt_val) {
                    dist[*vidx] = dist[*uidx] + wt_val;
                    parents[*vidx] = *u;
                    hops[*vidx] = hops[*uidx] + 1;
                    if (!h_push(pq, &nids[*vidx], dist[*vidx]) ) {
                        // push error--clear everything out
                        goto fail;
                    }
                }
            } else {
                goto fail;
            }
            curr_edge = curr_edge->next;
        }
        destroyEdges((void **)&nbr);
        nbr = NULL;
        vidx = NULL;
        curr_edge = NULL;
        u = NULL;
        uidx = NULL;
    }
    if (!found) {
        // we finished but never came across our target
        goto fail;
    }
    if (nbr != NULL) {
        destroyEdges((void **)&nbr);
    }
    //we're going to backtrack
    if (ht_get(id_map, &source, keysize, (void **)&uidx)
        && ht_get(id_map, &target, keysize, (void **)&vidx)) {
        size_t hcount = hops[*vidx]+1;
        out->n = hcount;
        out->nodes = malloc(sizeof(size_t) * hcount);
        out->parent = malloc(sizeof(size_t) * hcount);
        out->dist = malloc(sizeof(double) * hcount);
        if (out->nodes == NULL ||out->parent == NULL || out->dist == NULL) {
            goto fail;
        }

        out->nodes[0] = source;
        out->parent[0] = source;
        out->dist[0] = 0.0;
        while (vidx != uidx && hops[*vidx] > 0) {
            out->nodes[hops[*vidx]] = nids[*vidx];
            out->parent[hops[*vidx]] = parents[*vidx];
            out->dist[hops[*vidx]] = dist[*vidx];
            if (!ht_get(id_map, &parents[*vidx], keysize, (void **)&vidx)) {
                goto fail;
            }
        }
    } else {
        goto fail;
    }
    free(parents);
    free(dist);
    free(nids);
    free(nindices);
    free(hops);
    free(done);
    ht_free(id_map);
    while (!h_empty(pq)) {
        h_pop(pq, (void **)&idx);
    }
    h_free(pq);
    return GRAPH_OK;

fail:
    if (out != NULL) {
        free(out->nodes);
        free(out->parent);
        free(out->dist);
        memset(out, 0, sizeof(*out));
    }
    destroyEdges((void **)&nbr);
    destroyNodes((void **)&nlist);
    free(hops);
    free(done);
    free(parents);
    free(dist);
    free(nids);
    free(nindices);
    ht_free(id_map);
    if (pq != NULL) {
        void * ignored;
        while (!h_empty(pq)) {
            h_pop(pq, (void **)&ignored);
        }
        h_free(pq);
    }
    return GRAPH_ERR;

}
