#~----------------------------------------------------------------------------~#
#  Copyright (c) 2017 Los Alamos National Security, LLC
#  All rights reserved.
#~----------------------------------------------------------------------------~#

#~----------------------------------------------------------------------------~#
# 
# /@@@@@@@@  @@           @@@@@@   @@@@@@@@ @@@@@@@  @@      @@
# /@@/////  /@@          @@////@@ @@////// /@@////@@/@@     /@@
# /@@       /@@  @@@@@  @@    // /@@       /@@   /@@/@@     /@@
# /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@@@@@@ /@@@@@@@@@@
# /@@////   /@@/@@@@@@@/@@       ////////@@/@@////  /@@//////@@
# /@@       /@@/@@//// //@@    @@       /@@/@@      /@@     /@@
# /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@      /@@     /@@
# //       ///  //////   //////  ////////  //       //      //  
#
#~---------------------------------------------------------------------------~*#

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/mpisph/test)
include_directories(${CMAKE_SOURCE_DIR}/mpisph/)


#------------------------------------------------------------------------------#
# tree topology tests
#------------------------------------------------------------------------------#

cinch_add_unit(tree3d
  SOURCES
    test/tree3d.cc
    ${FleCSI_RUNTIME}/runtime_driver.cc  
  LIBRARIES 
    ${FleCSPH_LIBRARIES}
)

cinch_add_unit(mpi_qsort
  SOURCES
    test/mpi_qsort.cc
    ${FleCSI_RUNTIME}/runtime_driver.cc  
  LIBRARIES ${FleCSPH_LIBRARIES}
  POLICY MPI 
  THREADS 4 
)

cinch_add_unit(io
  SOURCES
    test/io.cc
    ${FleCSI_RUNTIME}/runtime_driver.cc  
  INPUTS
    test/io_test.h5part
  LIBRARIES ${FleCSPH_LIBRARIES}
  POLICY MPI 
  THREADS 4 
)


#~---------------------------------------------------------------------------~-#
# Formatting options
# vim: set tabstop=2 shiftwidth=2 expandtab :
#~---------------------------------------------------------------------------~-#

