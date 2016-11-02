SET ( CPU_SOURCE_FILES SensorIQ.cpp SDR.h SDR.cpp Sender.cpp Sender.h SpectrumSegment.cpp SpectrumSegment.h atomicops.h readerwriterqueue.h )


add_executable (SensorIQ ${CPU_SOURCE_FILES})
SET_TARGET_PROPERTIES(SensorIQ PROPERTIES COMPILE_FLAGS "-std=gnu++11")

INCLUDE_DIRECTORIES (../ESenseInterfaces/)

TARGET_LINK_LIBRARIES(SensorIQ
	${CMAKE_CURRENT_SOURCE_DIR}/../ESenseInterfaces/libESenseInterfaces.so
	${ZeroCIce_LIBRARIES}
	${LIBRTLSDR_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
	pthread
	rt
	dl
)
