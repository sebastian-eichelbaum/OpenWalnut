CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET( Boost_USE_MULTITHREAD ON )
FIND_PACKAGE( Boost REQUIRED program_options thread filesystem date_time system signals regex )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

ADD_EXECUTABLE( numCores WNumCores.cpp )
TARGET_LINK_LIBRARIES( numCores ${Boost_LIBRARIES} )
