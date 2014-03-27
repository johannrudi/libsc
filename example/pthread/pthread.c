/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include <pthread.h>
#include <sc_options.h>

static void        *
start_thread (void *v)
{
  int                 i = *(int *) v;
  int                 j;
  int                *p;

  SC_INFOF ("This is thread %d\n", i);
  p = SC_ALLOC (int, 1000);
  for (j = 0; j < 1000; ++j) {
    p[j] = j;
  }
  SC_FREE (p);

  /* automatically calls pthread_exit (v) */
  return v;
}

static void
test_threads (int N)
{
  int                 i;
  int                 pth;
  int                *ids;
  void               *exitval;
  pthread_attr_t      attr;
  pthread_t          *threads;

  ids = SC_ALLOC (int, N);
  threads = SC_ALLOC (pthread_t, N);

  /* create and run threads */
  pth = pthread_attr_init (&attr);
  SC_CHECK_ABORT (pth == 0, "Fail in pthread_attr_init");
  pth = pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
  SC_CHECK_ABORT (pth == 0, "Fail in pthread_attr_setdetachstate");
  for (i = 0; i < N; ++i) {
    ids[i] = i;
    pth = pthread_create (&threads[i], &attr, &start_thread, &ids[i]);
    SC_CHECK_ABORT (pth == 0, "Fail in pthread_create");
  }
  pth = pthread_attr_destroy (&attr);
  SC_CHECK_ABORT (pth == 0, "Fail in pthread_attr_destroy");

  /* wait for threads to finish */
  for (i = 0; i < N; ++i) {
    pth = pthread_join (threads[i], &exitval);
    SC_CHECK_ABORT (pth == 0, "Fail in pthread_join");
    SC_ASSERT (exitval == &ids[i]);
  }

  SC_FREE (threads);
  SC_FREE (ids);
}

int
main (int argc, char **argv)
{
  int                 mpiret;
  int                 first_arg;
  int                 N;
  sc_options_t       *opt;

  mpiret = sc_MPI_Init (&argc, &argv);
  SC_CHECK_MPI (mpiret);

  sc_init (sc_MPI_COMM_WORLD, 1, 1, NULL, SC_LP_DEFAULT);

  opt = sc_options_new (argv[0]);
  sc_options_add_int (opt, 'N', "num-threads", &N, 0, "Number of threads");

  first_arg = sc_options_parse (sc_package_id, SC_LP_ERROR, opt, argc, argv);
  if (first_arg != argc || N < 0) {
    sc_options_print_usage (sc_package_id, SC_LP_ERROR, opt, NULL);
    sc_abort_collective ("Option parsing failed");
  }
  else {
    sc_options_print_summary (sc_package_id, SC_LP_PRODUCTION, opt);
  }

  test_threads (N);

  sc_options_destroy (opt);
  sc_finalize ();

  mpiret = sc_MPI_Finalize ();
  SC_CHECK_MPI (mpiret);

  return 0;
}
