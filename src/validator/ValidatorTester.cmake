execute_process(COMMAND ${TESTER} ${SUITE}.inp ${SUITE}.fmt
                WORKING_DIRECTORY ${TESTDIR}
                OUTPUT_FILE ${SUITE}.out)

execute_process(COMMAND diff -wB ${SUITE}.ans ${SUITE}.out
                WORKING_DIRECTORY ${TESTDIR}
                RESULT_VARIABLE DIFF_RESULT)

if(${DIFF_RESULT} EQUAL 1)
    # CMake cannot return specified exit code from script,
    # so use FATAL_ERROR for returning non-zero code.
    message(FATAL_ERROR "Validator test '${SUITE}' is failed!")
elseif(${DIFF_RESULT} EQUAL 2)
    message(FATAL_ERROR "Diff trouble on test '${SUITE}'")
endif()
