/**
*  \file Buffer.test.cc
*  \copyright 2022 F.Lagarde
*/

#include "Buffer.h"

#include <doctest/doctest.h>
#include <iostream>

TEST_CASE("Buffer char[2]")
{
  unsigned char b[2] = {'a', 'b'};
  Buffer        buffer(b, 2);
  CHECK_EQ(buffer.size(), 2);
  CHECK_EQ(buffer.capacity(), 2);
}

TEST_CASE("Buffer char[]")
{
  char   b[] = {'a', 'b'};
  Buffer buffer(b, 2);
  CHECK_EQ(buffer.size(), 2);
  CHECK_EQ(buffer.capacity(), 2);
}

TEST_CASE("Buffer const unsigned char[]")
{
  const unsigned char b[] = {'a', 'b'};
  Buffer              buffer(b, 2);
  CHECK_EQ(buffer.size(), 2);
  CHECK_EQ(buffer.capacity(), 2);
}

TEST_CASE("Buffer const char[]")
{
  const char b[] = {'a', 'b'};
  Buffer     buffer(b, 2);
  CHECK_EQ(buffer.size(), 2);
  CHECK_EQ(buffer.capacity(), 2);
}

TEST_CASE("Buffer char*")
{
  char   b[] = {'a', 'b'};
  char*  kk  = &b[0];
  Buffer buffer(kk, 2);
  CHECK_EQ(buffer.size(), 2);
  CHECK_EQ(buffer.capacity(), 2);
}

TEST_CASE("Buffer std::vector<unsigned char>")
{
  std::vector<unsigned char> vec;
  vec.reserve(50);
  vec = {'a', 'b', 'c', 'd'};
  Buffer buffer(vec);
  CHECK_EQ(buffer.size(), 4);
  CHECK_EQ(buffer.capacity(), 50);
}

TEST_CASE("Buffer std::vector<std::uint64_t>")
{
  std::vector<std::uint64_t> vec;
  vec.reserve(50);
  vec = {0, 0, 0, 0};
  Buffer buffer(vec);
  CHECK_EQ(buffer.size(), 4 * sizeof(std::uint64_t));
  CHECK_EQ(buffer.capacity(), 50 * sizeof(std::uint64_t));
}

TEST_CASE("Buffer std::array<std::uint64_t,4>")
{
  std::array<std::uint64_t, 4> arra{0, 0, 0, 0};
  Buffer                       buffer(arra);
  CHECK_EQ(buffer.size(), 4 * sizeof(std::uint64_t));
  CHECK_EQ(buffer.capacity(), 4 * sizeof(std::uint64_t));
}
