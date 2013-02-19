execute_process(COMMAND ${TESTER}
                        ${TESTDIR}/${SUITE}.inp
                        ${TESTDIR}/${SUITE}.fmt
                        OUTPUT_FILE ${TESTDIR}/${SUITE}.out)

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
                        ${TESTDIR}/${SUITE}.ans
                        ${TESTDIR}/${SUITE}.out
                        RESULT_VARIABLE TEST_FAILED)

if(${TEST_FAILED} EQUAL 1)
    # CMake cannot return specified exit code from script,
    # so use FATAL_ERROR for returning non-zero code.
    message(FATAL_ERROR "Validator test '${SUITE}' is failed!")
endif()
