// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "../inc/viewport.hpp"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

using namespace Microsoft::Console::Types;

class ViewportTests
{
    TEST_CLASS(ViewportTests);

    TEST_METHOD(EmptyViewport)
    {
        Log::Comment(L"Empty viewport should have zero or negative dimensions and be invalid.");

        const auto vp = Viewport::Empty();

        VERIFY_IS_FALSE(vp.IsValid());
    }

    TEST_METHOD(FromInclusiveCreatesCorrectBounds)
    {
        Log::Comment(L"FromInclusive should create a viewport with the exact inclusive coordinates.");

        const til::inclusive_rect sr{ 2, 3, 10, 15 };
        const auto vp = Viewport::FromInclusive(sr);

        VERIFY_ARE_EQUAL(2, vp.Left());
        VERIFY_ARE_EQUAL(3, vp.Top());
        VERIFY_ARE_EQUAL(10, vp.RightInclusive());
        VERIFY_ARE_EQUAL(11, vp.RightExclusive());
        VERIFY_ARE_EQUAL(15, vp.BottomInclusive());
        VERIFY_ARE_EQUAL(16, vp.BottomExclusive());
    }

    TEST_METHOD(FromExclusiveCreatesCorrectBounds)
    {
        Log::Comment(L"FromExclusive should create a viewport with correct inclusive coordinates.");

        const til::rect sr{ 2, 3, 11, 16 };
        const auto vp = Viewport::FromExclusive(sr);

        VERIFY_ARE_EQUAL(2, vp.Left());
        VERIFY_ARE_EQUAL(3, vp.Top());
        VERIFY_ARE_EQUAL(10, vp.RightInclusive());
        VERIFY_ARE_EQUAL(11, vp.RightExclusive());
        VERIFY_ARE_EQUAL(15, vp.BottomInclusive());
        VERIFY_ARE_EQUAL(16, vp.BottomExclusive());
    }

    TEST_METHOD(FromDimensionsCreatesCorrectBounds)
    {
        Log::Comment(L"FromDimensions should create a viewport at the given origin with the given size.");

        const auto vp = Viewport::FromDimensions({ 5, 10 }, { 80, 24 });

        VERIFY_ARE_EQUAL(5, vp.Left());
        VERIFY_ARE_EQUAL(10, vp.Top());
        VERIFY_ARE_EQUAL(84, vp.RightInclusive());
        VERIFY_ARE_EQUAL(33, vp.BottomInclusive());
        VERIFY_ARE_EQUAL(80, vp.Width());
        VERIFY_ARE_EQUAL(24, vp.Height());
    }

    TEST_METHOD(FromDimensionsAtOrigin)
    {
        Log::Comment(L"FromDimensions at (0,0) should match typical console viewport.");

        const auto vp = Viewport::FromDimensions({ 0, 0 }, { 120, 30 });

        VERIFY_ARE_EQUAL(0, vp.Left());
        VERIFY_ARE_EQUAL(0, vp.Top());
        VERIFY_ARE_EQUAL(119, vp.RightInclusive());
        VERIFY_ARE_EQUAL(29, vp.BottomInclusive());
        VERIFY_ARE_EQUAL(120, vp.Width());
        VERIFY_ARE_EQUAL(30, vp.Height());
    }

    TEST_METHOD(WidthAndHeight)
    {
        Log::Comment(L"Width and Height should correctly reflect the viewport dimensions.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 79, 23 });

        VERIFY_ARE_EQUAL(80, vp.Width());
        VERIFY_ARE_EQUAL(24, vp.Height());
    }

    TEST_METHOD(OriginAndCorners)
    {
        Log::Comment(L"Origin and corner methods should return correct coordinates.");

        const auto vp = Viewport::FromInclusive({ 5, 10, 25, 30 });

        const auto origin = vp.Origin();
        VERIFY_ARE_EQUAL(5, origin.x);
        VERIFY_ARE_EQUAL(10, origin.y);

        const auto brInclusive = vp.BottomRightInclusive();
        VERIFY_ARE_EQUAL(25, brInclusive.x);
        VERIFY_ARE_EQUAL(30, brInclusive.y);

        const auto brExclusive = vp.BottomRightExclusive();
        VERIFY_ARE_EQUAL(26, brExclusive.x);
        VERIFY_ARE_EQUAL(31, brExclusive.y);

        const auto endExcl = vp.EndExclusive();
        VERIFY_ARE_EQUAL(5, endExcl.x);
        VERIFY_ARE_EQUAL(31, endExcl.y);
    }

    TEST_METHOD(Dimensions)
    {
        const auto vp = Viewport::FromInclusive({ 0, 0, 9, 4 });
        const auto dim = vp.Dimensions();

        VERIFY_ARE_EQUAL(10, dim.width);
        VERIFY_ARE_EQUAL(5, dim.height);
    }

    TEST_METHOD(IsInBoundsViewport)
    {
        Log::Comment(L"IsInBounds should detect when another viewport fits inside.");

        const auto outer = Viewport::FromInclusive({ 0, 0, 79, 23 });
        const auto inner = Viewport::FromInclusive({ 5, 5, 20, 10 });
        const auto overlapping = Viewport::FromInclusive({ 70, 20, 85, 30 });

        VERIFY_IS_TRUE(outer.IsInBounds(inner));
        VERIFY_IS_FALSE(outer.IsInBounds(overlapping));
    }

    TEST_METHOD(IsInBoundsPoint)
    {
        Log::Comment(L"IsInBounds should detect when a point is inside the viewport.");

        const auto vp = Viewport::FromInclusive({ 5, 10, 25, 30 });

        // Inside
        VERIFY_IS_TRUE(vp.IsInBounds({ 5, 10 }));
        VERIFY_IS_TRUE(vp.IsInBounds({ 15, 20 }));
        VERIFY_IS_TRUE(vp.IsInBounds({ 25, 30 }));

        // Outside
        VERIFY_IS_FALSE(vp.IsInBounds({ 4, 10 }));
        VERIFY_IS_FALSE(vp.IsInBounds({ 26, 10 }));
        VERIFY_IS_FALSE(vp.IsInBounds({ 5, 9 }));
        VERIFY_IS_FALSE(vp.IsInBounds({ 5, 31 }));
    }

    TEST_METHOD(IsInBoundsPointAllowEndExclusive)
    {
        Log::Comment(L"IsInBounds with allowEndExclusive should accept the EndExclusive point.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 79, 23 });
        const auto endExcl = vp.EndExclusive(); // {0, 24}

        // Without allowEndExclusive, EndExclusive is out of bounds
        VERIFY_IS_FALSE(vp.IsInBounds(endExcl, false));
        // With allowEndExclusive, EndExclusive is in bounds
        VERIFY_IS_TRUE(vp.IsInBounds(endExcl, true));
    }

    TEST_METHOD(ClampPoint)
    {
        Log::Comment(L"Clamp should constrain a point inside the viewport.");

        const auto vp = Viewport::FromInclusive({ 5, 10, 25, 30 });

        // Point above and to the left
        til::point above{ 0, 0 };
        vp.Clamp(above);
        VERIFY_ARE_EQUAL(5, above.x);
        VERIFY_ARE_EQUAL(10, above.y);

        // Point below and to the right
        til::point below{ 100, 100 };
        vp.Clamp(below);
        VERIFY_ARE_EQUAL(25, below.x);
        VERIFY_ARE_EQUAL(30, below.y);

        // Point already inside
        til::point inside{ 15, 20 };
        vp.Clamp(inside);
        VERIFY_ARE_EQUAL(15, inside.x);
        VERIFY_ARE_EQUAL(20, inside.y);
    }

    TEST_METHOD(ClampViewport)
    {
        Log::Comment(L"Clamp viewport should clip another viewport to fit inside.");

        const auto outer = Viewport::FromInclusive({ 0, 0, 79, 23 });
        const auto overlapping = Viewport::FromInclusive({ 70, 20, 90, 30 });

        const auto clamped = outer.Clamp(overlapping);

        VERIFY_ARE_EQUAL(70, clamped.Left());
        VERIFY_ARE_EQUAL(20, clamped.Top());
        VERIFY_ARE_EQUAL(79, clamped.RightInclusive());
        VERIFY_ARE_EQUAL(23, clamped.BottomInclusive());
    }

    TEST_METHOD(IncrementInBounds)
    {
        Log::Comment(L"IncrementInBounds should step through the viewport left to right, top to bottom.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 2, 1 }); // 3x2

        til::point pos{ 0, 0 };

        VERIFY_IS_TRUE(vp.IncrementInBounds(pos));
        VERIFY_ARE_EQUAL(1, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);

        VERIFY_IS_TRUE(vp.IncrementInBounds(pos));
        VERIFY_ARE_EQUAL(2, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);

        VERIFY_IS_TRUE(vp.IncrementInBounds(pos));
        VERIFY_ARE_EQUAL(0, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        VERIFY_IS_TRUE(vp.IncrementInBounds(pos));
        VERIFY_ARE_EQUAL(1, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        VERIFY_IS_TRUE(vp.IncrementInBounds(pos));
        VERIFY_ARE_EQUAL(2, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        // At the last position, incrementing should clamp and return false
        VERIFY_IS_FALSE(vp.IncrementInBounds(pos));
        VERIFY_ARE_EQUAL(2, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);
    }

    TEST_METHOD(DecrementInBounds)
    {
        Log::Comment(L"DecrementInBounds should step backward through the viewport.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 2, 1 }); // 3x2

        til::point pos{ 2, 1 }; // bottom-right

        VERIFY_IS_TRUE(vp.DecrementInBounds(pos));
        VERIFY_ARE_EQUAL(1, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        VERIFY_IS_TRUE(vp.DecrementInBounds(pos));
        VERIFY_ARE_EQUAL(0, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        VERIFY_IS_TRUE(vp.DecrementInBounds(pos));
        VERIFY_ARE_EQUAL(2, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);

        VERIFY_IS_TRUE(vp.DecrementInBounds(pos));
        VERIFY_ARE_EQUAL(1, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);

        VERIFY_IS_TRUE(vp.DecrementInBounds(pos));
        VERIFY_ARE_EQUAL(0, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);

        // At the first position, decrementing should clamp and return false
        VERIFY_IS_FALSE(vp.DecrementInBounds(pos));
        VERIFY_ARE_EQUAL(0, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);
    }

    TEST_METHOD(CompareInBounds)
    {
        Log::Comment(L"CompareInBounds should return negative, zero, or positive for ordering.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 79, 23 });

        // Same position
        VERIFY_ARE_EQUAL(0, vp.CompareInBounds({ 5, 5 }, { 5, 5 }));

        // First is left of second (same row)
        VERIFY_IS_TRUE(vp.CompareInBounds({ 3, 5 }, { 10, 5 }) < 0);

        // First is right of second (same row)
        VERIFY_IS_TRUE(vp.CompareInBounds({ 10, 5 }, { 3, 5 }) > 0);

        // First is above second (different rows)
        VERIFY_IS_TRUE(vp.CompareInBounds({ 0, 3 }, { 0, 5 }) < 0);

        // First is below second (different rows)
        VERIFY_IS_TRUE(vp.CompareInBounds({ 0, 5 }, { 0, 3 }) > 0);

        // End of one row vs start of next (should be -1 distance)
        const auto cmp = vp.CompareInBounds({ 79, 4 }, { 0, 5 });
        VERIFY_ARE_EQUAL(-1, cmp);
    }

    TEST_METHOD(WalkInBoundsForward)
    {
        Log::Comment(L"WalkInBounds with positive delta should move forward.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 4, 2 }); // 5x3

        til::point pos{ 0, 0 };

        // Walk forward by 6 (should move to position 6 -> col 1, row 1)
        VERIFY_IS_TRUE(vp.WalkInBounds(pos, 6));
        VERIFY_ARE_EQUAL(1, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        // Walk to end
        VERIFY_IS_TRUE(vp.WalkInBounds(pos, 8));
        VERIFY_ARE_EQUAL(4, pos.x);
        VERIFY_ARE_EQUAL(2, pos.y);

        // Walk past end should clamp
        VERIFY_IS_FALSE(vp.WalkInBounds(pos, 1));
        VERIFY_ARE_EQUAL(4, pos.x);
        VERIFY_ARE_EQUAL(2, pos.y);
    }

    TEST_METHOD(WalkInBoundsBackward)
    {
        Log::Comment(L"WalkInBounds with negative delta should move backward.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 4, 2 }); // 5x3

        til::point pos{ 4, 2 }; // last position

        // Walk backward by 6
        VERIFY_IS_TRUE(vp.WalkInBounds(pos, -6));
        VERIFY_ARE_EQUAL(3, pos.x);
        VERIFY_ARE_EQUAL(1, pos.y);

        // Walk past beginning should clamp to origin
        VERIFY_IS_FALSE(vp.WalkInBounds(pos, -100));
        VERIFY_ARE_EQUAL(0, pos.x);
        VERIFY_ARE_EQUAL(0, pos.y);
    }

    TEST_METHOD(GetWalkOriginForward)
    {
        Log::Comment(L"GetWalkOrigin with positive delta should return top-left.");

        const auto vp = Viewport::FromInclusive({ 5, 10, 25, 30 });

        const auto origin = vp.GetWalkOrigin(1);
        VERIFY_ARE_EQUAL(5, origin.x);
        VERIFY_ARE_EQUAL(10, origin.y);
    }

    TEST_METHOD(GetWalkOriginBackward)
    {
        Log::Comment(L"GetWalkOrigin with negative delta should return bottom-right.");

        const auto vp = Viewport::FromInclusive({ 5, 10, 25, 30 });

        const auto origin = vp.GetWalkOrigin(-1);
        VERIFY_ARE_EQUAL(25, origin.x);
        VERIFY_ARE_EQUAL(30, origin.y);
    }

    TEST_METHOD(TrimToViewport)
    {
        Log::Comment(L"TrimToViewport should clip a rectangle to the viewport bounds.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 79, 23 });

        // Overlapping rectangle
        til::rect rect{ 70, 20, 90, 30 };
        auto valid = vp.TrimToViewport(&rect);
        VERIFY_IS_TRUE(valid);
        VERIFY_ARE_EQUAL(70, rect.left);
        VERIFY_ARE_EQUAL(20, rect.top);
        VERIFY_ARE_EQUAL(80, rect.right);  // exclusive
        VERIFY_ARE_EQUAL(24, rect.bottom); // exclusive

        // Non-overlapping rectangle
        til::rect noOverlap{ 100, 100, 200, 200 };
        valid = vp.TrimToViewport(&noOverlap);
        VERIFY_IS_FALSE(valid);
    }

    TEST_METHOD(ConvertToOriginPoint)
    {
        Log::Comment(L"ConvertToOrigin should translate a point relative to the viewport's origin.");

        const auto vp = Viewport::FromInclusive({ 10, 20, 50, 40 });

        til::point pt{ 15, 25 };
        vp.ConvertToOrigin(&pt);

        VERIFY_ARE_EQUAL(5, pt.x);
        VERIFY_ARE_EQUAL(5, pt.y);
    }

    TEST_METHOD(ConvertFromOriginPoint)
    {
        Log::Comment(L"ConvertFromOrigin should translate a point back to absolute coordinates.");

        const auto vp = Viewport::FromInclusive({ 10, 20, 50, 40 });

        til::point pt{ 5, 5 };
        vp.ConvertFromOrigin(&pt);

        VERIFY_ARE_EQUAL(15, pt.x);
        VERIFY_ARE_EQUAL(25, pt.y);
    }

    TEST_METHOD(ConvertToOriginViewport)
    {
        Log::Comment(L"ConvertToOrigin on a viewport should translate it relative to this viewport's origin.");

        const auto vp = Viewport::FromInclusive({ 5, 6, 7, 8 });
        const auto other = Viewport::FromInclusive({ 6, 5, 11, 11 });

        const auto result = vp.ConvertToOrigin(other);

        VERIFY_ARE_EQUAL(1, result.Left());
        VERIFY_ARE_EQUAL(-1, result.Top());
        VERIFY_ARE_EQUAL(6, result.RightInclusive());
        VERIFY_ARE_EQUAL(5, result.BottomInclusive());
    }

    TEST_METHOD(ConvertFromOriginViewport)
    {
        Log::Comment(L"ConvertFromOrigin on a viewport should translate it to absolute coordinates.");

        const auto vp = Viewport::FromInclusive({ 5, 6, 7, 8 });
        const auto other = Viewport::FromInclusive({ 0, 0, 5, 6 });

        const auto result = vp.ConvertFromOrigin(other);

        VERIFY_ARE_EQUAL(5, result.Left());
        VERIFY_ARE_EQUAL(6, result.Top());
        VERIFY_ARE_EQUAL(10, result.RightInclusive());
        VERIFY_ARE_EQUAL(12, result.BottomInclusive());
    }

    TEST_METHOD(ToExclusiveAndInclusive)
    {
        Log::Comment(L"ToExclusive and ToInclusive should roundtrip correctly.");

        const auto vp = Viewport::FromInclusive({ 5, 10, 25, 30 });

        const auto excl = vp.ToExclusive();
        VERIFY_ARE_EQUAL(5, excl.left);
        VERIFY_ARE_EQUAL(10, excl.top);
        VERIFY_ARE_EQUAL(26, excl.right);
        VERIFY_ARE_EQUAL(31, excl.bottom);

        const auto incl = vp.ToInclusive();
        VERIFY_ARE_EQUAL(5, incl.left);
        VERIFY_ARE_EQUAL(10, incl.top);
        VERIFY_ARE_EQUAL(25, incl.right);
        VERIFY_ARE_EQUAL(30, incl.bottom);
    }

    TEST_METHOD(ToOrigin)
    {
        Log::Comment(L"ToOrigin should produce a viewport with same dimensions at (0,0).");

        const auto vp = Viewport::FromInclusive({ 6, 5, 11, 11 });
        const auto atOrigin = vp.ToOrigin();

        VERIFY_ARE_EQUAL(0, atOrigin.Left());
        VERIFY_ARE_EQUAL(0, atOrigin.Top());
        VERIFY_ARE_EQUAL(vp.Width(), atOrigin.Width());
        VERIFY_ARE_EQUAL(vp.Height(), atOrigin.Height());
    }

    TEST_METHOD(OffsetViewport)
    {
        Log::Comment(L"Offset should translate a viewport by the given delta.");

        const auto original = Viewport::FromInclusive({ 0, 0, 9, 9 });
        const auto offset = Viewport::Offset(original, { 5, 10 });

        VERIFY_ARE_EQUAL(5, offset.Left());
        VERIFY_ARE_EQUAL(10, offset.Top());
        VERIFY_ARE_EQUAL(14, offset.RightInclusive());
        VERIFY_ARE_EQUAL(19, offset.BottomInclusive());
        VERIFY_ARE_EQUAL(original.Width(), offset.Width());
        VERIFY_ARE_EQUAL(original.Height(), offset.Height());
    }

    TEST_METHOD(UnionOfTwoViewports)
    {
        Log::Comment(L"Union should create a viewport that encompasses both inputs.");

        const auto a = Viewport::FromInclusive({ 0, 0, 10, 10 });
        const auto b = Viewport::FromInclusive({ 5, 5, 20, 20 });

        const auto result = Viewport::Union(a, b);

        VERIFY_ARE_EQUAL(0, result.Left());
        VERIFY_ARE_EQUAL(0, result.Top());
        VERIFY_ARE_EQUAL(20, result.RightInclusive());
        VERIFY_ARE_EQUAL(20, result.BottomInclusive());
    }

    TEST_METHOD(UnionWithEmptyViewport)
    {
        Log::Comment(L"Union with an empty viewport should return the valid viewport.");

        const auto valid = Viewport::FromInclusive({ 5, 5, 15, 15 });
        const auto empty = Viewport::Empty();

        const auto result1 = Viewport::Union(valid, empty);
        VERIFY_ARE_EQUAL(valid, result1);

        const auto result2 = Viewport::Union(empty, valid);
        VERIFY_ARE_EQUAL(valid, result2);
    }

    TEST_METHOD(UnionOfTwoEmptyViewports)
    {
        Log::Comment(L"Union of two empty viewports should return empty.");

        const auto empty1 = Viewport::Empty();
        const auto empty2 = Viewport::Empty();

        const auto result = Viewport::Union(empty1, empty2);
        VERIFY_IS_FALSE(result.IsValid());
    }

    TEST_METHOD(IntersectOverlapping)
    {
        Log::Comment(L"Intersect of overlapping viewports should return the overlap area.");

        const auto a = Viewport::FromInclusive({ 0, 0, 10, 10 });
        const auto b = Viewport::FromInclusive({ 5, 5, 20, 20 });

        const auto result = Viewport::Intersect(a, b);

        VERIFY_IS_TRUE(result.IsValid());
        VERIFY_ARE_EQUAL(5, result.Left());
        VERIFY_ARE_EQUAL(5, result.Top());
        VERIFY_ARE_EQUAL(10, result.RightInclusive());
        VERIFY_ARE_EQUAL(10, result.BottomInclusive());
    }

    TEST_METHOD(IntersectNonOverlapping)
    {
        Log::Comment(L"Intersect of non-overlapping viewports should return empty.");

        const auto a = Viewport::FromInclusive({ 0, 0, 5, 5 });
        const auto b = Viewport::FromInclusive({ 10, 10, 20, 20 });

        const auto result = Viewport::Intersect(a, b);
        VERIFY_IS_FALSE(result.IsValid());
    }

    TEST_METHOD(IntersectContained)
    {
        Log::Comment(L"Intersect where one viewport is inside the other should return the inner viewport.");

        const auto outer = Viewport::FromInclusive({ 0, 0, 50, 50 });
        const auto inner = Viewport::FromInclusive({ 10, 10, 20, 20 });

        const auto result = Viewport::Intersect(outer, inner);
        VERIFY_ARE_EQUAL(inner, result);
    }

    TEST_METHOD(SubtractCentered)
    {
        Log::Comment(L"Subtracting a centered viewport should produce 4 remaining regions.");

        const auto original = Viewport::FromInclusive({ 0, 0, 20, 20 });
        const auto removeMe = Viewport::FromInclusive({ 5, 5, 15, 15 });

        const auto result = Viewport::Subtract(original, removeMe);

        // Should have up to 4 regions (top, bottom, left, right)
        int validCount = 0;
        for (const auto& vp : result)
        {
            if (vp.IsValid())
            {
                validCount++;
            }
        }

        VERIFY_ARE_EQUAL(4, validCount);
    }

    TEST_METHOD(SubtractNonOverlapping)
    {
        Log::Comment(L"Subtracting a non-overlapping viewport should return the original.");

        const auto original = Viewport::FromInclusive({ 0, 0, 10, 10 });
        const auto removeMe = Viewport::FromInclusive({ 20, 20, 30, 30 });

        const auto result = Viewport::Subtract(original, removeMe);

        VERIFY_ARE_EQUAL(1u, result.size());
        VERIFY_ARE_EQUAL(original, result[0]);
    }

    TEST_METHOD(SubtractSameViewport)
    {
        Log::Comment(L"Subtracting the same viewport should return no valid regions.");

        const auto vp = Viewport::FromInclusive({ 0, 0, 10, 10 });

        const auto result = Viewport::Subtract(vp, vp);

        VERIFY_ARE_EQUAL(0u, result.size());
    }

    TEST_METHOD(IsValid)
    {
        Log::Comment(L"IsValid should return true only for viewports with positive area.");

        const auto valid = Viewport::FromInclusive({ 0, 0, 10, 10 });
        VERIFY_IS_TRUE(valid.IsValid());

        const auto empty = Viewport::Empty();
        VERIFY_IS_FALSE(empty.IsValid());

        // A viewport with zero width (left > right)
        const auto zeroWidth = Viewport::FromInclusive({ 10, 0, 5, 10 });
        VERIFY_IS_FALSE(zeroWidth.IsValid());
    }

    TEST_METHOD(Equality)
    {
        Log::Comment(L"Viewports with same bounds should be equal; different bounds should not.");

        const auto a = Viewport::FromInclusive({ 0, 0, 10, 10 });
        const auto b = Viewport::FromInclusive({ 0, 0, 10, 10 });
        const auto c = Viewport::FromInclusive({ 0, 0, 10, 11 });

        VERIFY_IS_TRUE(a == b);
        VERIFY_IS_FALSE(a != b);
        VERIFY_IS_TRUE(a != c);
        VERIFY_IS_FALSE(a == c);
    }

    TEST_METHOD(DetermineWalkDirection)
    {
        Log::Comment(L"DetermineWalkDirection should return 1 when target is before source, -1 otherwise.");

        const auto source = Viewport::FromInclusive({ 5, 5, 10, 10 });
        const auto targetBefore = Viewport::FromInclusive({ 0, 0, 5, 5 });
        const auto targetAfter = Viewport::FromInclusive({ 10, 10, 20, 20 });

        VERIFY_ARE_EQUAL(1, Viewport::DetermineWalkDirection(source, targetBefore));
        VERIFY_ARE_EQUAL(-1, Viewport::DetermineWalkDirection(source, targetAfter));
    }

    TEST_METHOD(ConvertToOriginRect)
    {
        Log::Comment(L"ConvertToOrigin for exclusive rects should subtract viewport origin.");

        const auto vp = Viewport::FromInclusive({ 10, 20, 50, 40 });

        til::rect r{ 15, 25, 30, 35 };
        vp.ConvertToOrigin(&r);

        VERIFY_ARE_EQUAL(5, r.left);
        VERIFY_ARE_EQUAL(5, r.top);
        VERIFY_ARE_EQUAL(20, r.right);
        VERIFY_ARE_EQUAL(15, r.bottom);
    }

    TEST_METHOD(ConvertToOriginInclusiveRect)
    {
        Log::Comment(L"ConvertToOrigin for inclusive rects should subtract viewport origin.");

        const auto vp = Viewport::FromInclusive({ 10, 20, 50, 40 });

        til::inclusive_rect r{ 15, 25, 30, 35 };
        vp.ConvertToOrigin(&r);

        VERIFY_ARE_EQUAL(5, r.left);
        VERIFY_ARE_EQUAL(5, r.top);
        VERIFY_ARE_EQUAL(20, r.right);
        VERIFY_ARE_EQUAL(15, r.bottom);
    }

    TEST_METHOD(ConvertFromOriginInclusiveRect)
    {
        Log::Comment(L"ConvertFromOrigin for inclusive rects should add viewport origin.");

        const auto vp = Viewport::FromInclusive({ 10, 20, 50, 40 });

        til::inclusive_rect r{ 5, 5, 20, 15 };
        vp.ConvertFromOrigin(&r);

        VERIFY_ARE_EQUAL(15, r.left);
        VERIFY_ARE_EQUAL(25, r.top);
        VERIFY_ARE_EQUAL(30, r.right);
        VERIFY_ARE_EQUAL(35, r.bottom);
    }

    TEST_METHOD(FromInclusiveAndExclusiveRoundtrip)
    {
        Log::Comment(L"Converting between inclusive and exclusive representations should roundtrip.");

        const til::inclusive_rect inclusive{ 5, 10, 25, 30 };
        const auto vp = Viewport::FromInclusive(inclusive);

        const auto excl = vp.ToExclusive();
        const auto vp2 = Viewport::FromExclusive(excl);

        VERIFY_ARE_EQUAL(vp, vp2);
    }
};
