# Socket library BUILD file
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sock",
    srcs = glob(
        ["*.cc"],
        exclude = ["*_test.cc", "rpc_server.cc", "rpc_client.cc"],
    ),
    hdrs = glob(["*.h"]),
    deps = [
        "//base",
    ],
)

cc_library(
    name = "rpc",
    srcs = ["rpc_server.cc", "rpc_client.cc", "rpc_proto.cc"],
    hdrs = ["rpc_server.h", "rpc_client.h", "rpc_proto.h"],
    deps = [
        ":sock",
        "//base",
        "//cs:cs_server",
    ],
)
