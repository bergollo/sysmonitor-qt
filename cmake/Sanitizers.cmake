set(SYSMONITOR_SANITIZER "none" CACHE STRING
    "Sanitizer runtime: none, address, undefined, address,undefined, or thread")
set_property(CACHE SYSMONITOR_SANITIZER PROPERTY STRINGS
             none address undefined "address,undefined" thread)
option(SYSMONITOR_SANITIZER_RECOVER
       "Continue after sanitizer diagnostics where the runtime supports it" OFF)

set(_sysmonitor_supported_sanitizers none address undefined "address,undefined" thread)
if(NOT SYSMONITOR_SANITIZER IN_LIST _sysmonitor_supported_sanitizers)
    message(FATAL_ERROR
            "Unsupported SYSMONITOR_SANITIZER='${SYSMONITOR_SANITIZER}'. "
            "Choose none, address, undefined, address,undefined, or thread.")
endif()

if(NOT SYSMONITOR_SANITIZER STREQUAL "none")
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "Sanitizers require GCC or Clang on the native Linux build")
    endif()

    if(SYSMONITOR_SANITIZER STREQUAL "thread" AND
       SYSMONITOR_SANITIZER MATCHES "address|undefined")
        message(FATAL_ERROR "ThreadSanitizer cannot be combined with AddressSanitizer or UBSan")
    endif()
endif()

function(enable_project_sanitizer target)
    if(SYSMONITOR_SANITIZER STREQUAL "none")
        return()
    endif()

    target_compile_options("${target}" PRIVATE
        -fsanitize=${SYSMONITOR_SANITIZER}
        -fno-omit-frame-pointer
    )
    target_link_options("${target}" PRIVATE
        -fsanitize=${SYSMONITOR_SANITIZER}
        -fno-omit-frame-pointer
    )

    if(SYSMONITOR_SANITIZER MATCHES "undefined" AND NOT SYSMONITOR_SANITIZER_RECOVER)
        target_compile_options("${target}" PRIVATE -fno-sanitize-recover=undefined)
        target_link_options("${target}" PRIVATE -fno-sanitize-recover=undefined)
    endif()
endfunction()
