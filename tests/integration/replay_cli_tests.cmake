cmake_minimum_required(VERSION 3.26)

if(NOT DEFINED REPLAY_EXE OR NOT EXISTS "${REPLAY_EXE}")
    message(FATAL_ERROR "Replay executable is required")
endif()

function(expect_success name expected)
    execute_process(COMMAND "${REPLAY_EXE}" ${ARGN}
        RESULT_VARIABLE code OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic)
    if(NOT code STREQUAL "0" OR NOT output STREQUAL expected)
        message(FATAL_ERROR "${name}: expected exact successful trace; code=${code}; output=${output}; stderr=${diagnostic}")
    endif()
endfunction()

function(expect_error name)
    execute_process(COMMAND "${REPLAY_EXE}" ${ARGN}
        RESULT_VARIABLE code OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic)
    if(code STREQUAL "0" OR NOT output STREQUAL "")
        message(FATAL_ERROR "${name}: expected failure with empty stdout; code=${code}; output=${output}")
    endif()
endfunction()

expect_success(empty_zero "0 2662686785220386338\n" 4 0)
expect_success(empty_max "0 15288789527669922352\n" 4 18446744073709551615)
expect_success(leading_zeroes "0 2662686785220386338\n1 0 0 2662686785220386338\n" 0004 0000 move:048)
expect_success(legal_rotation "0 2662686785220386338\n1 1 0 3753952707039261317\n" 4 0 rotate:1)
set(corpus 4 18446744073709551615 move:48 rotate:0 wait wait wait move:1 move:8 move:1 rotate:1 wait)
set(expected "0 15288789527669922352\n1 0 0 15288789527669922352\n2 0 0 15288789527669922352\n3 1 0 3753952707039261317\n4 1 0 13512653858609942828\n5 1 0 9250046268769112888\n6 1 0 1913312357299138638\n7 1 1 16751804550216351426\n8 0 1 16751804550216351426\n9 0 1 16751804550216351426\n10 0 1 16751804550216351426\n")
expect_success(corpus_first "${expected}" ${corpus})
expect_success(corpus_repeat "${expected}" ${corpus})

expect_error(missing_version)
expect_error(missing_seed 4)
foreach(version IN ITEMS 0 1 2 3 5 4294967295 4294967296 1x -1 +1 " 1" "1 ")
    expect_error("version_${version}" "${version}" 0)
endforeach()
foreach(seed IN ITEMS 18446744073709551616 0x -1 +1 " 0" "0 ")
    expect_error("seed_${seed}" 4 "${seed}")
endforeach()
foreach(token IN ITEMS unknown wait:0 move move=0 rotate rotate=0 move: rotate: move:0x rotate:1x move:-1 rotate:-1 move:+1 rotate:+1 "move: 0" "rotate:1 " move:49 rotate:49 move:18446744073709551616 rotate:18446744073709551616)
    # The earlier valid command must not produce even an initial trace.
    expect_error("token_${token}" 4 0 wait "${token}")
endforeach()

message(STATUS "PASS: actual replay CLI exact traces, repeatability and argument failures")
