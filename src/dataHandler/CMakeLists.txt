ADD_SUBDIRECTORY( io )

FILE( GLOB DATAHANDLER_SRC "*.cpp" )
ADD_LIBRARY( dataHandler SHARED ${DATAHANDLER_SRC} )

# Unit tests
IF( CXXTEST_FOUND )
  CXXTEST_ADD_TEST( unittest_WMetaInfo 
                    WMetaInfo_test.cc 
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WMetaInfo_test.h )
  TARGET_LINK_LIBRARIES( unittest_WMetaInfo dataHandler )
  
  CXXTEST_ADD_TEST( unittest_WGrid 
                    WGrid_test.cc 
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WGrid_test.h )
  TARGET_LINK_LIBRARIES( unittest_WGrid dataHandler )
  
  CXXTEST_ADD_TEST( unittest_WValueSet 
                    WValueSet_test.cc 
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WValueSet_test.h )
  TARGET_LINK_LIBRARIES( unittest_WValueSet dataHandler )
  
  CXXTEST_ADD_TEST( unittest_WDataSet 
                    WDataSet_test.cc 
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WDataSet_test.h )
  TARGET_LINK_LIBRARIES( unittest_WDataSet dataHandler )
  
  CXXTEST_ADD_TEST( unittest_WDataSetSingle 
                    WDataSetSingle_test.cc 
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WDataSetSingle_test.h )
  TARGET_LINK_LIBRARIES( unittest_WDataSetSingle dataHandler )              
  
  CXXTEST_ADD_TEST( unittest_WDataSetTimeDependent
                    WDataSetTimeDependent_test.cc 
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WDataSetTimeDependent_test.h )
  TARGET_LINK_LIBRARIES( unittest_WDataSetTimeDependent dataHandler )                    
  
  CXXTEST_ADD_TEST( unittest_WDataHandler 
                    WDataHandler_test.cc
                    ${CMAKE_CURRENT_SOURCE_DIR}/test/WDataHandler_test.h )
  TARGET_LINK_LIBRARIES( unittest_WDataHandler dataHandler )
  
  
  
ENDIF( CXXTEST_FOUND )
