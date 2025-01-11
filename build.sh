#!/bin/bash

# build debug
bazel build //test:dag_test_debug --strip=never --define platform=macos

# run
./bazel-bin/test/dag_test_debug

# cpplint ckeck
# todo

# memleak check 
# bazel build //test:dag_test_debug --strip=never --define platform=linux
# valgrind --leak-check=full ./bazel-bin/test/dag_test_debug 

# asan check
# bazel build //test:dag_test_asan_linux --strip=never --define platform=linux