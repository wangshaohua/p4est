/*
  This file is part of p4est.
  p4est is a C library to manage a collection (a forest) of multiple
  connected adaptive quadtrees or octrees in parallel.

  Copyright (C) 2012 Carsten Burstedde

  p4est is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  p4est is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with p4est; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef P4_TO_P8
#include <p4est_wrap.h>
#else
#include <p8est_wrap.h>
#endif

int
main (int argc, char **argv)
{
  int                 mpiret;
  int                 changed;
#ifdef P4EST_DEBUG
  int                 lp = SC_LP_DEFAULT;
#else
  int                 lp = SC_LP_PRODUCTION;
#endif
  MPI_Comm            mpicomm;
  p4est_wrap_t       *wrap;

  mpiret = MPI_Init (&argc, &argv);
  SC_CHECK_MPI (mpiret);
  mpicomm = MPI_COMM_WORLD;

  sc_init (mpicomm, 0, 0, NULL, lp);
  p4est_init (NULL, lp);

#ifndef P4_TO_P8
  wrap = p4est_wrap_new_unitsquare (mpicomm, 0);
#else
  wrap = p8est_wrap_new_unitcube (mpicomm, 0);
#endif
  changed = p4est_wrap_refine (wrap);
  if (changed) {
    changed = p4est_wrap_partition (wrap);
    if (changed) {
      p4est_wrap_complete (wrap);
    }
  }
  p4est_wrap_destroy (wrap);

  sc_finalize ();

  mpiret = MPI_Finalize ();
  SC_CHECK_MPI (mpiret);

  return 0;
}
