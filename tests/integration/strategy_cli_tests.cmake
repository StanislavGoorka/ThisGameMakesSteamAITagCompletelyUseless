cmake_minimum_required(VERSION 3.26)
# Literal trace of the wait-then-walk bot (BALANCE_EXPERIMENT_PLAN); seed 296
# completes sector 1 by walking to the exit. Exit code 0 also means the
# resolver accepted every bot command. No balance threshold here.
set(seeds 0 1 2 296 18446744073709551615)
set(expected "strategy 1 wait_then_walk
seed sectors end first_turn
0 0 overload 0
1 0 overload 0
2 0 overload 0
296 1 overload 24
18446744073709551615 0 overload 0
summary 5 1 0
")
execute_process(COMMAND "${STRATEGY_EXE}" ${seeds} RESULT_VARIABLE code OUTPUT_VARIABLE report)
if(NOT code STREQUAL "0" OR NOT report STREQUAL expected)
    message(FATAL_ERROR "Unexpected strategy trace (${code}):\n${report}")
endif()
execute_process(COMMAND "${STRATEGY_EXE}" ${seeds} RESULT_VARIABLE code OUTPUT_VARIABLE repeated)
if(NOT code STREQUAL "0" OR NOT repeated STREQUAL report)
    message(FATAL_ERROR "Strategy trace is not repeatable")
endif()
foreach(arguments IN ITEMS "" "abc" "-1" "1x" "18446744073709551616" "--corpus;5" "5;--corpus")
    execute_process(COMMAND "${STRATEGY_EXE}" ${arguments} RESULT_VARIABLE code OUTPUT_VARIABLE output)
    if(code STREQUAL "0" OR NOT output STREQUAL "")
        message(FATAL_ERROR "Invalid CLI input accepted: '${arguments}'")
    endif()
endforeach()
message(STATUS "PASS: repeatable wait-then-walk trace and rejected CLI input")
