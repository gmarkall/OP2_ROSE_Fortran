#ifndef KERNEL_SUBROUTINE_H
#define KERNEL_SUBROUTINE_H

#include <Subroutine.h>
#include <ParallelLoop.h>

class KernelSubroutine: public Subroutine
{
  protected:

    std::string userSubroutineName;

    ParallelLoop * parallelLoop;

  protected:

    KernelSubroutine (std::string const & subroutineName,
        std::string const & userSubroutineName, ParallelLoop * parallelLoop);
};

#endif
