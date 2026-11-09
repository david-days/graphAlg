#include <graphAlgSearch.h>

#include <stdlib.h>
#include <string.h>
#include <graphCrud.h>
#include <stdbool.h>

#include "util/hashset.h"
#include "util/hashtable.h"
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
