// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include <til/bit.h>

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class BitTests
{
    TEST_CLASS(BitTests);

    TEST_METHOD(AsUnsignedPositiveValues)
    {
        Log::Comment(L"Positive signed integers should convert to the same unsigned value.");

        const int8_t i8 = 42;
        const auto u8 = til::as_unsigned(i8);
        static_assert(std::is_same_v<decltype(u8), const uint8_t>);
        VERIFY_ARE_EQUAL(static_cast<uint8_t>(42), u8);

        const int16_t i16 = 1000;
        const auto u16 = til::as_unsigned(i16);
        static_assert(std::is_same_v<decltype(u16), const uint16_t>);
        VERIFY_ARE_EQUAL(static_cast<uint16_t>(1000), u16);

        const int32_t i32 = 100000;
        const auto u32 = til::as_unsigned(i32);
        static_assert(std::is_same_v<decltype(u32), const uint32_t>);
        VERIFY_ARE_EQUAL(static_cast<uint32_t>(100000), u32);

        const int64_t i64 = 10000000000LL;
        const auto u64 = til::as_unsigned(i64);
        static_assert(std::is_same_v<decltype(u64), const uint64_t>);
        VERIFY_ARE_EQUAL(static_cast<uint64_t>(10000000000ULL), u64);
    }

    TEST_METHOD(AsUnsignedZero)
    {
        Log::Comment(L"Zero should convert to unsigned zero.");

        const int32_t zero = 0;
        const auto result = til::as_unsigned(zero);
        VERIFY_ARE_EQUAL(static_cast<uint32_t>(0), result);
    }

    TEST_METHOD(AsUnsignedNegativeOne)
    {
        Log::Comment(L"Negative one should convert to max unsigned value (no sign extension).");

        const int8_t neg8 = -1;
        const auto u8 = til::as_unsigned(neg8);
        VERIFY_ARE_EQUAL(static_cast<uint8_t>(0xFF), u8);

        const int16_t neg16 = -1;
        const auto u16 = til::as_unsigned(neg16);
        VERIFY_ARE_EQUAL(static_cast<uint16_t>(0xFFFF), u16);

        const int32_t neg32 = -1;
        const auto u32 = til::as_unsigned(neg32);
        VERIFY_ARE_EQUAL(static_cast<uint32_t>(0xFFFFFFFF), u32);

        const int64_t neg64 = -1;
        const auto u64 = til::as_unsigned(neg64);
        VERIFY_ARE_EQUAL(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), u64);
    }

    TEST_METHOD(AsUnsignedNegativeValues)
    {
        Log::Comment(L"Negative values should have bit-cast behavior without sign extension.");

        const int8_t minI8 = std::numeric_limits<int8_t>::min(); // -128
        const auto u8 = til::as_unsigned(minI8);
        VERIFY_ARE_EQUAL(static_cast<uint8_t>(0x80), u8);

        const int16_t minI16 = std::numeric_limits<int16_t>::min(); // -32768
        const auto u16 = til::as_unsigned(minI16);
        VERIFY_ARE_EQUAL(static_cast<uint16_t>(0x8000), u16);

        const int32_t minI32 = std::numeric_limits<int32_t>::min();
        const auto u32 = til::as_unsigned(minI32);
        VERIFY_ARE_EQUAL(static_cast<uint32_t>(0x80000000), u32);
    }

    TEST_METHOD(AsUnsignedMaxValues)
    {
        Log::Comment(L"Max positive signed values should convert correctly.");

        const int8_t maxI8 = std::numeric_limits<int8_t>::max(); // 127
        const auto u8 = til::as_unsigned(maxI8);
        VERIFY_ARE_EQUAL(static_cast<uint8_t>(0x7F), u8);

        const int16_t maxI16 = std::numeric_limits<int16_t>::max(); // 32767
        const auto u16 = til::as_unsigned(maxI16);
        VERIFY_ARE_EQUAL(static_cast<uint16_t>(0x7FFF), u16);

        const int32_t maxI32 = std::numeric_limits<int32_t>::max();
        const auto u32 = til::as_unsigned(maxI32);
        VERIFY_ARE_EQUAL(static_cast<uint32_t>(0x7FFFFFFF), u32);
    }
};
