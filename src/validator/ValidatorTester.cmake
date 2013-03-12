execute_process(COMMAND ${TESTER} ${SUITE}.inp ${SUITE}.fmt
                WORKING_DIRECTORY ${TESTDIR}
                OUTPUT_FILE ${SUITE}.out)

configure_file(${TESTDIR}/${SUITE}.ans ${TESTDIR} NEWLINE_STYLE LF)
configure_file(${TESTDIR}/${SUITE}.out ${TESTDIR} NEWLINE_STYLE LF)

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${SUITE}.ans ${SUITE}.out
                WORKING_DIRECTORY ${TESTDIR}
                ERROR_VARIABLE STDERR
                RESULT_VARIABLE RETCODE)

if((${RETCODE} GREATER 0) OR (${STDERR}))
    # CMake cannot return specified exit code from script,
    # so use FATAL_ERROR for returning non-zero code.
    message(FATAL_ERROR "Validator test '${SUITE}' is failed!")
endif()
