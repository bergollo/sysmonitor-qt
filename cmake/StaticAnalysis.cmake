option(SYSMONITOR_ENABLE_CLANG_TIDY
       "Run clang-tidy while compiling project targets" OFF)
option(SYSMONITOR_CLANG_TIDY_STRICT
       "Treat clang-tidy diagnostics as errors" OFF)

if(SYSMONITOR_ENABLE_CLANG_TIDY)
    find_program(SYSMONITOR_CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
    if(NOT SYSMONITOR_CLANG_TIDY_EXECUTABLE)
        message(FATAL_ERROR
                "SYSMONITOR_ENABLE_CLANG_TIDY is ON, but clang-tidy was not found")
    endif()
endif()

function(enable_project_clang_tidy target)
    if(NOT SYSMONITOR_ENABLE_CLANG_TIDY)
        return()
    endif()

    set(command "${SYSMONITOR_CLANG_TIDY_EXECUTABLE}")
    if(SYSMONITOR_CLANG_TIDY_STRICT)
        list(APPEND command "--warnings-as-errors=*")
    endif()

    # Keep analysis opt-in and target-scoped so fetched GoogleTest sources and
    # other third-party targets are not analyzed by the project configuration.
    set_property(TARGET "${target}" PROPERTY CXX_CLANG_TIDY "${command}")
endfunction()
