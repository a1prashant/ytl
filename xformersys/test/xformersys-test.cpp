#include <gtest/gtest.h>

TEST (Xformersys, Construction) {
    instance.start();
    instance.runTillSourceDataConsumed();
    auto userSysStatus = instance.userSys.status();
    EXPECT_EQ( userSysStatus.xformedData.size() == instance.sourceSys.sourceData.size() );
}

