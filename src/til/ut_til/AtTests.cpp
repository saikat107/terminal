// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include <til/at.h>

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class AtTests
{
    TEST_CLASS(AtTests);

    TEST_METHOD(AtWithVector)
    {
        Log::Comment(L"til::at should retrieve elements from a vector by index.");

        std::vector<int> vec = { 10, 20, 30, 40, 50 };

        VERIFY_ARE_EQUAL(10, til::at(vec, 0));
        VERIFY_ARE_EQUAL(20, til::at(vec, 1));
        VERIFY_ARE_EQUAL(30, til::at(vec, 2));
        VERIFY_ARE_EQUAL(40, til::at(vec, 3));
        VERIFY_ARE_EQUAL(50, til::at(vec, 4));
    }

    TEST_METHOD(AtWithArray)
    {
        Log::Comment(L"til::at should work with C-style arrays.");

        int arr[] = { 100, 200, 300 };

        VERIFY_ARE_EQUAL(100, til::at(arr, 0));
        VERIFY_ARE_EQUAL(200, til::at(arr, 1));
        VERIFY_ARE_EQUAL(300, til::at(arr, 2));
    }

    TEST_METHOD(AtWithStdArray)
    {
        Log::Comment(L"til::at should work with std::array.");

        std::array<int, 4> arr = { 5, 10, 15, 20 };

        VERIFY_ARE_EQUAL(5, til::at(arr, 0));
        VERIFY_ARE_EQUAL(10, til::at(arr, 1));
        VERIFY_ARE_EQUAL(15, til::at(arr, 2));
        VERIFY_ARE_EQUAL(20, til::at(arr, 3));
    }

    TEST_METHOD(AtWithString)
    {
        Log::Comment(L"til::at should work with std::string.");

        std::string str = "Hello";

        VERIFY_ARE_EQUAL('H', til::at(str, 0));
        VERIFY_ARE_EQUAL('e', til::at(str, 1));
        VERIFY_ARE_EQUAL('l', til::at(str, 2));
        VERIFY_ARE_EQUAL('l', til::at(str, 3));
        VERIFY_ARE_EQUAL('o', til::at(str, 4));
    }

    TEST_METHOD(AtWithWString)
    {
        Log::Comment(L"til::at should work with std::wstring.");

        std::wstring wstr = L"Test";

        VERIFY_ARE_EQUAL(L'T', til::at(wstr, 0));
        VERIFY_ARE_EQUAL(L'e', til::at(wstr, 1));
        VERIFY_ARE_EQUAL(L's', til::at(wstr, 2));
        VERIFY_ARE_EQUAL(L't', til::at(wstr, 3));
    }

    TEST_METHOD(AtReturnsReference)
    {
        Log::Comment(L"til::at should return a reference, allowing mutation.");

        std::vector<int> vec = { 1, 2, 3 };
        til::at(vec, 1) = 42;

        VERIFY_ARE_EQUAL(42, vec[1]);
    }

    TEST_METHOD(AtWithConstContainer)
    {
        Log::Comment(L"til::at should work with const containers.");

        const std::vector<int> vec = { 7, 8, 9 };
        const auto& val = til::at(vec, 2);

        VERIFY_ARE_EQUAL(9, val);
    }

    TEST_METHOD(AtWithDifferentIndexTypes)
    {
        Log::Comment(L"til::at should work with different integer index types.");

        std::vector<int> vec = { 10, 20, 30 };

        const int i = 0;
        const size_t s = 1;
        const ptrdiff_t p = 2;

        VERIFY_ARE_EQUAL(10, til::at(vec, i));
        VERIFY_ARE_EQUAL(20, til::at(vec, s));
        VERIFY_ARE_EQUAL(30, til::at(vec, p));
    }
};
