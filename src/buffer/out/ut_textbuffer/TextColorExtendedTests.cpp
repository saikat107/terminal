// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "../TextColor.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

class TextColorExtendedTests
{
    TEST_CLASS(TextColorExtendedTests);

    TEST_CLASS_SETUP(ClassSetup);

    std::array<COLORREF, TextColor::TABLE_SIZE> _colorTable;
    const COLORREF _defaultFg = RGB(1, 2, 3);
    const COLORREF _defaultBg = RGB(4, 5, 6);
    const size_t _defaultFgIndex = TextColor::DEFAULT_FOREGROUND;
    const size_t _defaultBgIndex = TextColor::DEFAULT_BACKGROUND;

    TEST_METHOD(TestIndex256Color)
    {
        Log::Comment(L"Index256 colors should use the extended color table.");

        // Index 100 is in the 256-color range (beyond the 16 legacy colors)
        TextColor color256(static_cast<BYTE>(100), true);

        VERIFY_IS_FALSE(color256.IsDefault());
        VERIFY_IS_FALSE(color256.IsLegacy());
        VERIFY_IS_TRUE(color256.IsIndex256());
        VERIFY_IS_FALSE(color256.IsRgb());

        VERIFY_ARE_EQUAL(static_cast<BYTE>(100), color256.GetIndex());
    }

    TEST_METHOD(TestIndex16Color)
    {
        Log::Comment(L"Index16 colors are legacy colors (0-15).");

        TextColor color16(static_cast<BYTE>(5), false);

        VERIFY_IS_FALSE(color16.IsDefault());
        VERIFY_IS_TRUE(color16.IsIndex16());
        VERIFY_IS_TRUE(color16.IsLegacy());
        VERIFY_IS_FALSE(color16.IsIndex256());
        VERIFY_IS_FALSE(color16.IsRgb());

        VERIFY_ARE_EQUAL(static_cast<BYTE>(5), color16.GetIndex());
    }

    TEST_METHOD(TestRgbComponents)
    {
        Log::Comment(L"RGB color components should be accessible individually.");

        TextColor rgbColor(RGB(0xAB, 0xCD, 0xEF));

        VERIFY_IS_TRUE(rgbColor.IsRgb());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(0xAB), rgbColor.GetR());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(0xCD), rgbColor.GetG());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(0xEF), rgbColor.GetB());
        VERIFY_ARE_EQUAL(RGB(0xAB, 0xCD, 0xEF), rgbColor.GetRGB());
    }

    TEST_METHOD(TestRgbBlackColor)
    {
        Log::Comment(L"RGB black (0,0,0) should still be recognized as RGB, not default.");

        TextColor black(RGB(0, 0, 0));

        VERIFY_IS_TRUE(black.IsRgb());
        VERIFY_IS_FALSE(black.IsDefault());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(0), black.GetR());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(0), black.GetG());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(0), black.GetB());
    }

    TEST_METHOD(TestRgbWhiteColor)
    {
        Log::Comment(L"RGB white (255,255,255) should have correct components.");

        TextColor white(RGB(255, 255, 255));

        VERIFY_IS_TRUE(white.IsRgb());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(255), white.GetR());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(255), white.GetG());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(255), white.GetB());
    }

    TEST_METHOD(TestDefaultIsDefaultOrLegacy)
    {
        Log::Comment(L"Default color should be classified as DefaultOrLegacy.");

        TextColor defaultColor;

        VERIFY_IS_TRUE(defaultColor.IsDefault());
        VERIFY_IS_TRUE(defaultColor.IsDefaultOrLegacy());
    }

    TEST_METHOD(TestLegacyIsDefaultOrLegacy)
    {
        Log::Comment(L"Legacy (Index16) color should be classified as DefaultOrLegacy.");

        TextColor legacyColor(static_cast<BYTE>(3), false);

        VERIFY_IS_TRUE(legacyColor.IsLegacy());
        VERIFY_IS_TRUE(legacyColor.IsDefaultOrLegacy());
    }

    TEST_METHOD(TestRgbIsNotDefaultOrLegacy)
    {
        Log::Comment(L"RGB color should NOT be classified as DefaultOrLegacy.");

        TextColor rgbColor(RGB(100, 200, 50));

        VERIFY_IS_FALSE(rgbColor.IsDefaultOrLegacy());
    }

    TEST_METHOD(TestIndex256IsNotLegacy)
    {
        Log::Comment(L"Index256 color should NOT be classified as Legacy.");

        TextColor color256(static_cast<BYTE>(100), true);

        VERIFY_IS_FALSE(color256.IsLegacy());
        VERIFY_IS_FALSE(color256.IsDefaultOrLegacy());
    }

    TEST_METHOD(TestSetColor)
    {
        Log::Comment(L"SetColor should change a default color to an RGB color.");

        TextColor color;
        VERIFY_IS_TRUE(color.IsDefault());

        color.SetColor(RGB(10, 20, 30));
        VERIFY_IS_TRUE(color.IsRgb());
        VERIFY_IS_FALSE(color.IsDefault());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(10), color.GetR());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(20), color.GetG());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(30), color.GetB());
    }

    TEST_METHOD(TestSetIndex)
    {
        Log::Comment(L"SetIndex should change an RGB color to an indexed color.");

        TextColor color(RGB(100, 200, 50));
        VERIFY_IS_TRUE(color.IsRgb());

        color.SetIndex(7, false);
        VERIFY_IS_TRUE(color.IsIndex16());
        VERIFY_IS_FALSE(color.IsRgb());
        VERIFY_ARE_EQUAL(static_cast<BYTE>(7), color.GetIndex());
    }

    TEST_METHOD(TestSetDefault)
    {
        Log::Comment(L"SetDefault should reset any color to default.");

        TextColor color(RGB(100, 200, 50));
        VERIFY_IS_TRUE(color.IsRgb());

        color.SetDefault();
        VERIFY_IS_TRUE(color.IsDefault());
        VERIFY_IS_FALSE(color.IsRgb());
    }

    TEST_METHOD(TestCanBeBrightenedDefault)
    {
        Log::Comment(L"Default colors should be able to be brightened.");

        TextColor defaultColor;
        VERIFY_IS_TRUE(defaultColor.CanBeBrightened());
    }

    TEST_METHOD(TestCanBeBrightenedDarkIndex)
    {
        Log::Comment(L"Dark index colors (0-7) should be able to be brightened.");

        TextColor darkIndex(static_cast<BYTE>(3), false);
        VERIFY_IS_TRUE(darkIndex.CanBeBrightened());
    }

    TEST_METHOD(TestCannotBeBrightenedBrightIndex)
    {
        Log::Comment(L"Bright index colors (8-15) should not be brightened.");

        TextColor brightIndex(static_cast<BYTE>(11), false);
        VERIFY_IS_FALSE(brightIndex.CanBeBrightened());
    }

    TEST_METHOD(TestCannotBeBrightenedRgb)
    {
        Log::Comment(L"RGB colors should not be brightened.");

        TextColor rgbColor(RGB(128, 128, 128));
        VERIFY_IS_FALSE(rgbColor.CanBeBrightened());
    }

    TEST_METHOD(TestCannotBeBrightenedIndex256)
    {
        Log::Comment(L"Index256 colors should not be brightened.");

        TextColor index256(static_cast<BYTE>(100), true);
        VERIFY_IS_FALSE(index256.CanBeBrightened());
    }

    TEST_METHOD(TestEquality)
    {
        Log::Comment(L"TextColors with the same type and value should be equal.");

        TextColor default1;
        TextColor default2;
        VERIFY_IS_TRUE(default1 == default2);

        TextColor rgb1(RGB(10, 20, 30));
        TextColor rgb2(RGB(10, 20, 30));
        VERIFY_IS_TRUE(rgb1 == rgb2);

        TextColor idx1(static_cast<BYTE>(5), false);
        TextColor idx2(static_cast<BYTE>(5), false);
        VERIFY_IS_TRUE(idx1 == idx2);
    }

    TEST_METHOD(TestInequality)
    {
        Log::Comment(L"TextColors with different types or values should not be equal.");

        TextColor defaultColor;
        TextColor rgbColor(RGB(10, 20, 30));
        VERIFY_IS_FALSE(defaultColor == rgbColor);

        TextColor rgb1(RGB(10, 20, 30));
        TextColor rgb2(RGB(10, 20, 31)); // different blue
        VERIFY_IS_FALSE(rgb1 == rgb2);

        TextColor idx16(static_cast<BYTE>(5), false);
        TextColor idx256(static_cast<BYTE>(5), true);
        VERIFY_IS_FALSE(idx16 == idx256);
    }

    TEST_METHOD(TestDarkIndexBrightening)
    {
        Log::Comment(L"A dark index color should resolve to its bright counterpart when brightened.");

        TextColor darkColor(static_cast<BYTE>(2), false); // Dark Green (index 2)

        auto normal = darkColor.GetColor(_colorTable, _defaultFgIndex, false);
        VERIFY_ARE_EQUAL(_colorTable[2], normal);

        auto brightened = darkColor.GetColor(_colorTable, _defaultFgIndex, true);
        VERIFY_ARE_EQUAL(_colorTable[10], brightened); // Bright Green (index 10 = 2 + 8)
    }

    TEST_METHOD(TestIndex256ColorResolves)
    {
        Log::Comment(L"Index256 colors should resolve to their table entry.");

        TextColor color256(static_cast<BYTE>(3), true);

        auto resolved = color256.GetColor(_colorTable, _defaultFgIndex, false);
        VERIFY_ARE_EQUAL(_colorTable[3], resolved);

        // 256-color indexes should NOT be brightened
        auto resolvedBright = color256.GetColor(_colorTable, _defaultFgIndex, true);
        VERIFY_ARE_EQUAL(_colorTable[3], resolvedBright);
    }

    TEST_METHOD(TestTransitionBetweenAllTypes)
    {
        Log::Comment(L"A color should correctly transition through all types.");

        TextColor color;

        // Start as default
        VERIFY_IS_TRUE(color.IsDefault());

        // Change to RGB
        color.SetColor(RGB(255, 0, 0));
        VERIFY_IS_TRUE(color.IsRgb());
        VERIFY_ARE_EQUAL(RGB(255, 0, 0), color.GetRGB());

        // Change to Index16
        color.SetIndex(5, false);
        VERIFY_IS_TRUE(color.IsIndex16());

        // Change to Index256
        color.SetIndex(200, true);
        VERIFY_IS_TRUE(color.IsIndex256());

        // Back to default
        color.SetDefault();
        VERIFY_IS_TRUE(color.IsDefault());
    }
};

bool TextColorExtendedTests::ClassSetup()
{
    _colorTable[0] = RGB(12, 12, 12);
    _colorTable[1] = RGB(0, 55, 218);
    _colorTable[2] = RGB(19, 161, 14);
    _colorTable[3] = RGB(58, 150, 221);
    _colorTable[4] = RGB(197, 15, 31);
    _colorTable[5] = RGB(136, 23, 152);
    _colorTable[6] = RGB(193, 156, 0);
    _colorTable[7] = RGB(204, 204, 204);
    _colorTable[8] = RGB(118, 118, 118);
    _colorTable[9] = RGB(59, 120, 255);
    _colorTable[10] = RGB(22, 198, 12);
    _colorTable[11] = RGB(97, 214, 214);
    _colorTable[12] = RGB(231, 72, 86);
    _colorTable[13] = RGB(180, 0, 158);
    _colorTable[14] = RGB(249, 241, 165);
    _colorTable[15] = RGB(242, 242, 242);
    _colorTable[_defaultFgIndex] = _defaultFg;
    _colorTable[_defaultBgIndex] = _defaultBg;
    return true;
}
