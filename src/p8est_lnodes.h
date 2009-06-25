/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2009 Carsten Burstedde, Lucas Wilcox.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef P8EST_LNODES_H
#define P8EST_LNODES_H

#include <p8est.h>

SC_EXTERN_C_BEGIN;

/** Store a parallel numbering of Lobatto points of a given degree > 0.
 *
 * Each element has degree+1 nodes per edge
 * and vnodes = (degree+1)^3 nodes per volume.
 * num_local_elements is the number of local quadrants in the p8est.
 * local_nodes is of dimension vnodes * num_local_elements
 * and indexes into the array global_nodes layed out as follows:
 * global_nodes = [<--------------->|<-------------------->|          ]
 *                  \ owned_offset    \ num_owned_nodes
 *                 <------------------------------------------------->
 *                  \ num_indep_nodes
 * global_nodes contains the globally unique numbers for independent nodes.
 * Hanging nodes are always local and don't have a global number.
 * They index the geometrically corresponding global indep_node of a neighbor.
 * Whether nodes are hanging or not will be encoded separately.
 *
 * Independent nodes can be shared by multiple MPI ranks.
 * The owner rank of a node is the one from the lowest numbered element
 * on the lowest numbered octree sharing the node.
 * sharers_offset is of dimension num_indep_nodes+1 and contains the offset
 * into sharers, which is of dimension sharers_offset[num_indep_nodes].
 * The sharers array is sorted by independent node.  The owner rank is first,
 * since by construction the owner rank is the minimum of the sharer ranks.
 */
typedef struct p8est_lnodes
{
  int                 degree, vnodes;
  p4est_locidx_t      num_local_elements;
  p4est_locidx_t      num_indep_nodes;
  p4est_locidx_t      owned_offset, num_owned_nodes;
  p4est_locidx_t     *local_nodes;
  p4est_gloidx_t     *global_nodes;
  int                *sharers_offset;
  int                *sharers;
}
p8est_lnodes_t;

p8est_lnodes_t     *p8est_lnodes_new (p8est_t * p8est,
                                      sc_array_t * ghost_layer, int degree);
void                p8est_lnodes_destroy (p8est_lnodes_t *);

SC_EXTERN_C_END;

#endif /* !P8EST_LNODES */