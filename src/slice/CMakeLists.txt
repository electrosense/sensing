set( CMAKE_CXX_FLAGS "-lIce -lIceUtil -lIceGrid -lIceBox -lIceStorm -lIceStormService" ) # Opciones para el compilador
set( SLICE2PY_COMMAND /usr/bin/slice2py)

FILE(GLOB ICE_FILES "*.ice")

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ../ESenseInterfaces/
  )



FOREACH(currentSourceFile ${ICE_FILES})
        string(REGEX REPLACE ".*/(.*)" "\\1" new_source ${currentSourceFile})
        string(REGEX REPLACE ".*/(.*).ice" "\\1.cpp" new_source1 ${currentSourceFile})
        string(REGEX REPLACE ".*/(.*).ice" "\\1.h" new_source2 ${currentSourceFile})
        string(REGEX REPLACE ".*/(.*).ice" "\\1" new_source_name ${currentSourceFile})
        LIST(APPEND SOURCE_FILES ../ESenseInterfaces/${new_source1})
        LIST(APPEND SOURCE_FILES ../ESenseInterfaces/${new_source2})                
        
        
        add_custom_command(
        		  OUTPUT ${SOURCE_FILES}
                  COMMAND slice2cpp -I${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/${new_source} --output-dir ../ESenseInterfaces/
        )        
        
               
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
