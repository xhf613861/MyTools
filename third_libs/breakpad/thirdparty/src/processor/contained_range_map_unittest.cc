// Copyright 2006 Google LLC
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google LLC nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// contained_range_map_unittest.cc: Unit tests for ContainedRangeMap
//
// Author: Mark Mentovai

#ifdef HAVE_CONFIG_H
#include <config.h>  // Must come first
#endif

#include <stdio.h>

#include "processor/contained_range_map-inl.h"

#include "processor/logging.h"


#define ASSERT_TRUE(condition) \
  if (!(condition)) { \
    fprintf(stderr, "FAIL: %s @ %s:%d\n", #condition, __FILE__, __LINE__); \
    return false; \
  }

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))


namespace {


using google_breakpad::ContainedRangeMap;
// The first is the querying address, the second is the entries vector result.
using EntriesTestPair = std::pair<unsigned, std::vector<int>>;
using EntriesTestPairVec = std::vector<EntriesTestPair>;

static bool RunTestsWithRetrieveRange(
    const ContainedRangeMap<unsigned int, int>& crm,
    const int* test_data,
    unsigned int test_length) {
  // Now, do the RetrieveRange tests.  This further validates that the
  // objects were stored properly and that retrieval returns the correct
  // object.
  // If GENERATE_TEST_DATA is defined, instead of the retrieval tests, a
  // new test_data array will be printed.  Exercise caution when doing this.
  // Be sure to verify the results manually!
#ifdef GENERATE_TEST_DATA
  printf("  const int test_data[] = {\n");
#endif  // GENERATE_TEST_DATA

  for (unsigned int address = 0; address < test_length; ++address) {
    int value;
    if (!crm.RetrieveRange(address, &value))
      value = 0;

#ifndef GENERATE_TEST_DATA
    // Don't use ASSERT inside the loop because it won't show the failed
    // |address|, and the line number will always be the same.  That makes
    // it difficult to figure out which test failed.
    if (value != test_data[address]) {
      fprintf(stderr, "FAIL: retrieve %d expected %d observed %d @ %s:%d\n",
              address, test_data[address], value, __FILE__, __LINE__);
      return false;
    }
#else   // !GENERATE_TEST_DATA
    printf("    %d%c%s  // %d\n", value, address == test_high - 1 ? ' ' : ',',
           value < 10 ? " " : "", address);
#endif  // !GENERATE_TEST_DATA
  }

#ifdef GENERATE_TEST_DATA
  printf("  };\n");
#endif  // GENERATE_TEST_DATA

  return true;
}

static bool RunTestsWithRetrieveRangeVector(
    const ContainedRangeMap<unsigned int, int>& crm,
    const EntriesTestPairVec& entries_tests) {
  for (const EntriesTestPair& entries_test : entries_tests) {
    std::vector<const int*> entries;
    crm.RetrieveRanges(entries_test.first, entries);
    if (entries.size() != entries_test.second.size()) {
      fprintf(stderr,
              "FAIL: retrieving entries at address %u has size %zu "
              "expected to have size %zu "
              "@ %s: %d\n",
              entries_test.first, entries.size(), entries_test.second.size(),
              __FILE__, __LINE__);
      return false;
    }
    for (size_t i = 0; i < entries.size(); ++i) {
      if (*entries[i] != entries_test.second[i]) {
        fprintf(stderr,
                "FAIL: retrieving entries at address %u entries[%zu] is %d "
                "expected %d"
                "@ %s: %d\n",
                entries_test.first, i, *entries[i], entries_test.second[i],
                __FILE__, __LINE__);
        return false;
      }
    }
  }
  return true;
}

static bool RunTestsWithNoEqualRange() {
  ContainedRangeMap<unsigned int, int> crm;

  // First, do the StoreRange tests.  This validates the containment
  // rules.
  ASSERT_TRUE (crm.StoreRange(10, 10,  1));
  ASSERT_FALSE(crm.StoreRange(10, 10,  2));  // exactly equal to 1
  ASSERT_FALSE(crm.StoreRange(11, 10,  3));  // begins inside 1 and extends up
  ASSERT_FALSE(crm.StoreRange( 9, 10,  4));  // begins below 1 and ends inside
  ASSERT_TRUE (crm.StoreRange(11,  9,  5));  // contained by existing
  ASSERT_TRUE (crm.StoreRange(12,  7,  6));
  ASSERT_TRUE (crm.StoreRange( 9, 12,  7));  // contains existing
  ASSERT_TRUE (crm.StoreRange( 9, 13,  8));
  ASSERT_TRUE (crm.StoreRange( 8, 14,  9));
  ASSERT_TRUE (crm.StoreRange(30,  3, 10));
  ASSERT_TRUE (crm.StoreRange(33,  3, 11));
  ASSERT_TRUE (crm.StoreRange(30,  6, 12));  // storable but totally masked
  ASSERT_TRUE (crm.StoreRange(40,  8, 13));  // will be totally masked
  ASSERT_TRUE (crm.StoreRange(40,  4, 14));
  ASSERT_TRUE (crm.StoreRange(44,  4, 15));
  ASSERT_FALSE(crm.StoreRange(32, 10, 16));  // begins in #10, ends in #14
  ASSERT_FALSE(crm.StoreRange(50,  0, 17));  // zero length
  ASSERT_TRUE (crm.StoreRange(50, 10, 18));
  ASSERT_TRUE (crm.StoreRange(50,  1, 19));
  ASSERT_TRUE (crm.StoreRange(59,  1, 20));
  ASSERT_TRUE (crm.StoreRange(60,  1, 21));
  ASSERT_TRUE (crm.StoreRange(69,  1, 22));
  ASSERT_TRUE (crm.StoreRange(60, 10, 23));
  ASSERT_TRUE (crm.StoreRange(68,  1, 24));
  ASSERT_TRUE (crm.StoreRange(61,  1, 25));
  ASSERT_TRUE (crm.StoreRange(61,  8, 26));
  ASSERT_FALSE(crm.StoreRange(59,  9, 27));
  ASSERT_FALSE(crm.StoreRange(59, 10, 28));
  ASSERT_FALSE(crm.StoreRange(59, 11, 29));
  ASSERT_TRUE (crm.StoreRange(70, 10, 30));
  ASSERT_TRUE (crm.StoreRange(74,  2, 31));
  ASSERT_TRUE (crm.StoreRange(77,  2, 32));
  ASSERT_FALSE(crm.StoreRange(72,  6, 33));
  ASSERT_TRUE (crm.StoreRange(80,  3, 34));
  ASSERT_TRUE (crm.StoreRange(81,  1, 35));
  ASSERT_TRUE (crm.StoreRange(82,  1, 36));
  ASSERT_TRUE (crm.StoreRange(83,  3, 37));
  ASSERT_TRUE (crm.StoreRange(84,  1, 38));
  ASSERT_TRUE (crm.StoreRange(83,  1, 39));
  ASSERT_TRUE (crm.StoreRange(86,  5, 40));
  ASSERT_TRUE (crm.StoreRange(88,  1, 41));
  ASSERT_TRUE (crm.StoreRange(90,  1, 42));
  ASSERT_TRUE (crm.StoreRange(86,  1, 43));
  ASSERT_TRUE (crm.StoreRange(87,  1, 44));
  ASSERT_TRUE (crm.StoreRange(89,  1, 45));
  ASSERT_TRUE (crm.StoreRange(87,  4, 46));
  ASSERT_TRUE (crm.StoreRange(87,  3, 47));
  ASSERT_FALSE(crm.StoreRange(86,  2, 48));

  // Each element in test_data contains the expected result when calling
  // RetrieveRange on an address.
  const int test_data[] = {
    0,   // 0
    0,   // 1
    0,   // 2
    0,   // 3
    0,   // 4
    0,   // 5
    0,   // 6
    0,   // 7
    9,   // 8
    7,   // 9
    1,   // 10
    5,   // 11
    6,   // 12
    6,   // 13
    6,   // 14
    6,   // 15
    6,   // 16
    6,   // 17
    6,   // 18
    5,   // 19
    7,   // 20
    8,   // 21
    0,   // 22
    0,   // 23
    0,   // 24
    0,   // 25
    0,   // 26
    0,   // 27
    0,   // 28
    0,   // 29
    10,  // 30
    10,  // 31
    10,  // 32
    11,  // 33
    11,  // 34
    11,  // 35
    0,   // 36
    0,   // 37
    0,   // 38
    0,   // 39
    14,  // 40
    14,  // 41
    14,  // 42
    14,  // 43
    15,  // 44
    15,  // 45
    15,  // 46
    15,  // 47
    0,   // 48
    0,   // 49
    19,  // 50
    18,  // 51
    18,  // 52
    18,  // 53
    18,  // 54
    18,  // 55
    18,  // 56
    18,  // 57
    18,  // 58
    20,  // 59
    21,  // 60
    25,  // 61
    26,  // 62
    26,  // 63
    26,  // 64
    26,  // 65
    26,  // 66
    26,  // 67
    24,  // 68
    22,  // 69
    30,  // 70
    30,  // 71
    30,  // 72
    30,  // 73
    31,  // 74
    31,  // 75
    30,  // 76
    32,  // 77
    32,  // 78
    30,  // 79
    34,  // 80
    35,  // 81
    36,  // 82
    39,  // 83
    38,  // 84
    37,  // 85
    43,  // 86
    44,  // 87
    41,  // 88
    45,  // 89
    42,  // 90
    0,   // 91
    0,   // 92
    0,   // 93
    0,   // 94
    0,   // 95
    0,   // 96
    0,   // 97
    0,   // 98
    0    // 99
  };
  unsigned int test_length = sizeof(test_data) / sizeof(int);
  return RunTestsWithRetrieveRange(crm, test_data, test_length);
}

static bool RunTestsWithEqualRange() {
  ContainedRangeMap<unsigned int, int> crm(true);

  // First, do the StoreRange tests.  This validates the containment
  // rules.
  ASSERT_TRUE (crm.StoreRange(1,  3,  1));
  ASSERT_TRUE (crm.StoreRange(1,  3,  2));  // exactly equal to 1
  ASSERT_TRUE (crm.StoreRange(1,  3,  3));  // exactly equal to 1, 2
  ASSERT_TRUE (crm.StoreRange(1,  3,  4));  // exactly equal to 1, 2, 3
  ASSERT_FALSE(crm.StoreRange(0,  3,  5));  // partial overlap.
  ASSERT_FALSE(crm.StoreRange(2,  3,  6));  // partial overlap.

  ASSERT_TRUE (crm.StoreRange(5,  3,  7));
  ASSERT_TRUE (crm.StoreRange(5,  3,  8));  // exactly equal to 7
  ASSERT_TRUE (crm.StoreRange(5,  3,  9));  // exactly equal to 7, 8
  ASSERT_TRUE (crm.StoreRange(5,  4, 10));  // encompasses 7, 8, 9
  ASSERT_TRUE (crm.StoreRange(5,  5, 11));  // encompasses 7, 8, 9, 10

  ASSERT_TRUE (crm.StoreRange(10, 3, 12));
  ASSERT_TRUE (crm.StoreRange(10, 3, 13));  // exactly equal to 12
  ASSERT_TRUE (crm.StoreRange(11, 2, 14));  // encompasses by 12
  ASSERT_TRUE (crm.StoreRange(11, 1, 15));  // encompasses by 12, 13

  ASSERT_TRUE (crm.StoreRange(14, 3, 16));
  ASSERT_TRUE (crm.StoreRange(14, 3, 17));  // exactly equal to 14
  ASSERT_TRUE (crm.StoreRange(14, 1, 18));  // encompasses by 14, 15
  ASSERT_TRUE (crm.StoreRange(14, 2, 19));  // encompasses by 14, 15 and encompasses 16
  ASSERT_TRUE (crm.StoreRange(14, 1, 20));  // exactly equal to 18
  ASSERT_TRUE (crm.StoreRange(14, 2, 21));  // exactly equal to 19

  // Each element in test_data contains the expected result when calling
  // RetrieveRange on an address.
  const int test_data[] = {
      0,   // 0
      4,   // 1
      4,   // 2
      4,   // 3
      0,   // 4
      9,   // 5
      9,   // 6
      9,   // 7
      10,  // 8
      11,   // 9
      13,  // 10
      15,  // 11
      14,  // 12
      0,   // 13
      20,  // 14
      21,  // 15
      17,  // 16
      0,   // 17
  };
  unsigned int test_length = sizeof(test_data) / sizeof(int);
  EntriesTestPairVec entries_tests = {
      {0,  {}},
      {1,  {4, 3, 2, 1}},
      {2,  {4, 3, 2, 1}},
      {3,  {4, 3, 2, 1}},
      {4,  {}},
      {5,  {9, 8, 7, 10, 11}},
      {6,  {9, 8, 7, 10, 11}},
      {7,  {9, 8, 7, 10, 11}},
      {8,  {10, 11}},
      {9,  {11}},
      {10, {13, 12}},
      {11, {15, 14, 13, 12}},
      {12, {14, 13, 12}},
      {13, {}},
      {14, {20, 18, 21, 19, 17, 16}},
      {15, {21, 19, 17, 16}},
      {16, {17, 16}},
      {17, {}},
  };
  return RunTestsWithRetrieveRange(crm, test_data, test_length) &&
         RunTestsWithRetrieveRangeVector(crm, entries_tests);
}

static bool RunTests() {
  return RunTestsWithNoEqualRange() && RunTestsWithEqualRange();
}


}  // namespace


int main(int argc, char** argv) {
  BPLOG_INIT(&argc, &argv);

  return RunTests() ? 0 : 1;
}
