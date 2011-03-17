#include <boost/algorithm/string.hpp>

#include "Debug.h"
#include "OP2ParallelLoop.h"

void
OP2ParallelLoop::retrieveArgumentTypes (
    OP2DeclaredVariables * op2DeclaredVariables)
{
  using boost::iequals;
  using std::string;
  using std::vector;

  Debug::getInstance ()->debugMessage (
      "Retrieving actual types of OP_DAT arguments", 2);

  /*
   * Scan OP_DAT arguments and get their actual base types.
   * Also determine whether this is a direct/indirect loop by inspecting OP_MAP arguments
   * Note that the first batch of OP_DAT arguments starts at index NUMBER_OF_NON_OP_DAT_ARGUMENTS.
   */
  for (vector <SgExpression *>::iterator it = actualArguments.begin ()
      + OP2::NUMBER_OF_NON_OP_DAT_ARGUMENTS; it != actualArguments.end (); ++it)
  {
    if ((*it)->variantT () == V_SgVarRefExp)
    {
      /*
       * The argument of the OP_PAR_LOOP is a variable reference (expression)
       */
      SgVarRefExp * variableReference = isSgVarRefExp (*it);

      if (variableReference->get_type ()->variantT () == V_SgClassType)
      {
        SgClassType* classReference = isSgClassType (
            variableReference->get_type ());
        ROSE_ASSERT (classReference != NULL);

        string const variableName =
            variableReference->get_symbol ()->get_name ().getString ();

        string const className = classReference->get_name ().getString ();

        if (iequals (className, OP2::OP_DAT_NAME))
        {
          /*
           * Found an OP_DAT variable, so retrieve its dimensions and its
           * type from the arguments passed to OP_DECL_DAT
           */
          try
          {
            OP_DAT_Declaration * opDatDeclaration =
                op2DeclaredVariables->get_OP_DAT_Declaration (variableName);

            /*
             * TODO: this needs to be fixed. OP_DAT variables can be declared in 2 ways:
             * 1) Through OP_DECL_DAT
             * 2) Through OP_DECL_GBL
             * An OP_DECL_GBL has 3 parameters, whereas an OP_DECL_DAT has 4 parameters
             */

            if (opDatDeclaration != NULL)
            {
              set_OP_DAT_Dimension (opDatDeclaration->getDimension ());
              set_OP_DAT_ActualType (opDatDeclaration->getActualType ());
            }
            else
            {
            }
          }
          catch (std::string const & variableName)
          {
            Debug::getInstance ()->debugMessage (
                "'" + variableName
                    + "' has not been declared through OP_DECL_DAT. It must have been declared using OP_DECL_GBL",
                1);
          }
        }
        else if (iequals (className, OP2::OP_MAP_NAME))
        {
          if (iequals (variableName, OP2::OP_ID_NAME) == false)
          {
            /*
             * OP_ID signals identity mapping and therefore direct access to the data.
             * If we discover an OP_MAP that is not an OP_ID then an indirect loop
             * is instead assumed
             */
            isDirect = false;
          }
        }
      }
    }
  }

  if (isDirect)
  {
    Debug::getInstance ()->debugMessage ("'" + userHostFunctionName
        + "' is a DIRECT loop", 5);
  }
  else
  {
    Debug::getInstance ()->debugMessage ("'" + userHostFunctionName
        + "' is an INDIRECT loop", 5);
  }
}
