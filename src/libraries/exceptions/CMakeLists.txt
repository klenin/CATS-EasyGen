file(GLOB SOURCES *.c)

if(MSVC)
    # C6011: http://msdn.microsoft.com/en-us/library/2ayc37ac(v=vs.110).aspx
    # C28182: http://msdn.microsoft.com/en-us/library/hh329451.aspx
    set_source_files_properties(
        Exceptions.c
        PROPERTIES COMPILE_FLAGS "/wd 6011 /wd 28182")
endif()

add_library(exceptions STATIC ${SOURCES})
