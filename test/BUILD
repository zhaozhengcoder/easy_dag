package(default_visibility = ["//visibility:public"])

COMMON_CXX_FLAGS = [
    "-std=c++11",
    "-Wall",
    "-Werror",
    "-Werror=return-type",
]

COMMON_LINKOPTS = [
    "-lpthread",
]

SRCS = ["test_main.cpp"]

DEPS = [
    "//src:easy_dag",
]

MACOS_SPECIFIC_LINKOPTS = [
    "-fuse-ld=lld", 
    "-debugger-tuning=lldb",
]

cc_binary(
    name = "dag_test_release",
    srcs = SRCS,
    copts = COMMON_CXX_FLAGS + [
        "-O3", 
        "-DNDEBUG", 
    ],
    linkopts = select({
        ":macos": COMMON_LINKOPTS + MACOS_SPECIFIC_LINKOPTS,
        "//conditions:default": COMMON_LINKOPTS,
    }),
    deps = DEPS,
)

cc_binary(
    name = "dag_test_debug",
    srcs = SRCS,
    copts = COMMON_CXX_FLAGS + [
        "-g", 
        "-O0",
    ],
    linkopts = select({
        ":macos": COMMON_LINKOPTS + MACOS_SPECIFIC_LINKOPTS,
        "//conditions:default": COMMON_LINKOPTS,
    }),
    deps = DEPS,
)

cc_binary(
    name = "dag_test_asan_linux",
    srcs = SRCS,
    copts = COMMON_CXX_FLAGS + [
        "-fsanitize=address",
        "-g", 
    ],
    linkopts = [
        "-lpthread",
        "-lasan",
    ],
    deps = DEPS,
)

config_setting(
    name = "macos",
    define_values = {
        "platform": "macos",
    },
)

config_setting(
    name = "linux",
    define_values = {
        "platform": "linux",
    },
)