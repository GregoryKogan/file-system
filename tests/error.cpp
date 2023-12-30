#include "../src/FileSystem/Error/Error.hpp"
#include <expected>
#include <gtest/gtest.h>

TEST(Error, SingleError) {
  DEFINE_ERROR(Test);

  auto err = TestError("test");
  EXPECT_EQ(err.type(), "Test");
  EXPECT_EQ(err.what(), "Test: test");
  EXPECT_EQ(err.trace().size(), 1);
  EXPECT_EQ(err.trace()[0].type(), "Test");
}

TEST(Error, Wrap) {
  DEFINE_ERROR(Outer);
  DEFINE_ERROR(Inner);

  auto inner = InnerError("I'm an inner error");
  auto outer = OuterError("I'm an outer error");
  outer.wrap(inner);

  EXPECT_EQ(outer.type(), "Outer");
  EXPECT_EQ(outer.what(), "Outer: I'm an outer error");
  ASSERT_EQ(outer.trace().size(), 2);
  EXPECT_EQ(outer.trace()[0].type(), "Inner");
  EXPECT_EQ(outer.trace()[1].type(), "Outer");
}

TEST(Error, WrapConstructor) {
  DEFINE_ERROR(Outer);
  DEFINE_ERROR(Inner);

  auto inner = InnerError("I'm an inner error");
  auto outer = OuterError("I'm an outer error").wrap(inner);

  EXPECT_EQ(outer.type(), "Outer");
  EXPECT_EQ(outer.what(), "Outer: I'm an outer error");
  ASSERT_EQ(outer.trace().size(), 2);
  EXPECT_EQ(outer.trace()[0].type(), "Inner");
  EXPECT_EQ(outer.trace()[1].type(), "Outer");
}

TEST(Error, WrapMultiple) {
  DEFINE_ERROR(TopLayer);
  DEFINE_ERROR(MiddleLayer);
  DEFINE_ERROR(BottomLayer);

  auto bottom = BottomLayerError("I'm a bottom layer error");
  auto middle = MiddleLayerError("I'm a middle layer error").wrap(bottom);
  auto top = TopLayerError("I'm a top layer error").wrap(middle);

  EXPECT_EQ(top.type(), "TopLayer");
  EXPECT_EQ(top.what(), "TopLayer: I'm a top layer error");
  ASSERT_EQ(top.trace().size(), 3);
  EXPECT_EQ(top.trace()[0].type(), "BottomLayer");
  EXPECT_EQ(top.trace()[1].type(), "MiddleLayer");
  EXPECT_EQ(top.trace()[2].type(), "TopLayer");

  EXPECT_EQ(middle.type(), "MiddleLayer");
  EXPECT_EQ(middle.what(), "MiddleLayer: I'm a middle layer error");
  ASSERT_EQ(middle.trace().size(), 2);
  EXPECT_EQ(middle.trace()[0].type(), "BottomLayer");
  EXPECT_EQ(middle.trace()[1].type(), "MiddleLayer");

  EXPECT_EQ(bottom.type(), "BottomLayer");
  EXPECT_EQ(bottom.what(), "BottomLayer: I'm a bottom layer error");
  ASSERT_EQ(bottom.trace().size(), 1);
  EXPECT_EQ(bottom.trace()[0].type(), "BottomLayer");

  ASSERT_EQ(top.trace()[1].trace().size(), 2);
  EXPECT_EQ(top.trace()[1].trace()[0].type(), "BottomLayer");
  EXPECT_EQ(top.trace()[1].trace()[1].type(), "MiddleLayer");
}

TEST(Error, InsideExpected) {
  DEFINE_ERROR(Test);

  std::expected<int, Error> result = std::unexpected(TestError("test"));

  EXPECT_FALSE(result);
  EXPECT_EQ(result.error().type(), "Test");
  EXPECT_EQ(result.error().what(), "Test: test");
  ASSERT_EQ(result.error().trace().size(), 1);
  EXPECT_EQ(result.error().trace()[0].type(), "Test");
}

TEST(Error, InsideExpectedWrap) {
  DEFINE_ERROR(Outer);
  DEFINE_ERROR(Inner);

  std::expected<int, Error> inner_result = std::unexpected(InnerError("I'm an inner error"));
  std::expected<int, Error> outer_result = std::unexpected(OuterError("I'm an outer error").wrap(inner_result.error()));

  EXPECT_FALSE(outer_result);
  EXPECT_EQ(outer_result.error().type(), "Outer");
  EXPECT_EQ(outer_result.error().what(), "Outer: I'm an outer error");
  ASSERT_EQ(outer_result.error().trace().size(), 2);
  EXPECT_EQ(outer_result.error().trace()[0].type(), "Inner");
  EXPECT_EQ(outer_result.error().trace()[1].type(), "Outer");
}