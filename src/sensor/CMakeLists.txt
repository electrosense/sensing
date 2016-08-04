SET ( CPU_SOURCE_FILES SensorIQ.cpp SDR.h SDR.cpp )


add_executable (SensorIQ ${CPU_SOURCE_FILES})

INCLUDE_DIRECTORIES (../ESenseInterfaces/)

TARGET_LINK_LIBRARIES(SensorIQ
	${CMAKE_CURRENT_SOURCE_DIR}/../ESenseInterfaces/libESenseInterfaces.so
	${ZeroCIce_LIBRARIES}
	${LIBRTLSDR_LIBRARIES}
	pthread
	rt
	dl
)
