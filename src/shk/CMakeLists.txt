add_custom_command(
  OUTPUT lexer.cpp
  COMMAND re2c -b -i --no-generation-date -o lexer.cpp ${CMAKE_CURRENT_LIST_DIR}/src/manifest/lexer.in.cpp
  DEPENDS src/manifest/lexer.in.cpp re2c)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/manifest/manifest_generated.h
  COMMAND flatc --binary --cpp --scoped-enums -o include/manifest ${CMAKE_CURRENT_LIST_DIR}/src/manifest/manifest.fbs
  DEPENDS src/manifest/manifest.fbs flatc)

shk_library(
  NAME shklib
  SOURCES
    src/build.h
    src/build.cpp
    src/build_config.h
    src/build_error.h
    src/clock.h
    src/cache/cache_lookup_result.cpp
    src/cache/cache_lookup_result.h
    src/cache/file_hasher.cpp
    src/cache/file_hasher.h
    src/cache/interner.h
    src/cmd/command_runner.h
    src/cmd/dry_run_command_runner.cpp
    src/cmd/dry_run_command_runner.h
    src/cmd/limited_command_runner.cpp
    src/cmd/limited_command_runner.h
    src/cmd/pooled_command_runner.cpp
    src/cmd/pooled_command_runner.h
    src/cmd/real_command_runner.h
    src/cmd/real_command_runner_posix.cpp
    src/cmd/trace_server_handle.cpp
    src/cmd/trace_server_handle.h
    src/cmd/tracing_command_runner.cpp
    src/cmd/tracing_command_runner.h
    src/edit_distance.cpp
    src/edit_distance.h
    src/exit_status.h
    src/fs/cleaning_file_system.cpp
    src/fs/cleaning_file_system.h
    src/fs/dry_run_file_system.cpp
    src/fs/dry_run_file_system.h
    src/fs/file_id.h
    src/fs/file_lock.cpp
    src/fs/file_lock.h
    src/fs/file_system.cpp
    src/fs/file_system.h
    src/fs/fingerprint.cpp
    src/fs/fingerprint.h
    src/fs/path.cpp
    src/fs/path.h
    src/fs/persistent_file_system.cpp
    src/fs/persistent_file_system.h
    src/manifest/eval_env.cpp
    src/manifest/eval_env.h
    src/manifest/eval_string.cpp
    src/manifest/eval_string.h
    src/manifest/compiled_manifest.cpp
    src/manifest/compiled_manifest.h
    src/manifest/lexer.h
    src/manifest/raw_manifest.cpp
    src/manifest/raw_manifest.h
    src/manifest/raw_step.cpp
    src/manifest/raw_step.h
    src/manifest/rule.cpp
    src/manifest/rule.h
    src/manifest/step.h
    src/manifest/wrapper_view.h
    src/io_error.cpp
    src/io_error.h
    src/log/delayed_invocation_log.cpp
    src/log/delayed_invocation_log.h
    src/log/dummy_invocation_log.h
    src/log/invocation_log.cpp
    src/log/invocation_log.h
    src/log/invocations.cpp
    src/log/invocations.h
    src/log/persistent_invocation_log.cpp
    src/log/persistent_invocation_log.h
    src/nullterminated_string.h
    src/parse_error.h
    src/status/build_status.h
    src/status/dummy_build_status.h
    src/status/line_printer.cpp
    src/status/line_printer.h
    src/status/stopwatch.h
    src/status/terminal_build_status.cpp
    src/status/terminal_build_status.h
    src/tools/clean.cpp
    src/tools/clean.h
    src/tools/compilation_database.cpp
    src/tools/compilation_database.h
    src/tools/deps.cpp
    src/tools/deps.h
    src/tools/query.cpp
    src/tools/query.h
    src/tools/recompact.cpp
    src/tools/recompact.h
    src/tools/targets.cpp
    src/tools/targets.h
    src/tools/tool_params.h
    src/util.cpp
    src/util.h
    src/version.cpp
    src/version.h
    lexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/manifest/manifest_generated.h
  DEPENDENCIES
    blake2 rs shk-util
  PUBLIC_INCLUDES
    src ${CMAKE_CURRENT_BINARY_DIR}/include)

shk_executable(
  NAME shk
  SOURCES
    src/shk.cpp
  DEPENDENCIES
    shklib
  USER_FACING)

shk_executable(
  NAME shk-trace-dummy
  SOURCES
    test/cmd/shk_trace_dummy.cpp)

shk_test(
  NAME shk-test
  SOURCES
    test/build_test.cpp
    test/cache/cache_lookup_result_test.cpp
    test/cache/file_hasher_test.cpp
    test/cache/interner_test.cpp
    test/cmd/dry_run_command_runner_test.cpp
    test/cmd/limited_command_runner_test.cpp
    test/cmd/pooled_command_runner_test.cpp
    test/cmd/real_command_runner_test.cpp
    test/cmd/tracing_command_runner_test.cpp
    test/cmd/trace_server_handle_test.cpp
    test/dummy_command_runner.cpp
    test/dummy_command_runner.h
    test/dummy_command_runner_test.cpp
    test/edit_distance_test.cpp
    test/fs/cleaning_file_system_test.cpp
    test/fs/dry_run_file_system_test.cpp
    test/fs/file_id_test.cpp
    test/fs/file_lock_test.cpp
    test/fs/file_system_test.cpp
    test/fs/fingerprint_test.cpp
    test/fs/path_test.cpp
    test/fs/persistent_file_system_test.cpp
    test/in_memory_file_system.cpp
    test/in_memory_file_system.h
    test/in_memory_file_system_test.cpp
    test/in_memory_invocation_log.cpp
    test/in_memory_invocation_log.h
    test/in_memory_invocation_log_test.cpp
    test/io_error_test.cpp
    test/log/delayed_invocation_log_test.cpp
    test/log/invocation_log_test.cpp
    test/log/invocations_test.cpp
    test/log/persistent_invocation_log_test.cpp
    test/main.cpp
    test/manifest/compiled_manifest_test.cpp
    test/manifest/lexer_test.cpp
    test/manifest/raw_manifest_test.cpp
    test/manifest/raw_step_test.cpp
    test/manifest/step_builder.cpp
    test/manifest/step_builder.h
    test/manifest/step_test.cpp
    test/manifest/wrapper_view_test.cpp
    test/nullterminated_string_test.cpp
    test/status/line_printer_test.cpp
    test/status/stopwatch_test.cpp
    test/util_test.cpp
  DEPENDENCIES
    shklib catch)
add_dependencies(shk-test shk-trace-dummy)

add_test(
  NAME shk-integration
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/integrationtest
  COMMAND env SHK_PATH=${PROJECT_BINARY_DIR}/bin/shk python ${CMAKE_CURRENT_LIST_DIR}/integrationtest/test.py)
