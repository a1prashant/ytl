cc_library(
        name = "libfieldmapper",
        srcs = glob(["src/*.cpp"]),
        hdrs = glob(["include/*.h"]),
        linkopts = ["-pthread"],
        visibility = ["//visibility:public"],
# copts = ["-Iexternal/gtest/include"],
        )

cc_test(
        name = "fieldmapper-test",
        srcs = ["fieldmapper-test.cpp"],
        copts = ["-Iexternal/gtest/include"],
        deps = [ "@gtest//:main"
        , ":libfieldmapper"],
       )
