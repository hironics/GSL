

/* $Id$ */
/* Driver routine for the uniform random number generators */
#include <stdio.h>
#include <stdlib.h>
#include "gsl_ran.h"

int
main (int argc, char **argv)
{
  int i, n = 1;
  double mu = 1.0;
  double rmax;
  int randseed = 17;
  if (argc == 1)
    {
      printf ("Usage: %s <mu> <n> [seed]\n", argv[0]);
      printf ("          Writes <n> random numbers\n");
      printf ("          Poisson distributed wiht mean <mu>,\n");
      printf ("          optionally using <seed>\n");
      exit (0);
    }
  if (argc > 1)
    mu = atof (argv[1]);
  if (argc > 2)
    n = atoi (argv[2]);
  if (argc > 3)
    {
      randseed = atoi (argv[3]);
      gsl_ran_seed (randseed);
    }

  for (i = 0; i < n; ++i)
    {
      printf ("%d\n", gsl_ran_poisson (mu));
    }
  return 0;
}
