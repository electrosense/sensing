SET ( CPU_SOURCE_FILES ControllerIQ.cpp )


add_executable (ControllerIQ ${CPU_SOURCE_FILES})

TARGET_LINK_LIBRARIES(ControllerIQ
        ${CMAKE_CURRENT_SOURCE_DIR}/../ESenseInterfaces/libESenseInterfaces.so
        ${ZeroCIce_LIBRARIES}
	${Boost_LIBRARIES}
        pthread
        rt
)
