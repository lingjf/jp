CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(jp)

SET(CMAKE_BUILD_TYPE Release)

SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG .)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE .)

IF(NOT CMAKE_SYSTEM_NAME MATCHES "Windows")

        execute_process (
                COMMAND git show --format='%h' --no-patch #git rev-parse HEAD
                OUTPUT_VARIABLE BUILD_GIT_COMMIT
        )
 
        execute_process (
                COMMAND bash -c "date +'%F %T'"
                OUTPUT_VARIABLE BUILD_DATE_TIME
        )

        ADD_DEFINITIONS('-DBINFO_GIT_COMMIT="${BUILD_GIT_COMMIT}"')
        ADD_DEFINITIONS('-DBINFO_DATE_TIME="${BUILD_DATE_TIME}"')

ENDIF()

ADD_EXECUTABLE(jp main.cpp)
ADD_EXECUTABLE(jp. main.cpp)

TARGET_COMPILE_DEFINITIONS(jp. PUBLIC JPDOT)

INSTALL(TARGETS jp
        RUNTIME DESTINATION bin)

INSTALL(TARGETS jp.
        RUNTIME DESTINATION bin)

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
        ADD_CUSTOM_TARGET(copy ALL
                COMMAND cp ./jp.exe 'C:/Windows/system32/'
                COMMAND cp ./jp..exe 'C:/Windows/system32/'
                DEPENDS jp jp.
        )
ENDIF()
