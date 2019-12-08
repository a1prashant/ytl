cc_library(
        name = "libfieldmapper",
        srcs = glob(["src/*.cpp"]),
        hdrs = glob(["include/*.h"]),
        linkopts = ["-pthread"],
        visibility = ["//visibility:public"],
        )

cc_test(
        name = "fieldmapper-test",
        srcs = glob(["test/*.cpp", "test/*.h"]),
        copts = ["-Iexternal/gtest/include"],
        deps = [ 
        ":libfieldmapper",
        "@gtest//:main",
        ],
       )
