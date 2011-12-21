#pragma once
#ifndef CPP_OP2_DEFINITIONS_H
#define CPP_OP2_DEFINITIONS_H

#include <OP2Definitions.h>
#include <rose.h>

class SgExprListExp;

class CPPImperialOpDatDefinition: public OpDatDefinition
{
    /*
     * ======================================================
     * Models an OP_DAT definition in C++ (Imperial API).
     *
     * The following function prototype is assumed:
     * OP_DECL_DAT (op_set, int, template<T> *)
     * ======================================================
     */

  public:

    static int const indexOpSet = 0;

    static int const indexDimension = 1;

    static int const indexDataArray = 2;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 3;
    }

    CPPImperialOpDatDefinition (SgExprListExp * parameters,
        std::string const & variableName);
};

class CPPImperialOpSetDefinition: public OpSetDefinition
{
    /*
     * ======================================================
     * Models an OP_SET definition in C++ (Imperial API).
     *
     * The following function prototype is assumed:
     * OP_DECL_SET (int)
     * ======================================================
     */

  public:

    static int const indexDimension = 0;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 1;
    }

    CPPImperialOpSetDefinition (SgExprListExp * parameters,
        std::string const & variableName);
};

class CPPOxfordOpSubSetDefinition: public OpSubSetDefinition
{
    /*
     * ======================================================
     * Models an OP_SUBSET definition in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_DECL_SUBSET (op_set, givenname, kernel, op_arg...)
     * ======================================================
     */
    
private:
    SgExprListExp* parameters;
	SgFunctionDefinition* wrapperFunction;
    
public:
    static int const indexOriginSet = 0;
	static int const indexGivenName = 1;
    static int const indexFilterKernel = 2;
    
    SgNode* getOpArgDat(int i);
	
	SgFunctionDefinition* getFilterWrapperFunction ();
	void setFilterWrapperFunction (SgFunctionDefinition* wrapper);
    
    CPPOxfordOpSubSetDefinition (SgExprListExp * parameters,
                                std::string const & variableName);
};

class CPPImperialOpMapDefinition: public OpMapDefinition
{
    /*
     * ======================================================
     * Models an OP_MAP definition in C++ (Imperial API).
     *
     * The following function prototype is assumed:
     * OP_DECL_MAP (op_set, op_set, int, int *)
     * ======================================================
     */

  public:

    static int const indexOpSetSource = 0;

    static int const indexOpSetDestination = 1;

    static int const indexDimension = 2;

    static int const indexMappingArray = 3;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 4;
    }

    CPPImperialOpMapDefinition (SgExprListExp * parameters,
        std::string const & variableName);
};

class CPPImperialOpConstDefinition: public OpConstDefinition
{
    /*
     * ======================================================
     * Models a constant definition in C++ (Imperial API).
     *
     * The following function prototype is assumed:
     * OP_DECL_CONST (int, template<T> *)
     * ======================================================
     */

  public:

    static int const indexDimenson = 0;

    static int const indexData = 1;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 2;
    }

    CPPImperialOpConstDefinition (SgExprListExp * parameters);
};

class CPPImperialOpArgDatCall
{
    /*
     * ======================================================
     * Models a call to OP_ARG_DAT in C++ (Imperial API).
     *
     * The following function prototype is assumed:
     * OP_ARG_DAT (op_dat, int, op_map, op_access)
     * ======================================================
     */

  public:

    static int const indexOpDat = 0;

    static int const indexOpIndex = 1;

    static int const indexOpMap = 2;

    static int const indexAccessDescriptor = 3;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 4;
    }
};

class CPPImperialOpArgGblCall
{
    /*
     * ======================================================
     * Models a call to OP_ARG_GBL in C++ (Imperial API).
     *
     * The following function prototype is assumed:
     * OP_ARG_GBL (op_dat, op_access)
     * ======================================================
     */

  public:

    static int const indexOpDat = 0;

    static int const indexAccessDescriptor = 1;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 2;
    }
};

class CPPOxfordOpDatDefinition: public OpDatDefinition
{
    /*
     * ======================================================
     * Models an OP_DAT definition in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_DECL_DAT (op_set, int, char *, template<T> *, char *)
     * ======================================================
     */

  public:

    static int const indexOpSet = 0;

    static int const indexDimension = 1;

    static int const indexTypeName = 2;

    static int const indexDataArray = 3;

    static int const indexOpDatName = 4;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 5;
    }

    CPPOxfordOpDatDefinition (SgExprListExp * parameters,
        std::string const & variableName);
};

class CPPOxfordOpSetDefinition: public OpSetDefinition
{
    /*
     * ======================================================
     * Models an OP_SET definition in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_DECL_SET (int, char *)
     * ======================================================
     */

  public:

    static int const indexDimension = 0;

    static int const indexOpSetName = 1;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 2;
    }

    CPPOxfordOpSetDefinition (SgExprListExp * parameters,
        std::string const & variableName);
};

class CPPOxfordOpMapDefinition: public OpMapDefinition
{
    /*
     * ======================================================
     * Models an OP_MAP definition in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_DECL_MAP (op_set, op_set, int, int *, char *)
     * ======================================================
     */

  public:

    static int const indexOpSetSource = 0;

    static int const indexOpSetDestination = 1;

    static int const indexDimension = 2;

    static int const indexMappingArray = 3;

    static int const indexOpMapName = 4;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 5;
    }

    CPPOxfordOpMapDefinition (SgExprListExp * parameters,
        std::string const & variableName);
};

class CPPOxfordOpConstDefinition: public OpConstDefinition
{
    /*
     * ======================================================
     * Models a constant definition in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_DECL_CONST (int, char *, template<T> *)
     * ======================================================
     */

  public:

    static int const indexDimension = 0;

    static int const indexTypeName = 1;

    static int const indexData = 2;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 3;
    }

    CPPOxfordOpConstDefinition (SgExprListExp * parameters);
};

class CPPOxfordOpArgDatCall
{
    /*
     * ======================================================
     * Models a call to OP_ARG_DAT in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_ARG_DAT (op_dat, int, op_map, int, char *, op_access)
     * ======================================================
     */

  public:

    static int const indexOpDat = 0;

    static int const indexOpIndex = 1;

    static int const indexOpMap = 2;

    static int const indexDimension = 3;

    static int const indexTypeName = 4;

    static int const indexAccessDescriptor = 5;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 6;
    }
};

class CPPOxfordOpArgGblCall
{
    /*
     * ======================================================
     * Models a call to OP_ARG_GBL in C++ (Oxford API).
     *
     * The following function prototype is assumed:
     * OP_ARG_GBL (op_dat, int, char *, op_access)
     * ======================================================
     */

  public:

    static int const indexOpDat = 0;

    static int const indexDimension = 1;

    static int const indexTypeName = 2;

    static int const indexAccessDescriptor = 3;

  public:

    static unsigned int
    getNumberOfExpectedArguments ()
    {
      return 4;
    }
};

#endif
