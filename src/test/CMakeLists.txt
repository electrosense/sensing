SET ( CPU_SOURCE_FILES AnalyzeTime.cpp )


add_executable (AnalyzeTime ${CPU_SOURCE_FILES})
SET_TARGET_PROPERTIES(AnalyzeTime PROPERTIES COMPILE_FLAGS "-std=gnu++11")

INCLUDE_DIRECTORIES (../ESenseInterfaces/)

TARGET_LINK_LIBRARIES(AnalyzeTime
	${CMAKE_CURRENT_SOURCE_DIR}/../ESenseInterfaces/libESenseInterfaces.so
	${ZeroCIce_LIBRARIES}
	${LIBRTLSDR_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
	pthread
	rt
	dl
)
