# RPC Demo BUILD file
# Multi-server demo with client and multiple server targets
package(default_visibility = ["//visibility:public"])

# Client demo binary
cc_binary(
    name = "rpc_client",
    srcs = ["rpc_client_demo.cc"],
    data = [":conf"],
    deps = [
        "//base",
        "//sock:rpc",
        "//sock:sock",
    ],
)

# Server demo binaries - Control server
cc_binary(
    name = "rpc_server_control",
    srcs = ["rpc_server_control_demo.cc"],
    data = [
        "conf/control_server.conf",
        "conf/control_user_info.conf",
    ],
    deps = [
        "//base",
        "//sock:rpc",
        "//sock:sock",
    ],
)

# Server demo binaries - ACL DAO server
cc_binary(
    name = "rpc_server_acl_dao",
    srcs = ["rpc_server_acl_dao_demo.cc"],
    data = [
        "conf/acl_server.conf",
        "conf/acl_user_info.conf",
    ],
    deps = [
        "//base",
        "//sock:rpc",
        "//sock:sock",
    ],
)

# Server demo binaries - Study DAO server
cc_binary(
    name = "rpc_server_study_dao",
    srcs = ["rpc_server_study_dao_demo.cc"],
    data = [
        "conf/study_server.conf",
        "conf/study_user_info.conf",
    ],
    deps = [
        "//base",
        "//sock:rpc",
        "//sock:sock",
    ],
)

# Filegroup for all configuration files
filegroup(
    name = "conf",
    srcs = glob(["conf/*.conf"]),
)

# Filegroup for all client demos
filegroup(
    name = "client_demos",
    srcs = [":rpc_client"],
)

# Filegroup for all server demos
filegroup(
    name = "server_demos",
    srcs = [
        ":rpc_server_control",
        ":rpc_server_acl_dao",
        ":rpc_server_study_dao",
    ],
)

# Build all demos
filegroup(
    name = "all_demos",
    srcs = [
        ":client_demos",
        ":server_demos",
    ],
)
