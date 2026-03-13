// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include <til/bytes.h>

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class BytesTests
{
    TEST_CLASS(BytesTests);

    TEST_METHOD(BytesAdvanceBasic)
    {
        Log::Comment(L"bytes_advance should advance the span by the given count.");

        char buffer[16] = {};
        std::span<char> span{ buffer };

        VERIFY_ARE_EQUAL(16u, span.size());

        til::bytes_advance(span, 4);

        VERIFY_ARE_EQUAL(12u, span.size());
        VERIFY_ARE_EQUAL(buffer + 4, span.data());
    }

    TEST_METHOD(BytesAdvanceToEnd)
    {
        Log::Comment(L"bytes_advance should handle advancing to the exact end of the span.");

        char buffer[8] = {};
        std::span<char> span{ buffer };

        til::bytes_advance(span, 8);

        VERIFY_ARE_EQUAL(0u, span.size());
    }

    TEST_METHOD(BytesAdvanceZero)
    {
        Log::Comment(L"bytes_advance with count 0 should be a no-op.");

        char buffer[8] = {};
        std::span<char> span{ buffer };

        til::bytes_advance(span, 0);

        VERIFY_ARE_EQUAL(8u, span.size());
        VERIFY_ARE_EQUAL(buffer, span.data());
    }

    TEST_METHOD(BytesAdvancePastEnd)
    {
        Log::Comment(L"bytes_advance past the end should throw std::length_error.");

        char buffer[4] = {};
        std::span<char> span{ buffer };

        auto threwException = false;
        try
        {
            til::bytes_advance(span, 5);
        }
        catch (const std::length_error&)
        {
            threwException = true;
        }

        VERIFY_IS_TRUE(threwException);
    }

    TEST_METHOD(BytesCanPutFits)
    {
        Log::Comment(L"bytes_can_put should return true when there is enough space.");

        char buffer[8] = {};
        std::span<char> span{ buffer };

        VERIFY_IS_TRUE(til::bytes_can_put<uint32_t>(span));
        VERIFY_IS_TRUE(til::bytes_can_put<uint64_t>(span));
        VERIFY_IS_TRUE(til::bytes_can_put<char>(span));
    }

    TEST_METHOD(BytesCanPutDoesNotFit)
    {
        Log::Comment(L"bytes_can_put should return false when there is not enough space.");

        char buffer[2] = {};
        std::span<char> span{ buffer };

        VERIFY_IS_FALSE(til::bytes_can_put<uint32_t>(span));
        VERIFY_IS_FALSE(til::bytes_can_put<uint64_t>(span));
    }

    TEST_METHOD(BytesCanPutExactFit)
    {
        Log::Comment(L"bytes_can_put should return true when the size is exactly right.");

        char buffer[4] = {};
        std::span<char> span{ buffer };

        VERIFY_IS_TRUE(til::bytes_can_put<uint32_t>(span));
    }

    TEST_METHOD(BytesPutBasic)
    {
        Log::Comment(L"bytes_put should write a value and advance the span.");

        char buffer[16] = {};
        std::span<char> span{ buffer };

        const uint32_t value = 0x12345678;
        til::bytes_put(span, value);

        // Span should be advanced by sizeof(uint32_t) = 4
        VERIFY_ARE_EQUAL(12u, span.size());
        VERIFY_ARE_EQUAL(buffer + 4, span.data());

        // Verify the value was written
        uint32_t written;
        memcpy(&written, buffer, sizeof(written));
        VERIFY_ARE_EQUAL(value, written);
    }

    TEST_METHOD(BytesPutMultipleValues)
    {
        Log::Comment(L"bytes_put should support writing multiple sequential values.");

        char buffer[16] = {};
        std::span<char> span{ buffer };

        const uint32_t val1 = 0xAABBCCDD;
        const uint32_t val2 = 0x11223344;

        til::bytes_put(span, val1);
        til::bytes_put(span, val2);

        VERIFY_ARE_EQUAL(8u, span.size());

        uint32_t read1, read2;
        memcpy(&read1, buffer, sizeof(read1));
        memcpy(&read2, buffer + 4, sizeof(read2));

        VERIFY_ARE_EQUAL(val1, read1);
        VERIFY_ARE_EQUAL(val2, read2);
    }

    TEST_METHOD(BytesPutInsufficientSpace)
    {
        Log::Comment(L"bytes_put should throw when there is not enough space.");

        char buffer[2] = {};
        std::span<char> span{ buffer };

        auto threwException = false;
        try
        {
            til::bytes_put(span, uint32_t{ 0 });
        }
        catch (const std::length_error&)
        {
            threwException = true;
        }

        VERIFY_IS_TRUE(threwException);
    }

    TEST_METHOD(BytesPutSingleByte)
    {
        Log::Comment(L"bytes_put should work with single byte values.");

        char buffer[4] = {};
        std::span<char> span{ buffer };

        const char val = 'X';
        til::bytes_put(span, val);

        VERIFY_ARE_EQUAL(3u, span.size());
        VERIFY_ARE_EQUAL('X', buffer[0]);
    }

    TEST_METHOD(BytesTransferBasic)
    {
        Log::Comment(L"bytes_transfer should copy data from source to target.");

        const char sourceData[] = "Hello World";
        std::string_view source{ sourceData, 5 }; // "Hello"

        char buffer[16] = {};
        std::span<char> target{ buffer };

        til::bytes_transfer(target, source);

        // Both should be advanced
        VERIFY_ARE_EQUAL(11u, target.size());
        VERIFY_ARE_EQUAL(0u, source.size());

        // Data should be copied
        VERIFY_ARE_EQUAL('H', buffer[0]);
        VERIFY_ARE_EQUAL('e', buffer[1]);
        VERIFY_ARE_EQUAL('l', buffer[2]);
        VERIFY_ARE_EQUAL('l', buffer[3]);
        VERIFY_ARE_EQUAL('o', buffer[4]);
    }

    TEST_METHOD(BytesTransferTargetSmaller)
    {
        Log::Comment(L"bytes_transfer should only copy what fits when target is smaller.");

        const char sourceData[] = "ABCDEFGHIJ";
        std::string_view source{ sourceData, 10 };

        char buffer[4] = {};
        std::span<char> target{ buffer };

        til::bytes_transfer(target, source);

        // Target should be fully consumed
        VERIFY_ARE_EQUAL(0u, target.size());
        // Source should have remaining data
        VERIFY_ARE_EQUAL(6u, source.size());

        VERIFY_ARE_EQUAL('A', buffer[0]);
        VERIFY_ARE_EQUAL('B', buffer[1]);
        VERIFY_ARE_EQUAL('C', buffer[2]);
        VERIFY_ARE_EQUAL('D', buffer[3]);
    }

    TEST_METHOD(BytesTransferEmptySource)
    {
        Log::Comment(L"bytes_transfer with empty source should be a no-op.");

        std::string_view source;

        char buffer[8] = {};
        std::span<char> target{ buffer };

        til::bytes_transfer(target, source);

        VERIFY_ARE_EQUAL(8u, target.size());
        VERIFY_ARE_EQUAL(0u, source.size());
    }

    TEST_METHOD(BytesTransferEmptyTarget)
    {
        Log::Comment(L"bytes_transfer with empty target should be a no-op.");

        const char sourceData[] = "Test";
        std::string_view source{ sourceData, 4 };

        std::span<char> target;

        til::bytes_transfer(target, source);

        VERIFY_ARE_EQUAL(0u, target.size());
        VERIFY_ARE_EQUAL(4u, source.size());
    }
};
