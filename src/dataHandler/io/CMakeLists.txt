ADD_SUBDIRECTORY( nifti )
ADD_SUBDIRECTORY( biosig )
ADD_SUBDIRECTORY( libeep )

FILE( GLOB DATAHANDLER_IO_SRC "*.cpp" "*.h" )

# Unit tests
IF( OW_COMPILE_TESTS )
  CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_IO_SRC}"
                               "dataHandler;niftiio;biosig;libeep"
                               "WWriter.cpp" # too simple to test
                               "WReader.cpp" # too simple to test
                               "WWriterLookUpTableVTK.cpp" # there is only one member function which is not trivial, but its more an integration test
                               "WReaderLookUpTableVTK.cpp" # there is only one member function which is not trivial, but its more an integration test
                               "WWriterFiberVTK.cpp" # there is only one member function which is not trivial, but its more an integration test
                             )
ENDIF( OW_COMPILE_TESTS )
