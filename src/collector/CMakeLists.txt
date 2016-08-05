SET ( CPU_SOURCE_FILES CollectorIQ.cpp )

INCLUDE_DIRECTORIES (../ESenseInterfaces/)

add_executable (CollectorIQ ${CPU_SOURCE_FILES})

TARGET_LINK_LIBRARIES(CollectorIQ
        ${CMAKE_CURRENT_SOURCE_DIR}/../ESenseInterfaces/libESenseInterfaces.so
        ${ZeroCIce_LIBRARIES}
        pthread
        rt
)
