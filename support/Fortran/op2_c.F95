


module OP2_FORTRAN_DECLARATIONS


use, intrinsic :: ISO_C_BINDING

  integer, parameter :: MAX_NAME_LEN = 100

  ! accessing operation codes
  integer(4) :: OP_READ =   1
  integer(4) :: OP_WRITE =  2
  integer(4) :: OP_INC =    3
  integer(4) :: OP_RW =     4
  integer(4) :: OP_MIN =    5
  integer(4) :: OP_MAX =    6

  type op_set
  integer:: id, size
  end type op_set

  type op_map
  type(op_set) :: from, to
  integer:: id, dim, size
  end type op_map

  type op_dat
  type(op_set) :: set
  integer:: id, dim
  end type op_dat

  ! declaration of identity map
  type(op_map) :: OP_ID, OP_GBL

  interface op_decl_dat
    module procedure op_decl_dat_real_8_1, op_decl_dat_integer_4_1, op_decl_dat_real_8_2, op_decl_dat_integer_4_2, op_decl_dat_real_8_3, op_decl_dat_integer_4_3
  end interface op_decl_dat
 
  interface op_decl_gbl
     module procedure op_decl_gbl_real_8_0, &
                    & op_decl_gbl_real_8_1, &
                    & op_decl_gbl_real_8_2, &
                    & op_decl_gbl_real_8_3, &
                    & op_decl_gbl_integer_4_0, &
                    & op_decl_gbl_integer_4_1, &
                    & op_decl_gbl_integer_4_2, &
                    & op_decl_gbl_integer_4_3, &
                    & op_decl_gbl_logical_0, &
                    & op_decl_gbl_logical_1
  end interface op_decl_gbl     

 interface op_decl_const
     module procedure op_decl_const_scalar_logical, &
                    & op_decl_const_scalar_real_4, &
                    & op_decl_const_scalar_real_8, &
                    & op_decl_const_scalar_integer_4, &
                    & op_decl_const_logical, &
                    & op_decl_const_real_4, &
                    & op_decl_const_real_8, &
                    & op_decl_const_real_8_2, &
                    & op_decl_const_integer_4
  end interface op_decl_const

contains

  subroutine op_init ()
    
  end subroutine op_init

  subroutine op_decl_set ( size, set, setname )
    
    integer :: size
    type(op_set) :: set
		character(kind=c_char,len=*) :: setname

  end subroutine op_decl_set


  subroutine op_decl_map ( from , to, dim, dat, outmap, mapname )
    type(op_set) :: from, to
    integer :: dim
    integer(4), dimension(*) :: dat
    type(op_map) :: outmap
    character(kind=c_char,len=*) :: mapname
  end subroutine op_decl_map

  subroutine op_decl_dat_real_8_1(set, dim, dat, data, datname)
    type(op_set) :: set
    integer :: dim
    real(8), dimension(:) :: dat
    type(op_dat) :: data
		character(kind=c_char,len=*) :: datname
  end subroutine op_decl_dat_real_8_1
  
  subroutine op_decl_dat_integer_4_1(set, dim, dat, data, datname)
    type(op_set) :: set
    integer :: dim
    integer(4), dimension(:) :: dat
    type(op_dat) :: data  
		character(kind=c_char,len=*) :: datname
  end subroutine op_decl_dat_integer_4_1

  subroutine op_decl_dat_real_8_2(set, dim, dat, data, datname)
    type(op_set) :: set
    integer :: dim
    real(8), dimension(:,:) :: dat
    type(op_dat) :: data
		character(kind=c_char,len=*) :: datname
  end subroutine op_decl_dat_real_8_2
  
  subroutine op_decl_dat_integer_4_2(set, dim, dat, data, datname)
    type(op_set) :: set
    integer :: dim
    integer(4), dimension(:,:) :: dat
    type(op_dat) :: data  
		character(kind=c_char,len=*) :: datname
  end subroutine op_decl_dat_integer_4_2

  subroutine op_decl_dat_real_8_3(set, dim, dat, data, datname)
    type(op_set) :: set
    integer :: dim
    real(8), dimension(:,:,:) :: dat
    type(op_dat) :: data
		character(kind=c_char,len=*) :: datname
  end subroutine op_decl_dat_real_8_3
  
  subroutine op_decl_dat_integer_4_3(set, dim, dat, data, datname)
    type(op_set) :: set
    integer :: dim
    integer(4), dimension(:,:,:) :: dat
    type(op_dat) :: data  
		character(kind=c_char,len=*) :: datname
  end subroutine op_decl_dat_integer_4_3

  subroutine op_decl_gbl_real_8_0 ( dat, data)
    real(8) :: dat   
    type(op_dat) :: data
  end subroutine op_decl_gbl_real_8_0
  
  subroutine op_decl_gbl_real_8_1 ( dat, data, dim)
    real(8), dimension(:) :: dat
    type(op_dat) :: data
    integer :: dim
  end subroutine op_decl_gbl_real_8_1

  subroutine op_decl_gbl_real_8_2 ( dat, data, dim)
    real(8), dimension(:,:) :: dat
    type(op_dat) :: data
    integer :: dim
  end subroutine op_decl_gbl_real_8_2

  subroutine op_decl_gbl_integer_4_2 ( dat, data, dim)
    integer(4), dimension(:,:) :: dat    
    type(op_dat) :: data
    integer :: dim
  end subroutine op_decl_gbl_integer_4_2

  subroutine op_decl_gbl_real_8_3 ( dat, data, dim)
    real(8), dimension(:,:,:) :: dat
    type(op_dat) :: data
    integer :: dim
  end subroutine op_decl_gbl_real_8_3

  subroutine op_decl_gbl_integer_4_0 ( dat, data)
    integer(4) :: dat
    type(op_dat) :: data    
  end subroutine op_decl_gbl_integer_4_0

  subroutine op_decl_gbl_integer_4_1 ( dat, data, dim)
    integer(4), dimension(:) :: dat
    type(op_dat) :: data
    integer :: dim
  end subroutine op_decl_gbl_integer_4_1

  subroutine op_decl_gbl_integer_4_3 ( dat, data, dim)
    integer(4), dimension(:,:,:) :: dat
    type(op_dat) :: data
    integer :: dim
  end subroutine op_decl_gbl_integer_4_3

    subroutine op_decl_gbl_logical_0 ( dat, data)
        
        logical :: dat
        type(op_dat) :: data
        
    end subroutine op_decl_gbl_logical_0

    subroutine op_decl_gbl_logical_1 ( dat, data, gbldim)
        
        logical, dimension(*):: dat        
        type(op_dat) :: data
        integer :: gbldim
       
    end subroutine op_decl_gbl_logical_1

  subroutine op_decl_const_scalar_logical ( data, dim )
    
    integer(4) :: dim
    logical :: data
  
  end subroutine op_decl_const_scalar_logical

  subroutine op_decl_const_scalar_real_4 ( data, dim )
    
    integer(4) :: dim
    real(4) :: data
  
  end subroutine op_decl_const_scalar_real_4

  subroutine op_decl_const_scalar_real_8 ( data, dim )
    
    integer(4) :: dim
    real(8) :: data
  
  end subroutine op_decl_const_scalar_real_8

  subroutine op_decl_const_scalar_integer_4 ( data, dim )
    
    integer(4) :: dim
    integer(4) :: data
  
  end subroutine op_decl_const_scalar_integer_4



  subroutine op_decl_const_logical ( data, dim )
    
    integer(4) :: dim
    logical, dimension(:) :: data
  
  end subroutine op_decl_const_logical

  subroutine op_decl_const_real_4 ( data, dim )
    
    integer(4) :: dim
    real(4), dimension(:) :: data
  
  end subroutine op_decl_const_real_4

  subroutine op_decl_const_real_8 ( data, dim )
    
    integer(4) :: dim
    real(8), dimension(:) :: data
  
  end subroutine op_decl_const_real_8

  subroutine op_decl_const_integer_4 ( data, dim )
    
    integer(4) :: dim
    integer(4), dimension(:) :: data
  
  end subroutine op_decl_const_integer_4

  subroutine op_decl_const_real_8_2 ( data, dim1, dim2 )
    
    integer(4) :: dim1, dim2
    real(8), dimension(:,:) :: data
  
  end subroutine op_decl_const_real_8_2


  
  subroutine op_par_loop_2 ( subroutineName, set, &
                           & data0, itemSel0, map0, access0, &
                           & data1, itemSel1, map1, access1 &
                         & )  

  external subroutineName

!  interface
!    subroutine subroutineName ( d0, d1 )
!      real(8), dimension(*) :: d0, d1
!    end subroutine
!  end interface
  type(op_set) :: set
  type(op_dat) :: data0, data1
  integer:: itemSel0, itemSel1, access0, access1
  type(op_map) :: map0, map1

  end subroutine op_par_loop_2

  subroutine op_par_loop_5 ( subroutineName, set, &
                             & data0, itemSel0, map0, access0, &
                             & data1, itemSel1, map1, access1, &
                             & data2, itemSel2, map2, access2, &
                             & data3, itemSel3, map3, access3, &
                             & data4, itemSel4, map4, access4  &
                           & )

  external subroutineName
!  interface
!    subroutine subroutineName ( d0, d1, d2, d3, d4 )
!    end subroutine
!  end interface
                            

  type(op_set) :: set
  type(op_dat) :: data0, data1, data2, data3, data4
  integer:: itemSel0, itemSel1, itemSel2, itemSel3, itemSel4
  integer :: access0, access1, access2, access3, access4
  type(op_map) :: map0, map1, map2, map3, map4


  end subroutine op_par_loop_5

  
  subroutine op_par_loop_6 ( subroutineName, set, &
                             & data0, itemSel0, map0, access0, &
                             & data1, itemSel1, map1, access1, &
                             & data2, itemSel2, map2, access2, &
                             & data3, itemSel3, map3, access3, &
                             & data4, itemSel4, map4, access4, &
                             & data5, itemSel5, map5, access5  &
                           & )

  external subroutineName
!  interface
!    subroutine subroutineName ( d0, d1, d2, d3, d4, d5 )
!    end subroutine
!  end interface

  type(op_set) :: set
  type(op_dat) :: data0, data1, data2, data3, data4, data5
  integer :: itemSel0, itemSel1, itemSel2, itemSel3, itemSel4, itemSel5 
  integer :: access0, access1, access2, access3, access4, access5
  type(op_map) :: map0, map1, map2, map3, map4, map5
  


          
    end subroutine op_par_loop_6

  
  subroutine op_par_loop_8 ( subroutineName, set, &
                             & data0, itemSel0, map0, access0, &
                             & data1, itemSel1, map1, access1, &
                             & data2, itemSel2, map2, access2, &
                             & data3, itemSel3, map3, access3, &
                             & data4, itemSel4, map4, access4, &
                             & data5, itemSel5, map5, access5, &
                             & data6, itemSel6, map6, access6, &
                             & data7, itemSel7, map7, access7  &
                           & )

  external subroutineName
!  interface
!    subroutine subroutineName ( d0, d1, d2, d3, d4, d5, d6, d7 )
!    end subroutine
!  end interface
  
  type(op_set) :: set
  type(op_dat) :: data0, data1, data2, data3, data4, data5, data6, data7
  integer :: itemSel0, itemSel1, itemSel2, itemSel3, itemSel4, itemSel5, itemSel6, itemSel7
  integer :: access0, access1, access2, access3, access4, access5, access6, access7
  type(op_map) :: map0, map1, map2, map3, map4, map5, map6, map7
                  
    end subroutine op_par_loop_8

  

end module OP2_FORTRAN_DECLARATIONS
