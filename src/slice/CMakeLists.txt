set( CMAKE_CXX_FLAGS "-lIce -lIceUtil -lIceGrid -lIceBox -lIceStorm -lIceStormService" ) # Opciones para el compilador
set( SLICE2PY_COMMAND /usr/bin/slice2py)

FILE(GLOB ICE_FILES "*.ice")

INCLUDE_DIRECTORIES(
  ${ZeroCIce_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ../ESenseInterfaces/
  /usr/share/Ice-3.5.1/slice/
  )

FOREACH(currentSourceFile ${ICE_FILES})
        string(REGEX REPLACE ".*/(.*)" "\\1" new_source ${currentSourceFile})
        string(REGEX REPLACE ".*/(.*).ice" "\\1.cpp" new_source1 ${currentSourceFile})
        string(REGEX REPLACE ".*/(.*).ice" "\\1.h" new_source2 ${currentSourceFile})
        string(REGEX REPLACE ".*/(.*).ice" "\\1" new_source_name ${currentSourceFile})
        LIST(APPEND SOURCE_FILES ../ESenseInterfaces/${new_source1})
        LIST(APPEND SOURCE_FILES ../ESenseInterfaces/${new_source2})                
        
        LIST(APPEND SOURCE_FILES_2 ../ESenseInterfaces/${new_source1})
        LIST(APPEND SOURCE_FILES_2 ../ESenseInterfaces/${new_source2})
        
        #MESSAGE (STATUS "SOURCE: ${SOURCE_FILES_2}")
        add_custom_command(
        		  OUTPUT ${SOURCE_FILES_2}
                  COMMAND slice2cpp -I${CMAKE_CURRENT_SOURCE_DIR}/.. -I/usr/share/Ice-3.5.1/slice/ ${CMAKE_CURRENT_SOURCE_DIR}/${new_source} --output-dir ../ESenseInterfaces/
        )        
                               
        LIST(REMOVE_AT SOURCE_FILES_2 0)
        LIST(REMOVE_AT SOURCE_FILES_2 0)
        
#        execute_process(COMMAND slice2cpp ${ICE_FILES} 
#        				WORKING_DIRECTORY ../ESenseInterfaces/       
#        				     )

ENDFOREACH(currentSourceFile)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/../ESenseInterfaces/)

add_library (ESenseInterfaces SHARED ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(ESenseInterfaces
        ${ZeroCIce_LIBRARIES}
)

SET_PROPERTY(TARGET ESenseInterfaces PROPERTY SOVERSION 0.1)
