#pragma once
#include "gtest/gtest.h"
#include "include/fieldmapper.h"
#include "include/getter.h"

namespace ytl::fieldmapper {

class TestFixture : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

}
