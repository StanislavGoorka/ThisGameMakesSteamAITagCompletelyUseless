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

expect_success(empty_zero "0 7229561696845526205\n" 1 0)
expect_success(empty_max "0 18070937588695573379\n" 1 18446744073709551615)
expect_success(leading_zeroes "0 7229561696845526205\n1 0 0 7229561696845526205\n" 0001 0000 move:048)
expect_success(legal_rotation "0 7229561696845526205\n1 1 0 1475961550550524851\n" 1 0 rotate:1)
set(corpus 1 18446744073709551615 move:48 rotate:0 wait wait wait move:1 move:8 move:1 rotate:1 wait)
set(expected "0 18070937588695573379\n1 0 0 18070937588695573379\n2 0 0 18070937588695573379\n3 1 0 1475961550550524851\n4 1 0 12958317580500620444\n5 1 0 17475587385127207568\n6 1 0 10204055215828761158\n7 1 1 12376011081723499050\n8 0 1 12376011081723499050\n9 0 1 12376011081723499050\n10 0 1 12376011081723499050\n")
expect_success(corpus_first "${expected}" ${corpus})
expect_success(corpus_repeat "${expected}" ${corpus})

expect_error(missing_version)
expect_error(missing_seed 1)
foreach(version IN ITEMS 0 2 4294967295 4294967296 1x -1 +1 " 1" "1 ")
    expect_error("version_${version}" "${version}" 0)
endforeach()
foreach(seed IN ITEMS 18446744073709551616 0x -1 +1 " 0" "0 ")
    expect_error("seed_${seed}" 1 "${seed}")
endforeach()
foreach(token IN ITEMS unknown wait:0 move move=0 rotate rotate=0 move: rotate: move:0x rotate:1x move:-1 rotate:-1 move:+1 rotate:+1 "move: 0" "rotate:1 " move:49 rotate:49 move:18446744073709551616 rotate:18446744073709551616)
    # The earlier valid command must not produce even an initial trace.
    expect_error("token_${token}" 1 0 wait "${token}")
endforeach()

message(STATUS "PASS: actual replay CLI exact traces, repeatability and argument failures")
