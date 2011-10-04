
#include <stdio.h>
#include "omp_testsuite.h"



int
check_section_private (FILE * logFile)
{
  int sum = 7;
  int sum0 = 0;
  int known_sum;
  int i;
#pragma omp parallel
  {
#pragma omp  sections private(sum0,i)
    {
#pragma omp section
      {
	sum0 = 0;
	for (i = 1; i < 400; i++)
	  sum0 = sum0 + i;
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
#pragma omp section
      {
	sum0 = 0;
	for (i = 400; i < 700; i++)
	  sum0 = sum0 + i;
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
#pragma omp section
      {
	sum0 = 0;
	for (i = 700; i < 1000; i++)
	  sum0 = sum0 + i;
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
    }				/*end of sections */
  }				/* end of parallel */
  known_sum = (999 * 1000) / 2 + 7;
  return (known_sum == sum);
}				/* end of check_section_private */


int
crosscheck_section_private (FILE * logFile)
{
  int sum = 7;
  int sum0 = 0;
  int known_sum;
  int i;
#pragma omp parallel
  {
#pragma omp  sections private(i)
    {
#pragma omp section
      {
	sum0 = 0;
	for (i = 1; i < 400; i++)
	  sum0 = sum0 + i;
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
#pragma omp section
      {
	sum0 = 0;
	for (i = 400; i < 700; i++)
	  sum0 = sum0 + i;
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
#pragma omp section
      {
	sum0 = 0;
	for (i = 700; i < 1000; i++)
	  sum0 = sum0 + i;
#pragma omp critical
	{
	  sum = sum + sum0;
	}			/*end of critical */
      }
    }				/*end of sections */
  }				/* end of parallel */
  known_sum = (999 * 1000) / 2 + 7;
  return (known_sum == sum);
}				/* end of check_section_private */
