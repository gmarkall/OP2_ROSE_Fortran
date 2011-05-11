/*
 * Written by Adam Betts and Carlo Bertolli
 *
 * This class models subroutines to be generated by the
 * compiler
 */

#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <rose.h>
#include <Declarations.h>

class Subroutine
{
  protected:

    /*
     * ======================================================
     * The name of the generated subroutine
     * ======================================================
     */
    std::string subroutineName;

    /*
     * ======================================================
     * Access to this statement allows ROSE to build function
     * call expressions to the generated subroutine. This
     * is needed, for example, when patching the user-supplied
     * code
     * ======================================================
     */
    SgProcedureHeaderStatement * subroutineHeaderStatement;

    /*
     * ======================================================
     * The parameters of the newly created subroutine
     * ======================================================
     */
    SgFunctionParameterList * formalParameters;

    /*
     * ======================================================
     * The scope of the newly created subroutine
     * ======================================================
     */
    SgScopeStatement * subroutineScope;

    /*
     * ======================================================
     * Local variable declarations in the newly created
     * subroutine
     * ======================================================
     */
    std::map <std::string, SgVariableDeclaration *> localVariables;

  protected:

    Subroutine (std::string const & subroutineName)
    {
      this->subroutineName = subroutineName;
    }

  public:

    std::string const &
    getSubroutineName () const
    {
      return subroutineName;
    }

    SgProcedureHeaderStatement *
    getSubroutineHeaderStatement ()
    {
      return subroutineHeaderStatement;
    }
};

#endif
