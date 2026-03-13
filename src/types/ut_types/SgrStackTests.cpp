// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "WexTestClass.h"
#include "../../inc/consoletaeftemplates.hpp"

#include "../inc/sgrStack.hpp"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

using namespace Microsoft::Console::VirtualTerminal;
using namespace Microsoft::Console::VirtualTerminal::DispatchTypes;

class SgrStackTests
{
    TEST_CLASS(SgrStackTests);

    TEST_METHOD(PushAndPopAllAttributes)
    {
        Log::Comment(L"Push and pop with no options should save and restore all attributes.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetIntense(true);
        savedAttr.SetForeground(COLORREF(0x00FF00));
        savedAttr.SetBackground(COLORREF(0x0000FF));

        // Push with empty options (saves all)
        VTParameters emptyParams;
        stack.Push(savedAttr, emptyParams);

        // Current attributes are different
        TextAttribute currentAttr;
        currentAttr.SetFaint(true);

        // Pop should restore the saved attributes
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsIntense());
        VERIFY_IS_FALSE(restored.IsFaint());
    }

    TEST_METHOD(PopOnEmptyStackReturnsCurrentAttributes)
    {
        Log::Comment(L"Pop on an empty stack should return the current attributes unchanged.");

        SgrStack stack;

        TextAttribute current;
        current.SetIntense(true);
        current.SetItalic(true);

        const auto result = stack.Pop(current);

        VERIFY_IS_TRUE(result.IsIntense());
        VERIFY_IS_TRUE(result.IsItalic());
    }

    TEST_METHOD(PushPopSingleAttribute)
    {
        Log::Comment(L"Push with specific option should only save/restore that attribute.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetIntense(true);
        savedAttr.SetItalic(true);
        savedAttr.SetBlinking(true);

        // Push only the Intense attribute (option 1)
        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Intense) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        // Current attributes: nothing set
        TextAttribute currentAttr;
        currentAttr.SetFaint(true); // set something different

        const auto restored = stack.Pop(currentAttr);

        // Intense should be restored from saved
        VERIFY_IS_TRUE(restored.IsIntense());
        // Faint should remain from current (not overwritten)
        VERIFY_IS_TRUE(restored.IsFaint());
        // Italic should NOT be restored (wasn't saved selectively)
        VERIFY_IS_FALSE(restored.IsItalic());
    }

    TEST_METHOD(PushPopMultipleSelectiveAttributes)
    {
        Log::Comment(L"Push with multiple options should save/restore only those attributes.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetIntense(true);
        savedAttr.SetItalic(true);
        savedAttr.SetBlinking(true);
        savedAttr.SetReverseVideo(true);

        // Save Intense (1) and Italics (3) selectively
        VTParameter params[] = {
            VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Intense) },
            VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Italics) }
        };
        VTParameters options{ params, 2 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsIntense());
        VERIFY_IS_TRUE(restored.IsItalic());
        // These were NOT saved selectively, so they should remain as current (false)
        VERIFY_IS_FALSE(restored.IsBlinking());
        VERIFY_IS_FALSE(restored.IsReverseVideo());
    }

    TEST_METHOD(PushPopForegroundColor)
    {
        Log::Comment(L"Push with SaveForegroundColor option should save/restore only the foreground.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetForeground(COLORREF(0xFF0000));
        savedAttr.SetBackground(COLORREF(0x00FF00));

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::SaveForegroundColor) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        currentAttr.SetBackground(COLORREF(0x0000FF));

        const auto restored = stack.Pop(currentAttr);

        // Foreground should be restored from saved
        VERIFY_ARE_EQUAL(savedAttr.GetForeground(), restored.GetForeground());
        // Background should remain from current (not saved selectively)
        VERIFY_ARE_EQUAL(currentAttr.GetBackground(), restored.GetBackground());
    }

    TEST_METHOD(PushPopBackgroundColor)
    {
        Log::Comment(L"Push with SaveBackgroundColor option should save/restore only the background.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetForeground(COLORREF(0xFF0000));
        savedAttr.SetBackground(COLORREF(0x00FF00));

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::SaveBackgroundColor) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        currentAttr.SetForeground(COLORREF(0x0000FF));

        const auto restored = stack.Pop(currentAttr);

        // Background should be restored from saved
        VERIFY_ARE_EQUAL(savedAttr.GetBackground(), restored.GetBackground());
        // Foreground should remain from current
        VERIFY_ARE_EQUAL(currentAttr.GetForeground(), restored.GetForeground());
    }

    TEST_METHOD(MultiplePushPopLIFOOrder)
    {
        Log::Comment(L"Multiple pushes and pops should follow LIFO order.");

        SgrStack stack;
        VTParameters emptyParams;

        // Push first: intense
        TextAttribute attr1;
        attr1.SetIntense(true);
        stack.Push(attr1, emptyParams);

        // Push second: italic
        TextAttribute attr2;
        attr2.SetItalic(true);
        stack.Push(attr2, emptyParams);

        // Push third: blinking
        TextAttribute attr3;
        attr3.SetBlinking(true);
        stack.Push(attr3, emptyParams);

        TextAttribute current;

        // Pop should return blinking first (LIFO)
        const auto pop1 = stack.Pop(current);
        VERIFY_IS_TRUE(pop1.IsBlinking());
        VERIFY_IS_FALSE(pop1.IsItalic());
        VERIFY_IS_FALSE(pop1.IsIntense());

        // Pop should return italic second
        const auto pop2 = stack.Pop(current);
        VERIFY_IS_TRUE(pop2.IsItalic());
        VERIFY_IS_FALSE(pop2.IsIntense());

        // Pop should return intense third
        const auto pop3 = stack.Pop(current);
        VERIFY_IS_TRUE(pop3.IsIntense());
    }

    TEST_METHOD(RingStackOverflow)
    {
        Log::Comment(L"When stack is full (10 pushes), new push should drop the oldest entry.");

        SgrStack stack;
        VTParameters emptyParams;

        // Push 10 items to fill the stack
        for (int i = 0; i < SgrStack::c_MaxStoredSgrPushes; i++)
        {
            TextAttribute attr;
            if (i == 0)
            {
                attr.SetIntense(true); // First push: intense
            }
            else
            {
                attr.SetItalic(true); // Rest: italic
            }
            stack.Push(attr, emptyParams);
        }

        // Push one more (11th), which should drop the first (intense) entry
        TextAttribute overflowAttr;
        overflowAttr.SetBlinking(true);
        stack.Push(overflowAttr, emptyParams);

        TextAttribute current;

        // Pop the most recent (blinking)
        const auto pop1 = stack.Pop(current);
        VERIFY_IS_TRUE(pop1.IsBlinking());

        // Pop the remaining 9 entries (all italic)
        for (int i = 0; i < SgrStack::c_MaxStoredSgrPushes - 1; i++)
        {
            const auto popped = stack.Pop(current);
            VERIFY_IS_TRUE(popped.IsItalic());
        }

        // Stack should now be empty; pop should return current
        current.SetCrossedOut(true);
        const auto finalPop = stack.Pop(current);
        VERIFY_IS_TRUE(finalPop.IsCrossedOut());
    }

    TEST_METHOD(PushPopCrossedOut)
    {
        Log::Comment(L"Push/pop of CrossedOut attribute should work correctly.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetCrossedOut(true);

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::CrossedOut) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsCrossedOut());
    }

    TEST_METHOD(PushPopInvisible)
    {
        Log::Comment(L"Push/pop of Invisible attribute should work correctly.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetInvisible(true);

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Invisible) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsInvisible());
    }

    TEST_METHOD(PushPopNegativeReverse)
    {
        Log::Comment(L"Push/pop of Negative (reverse video) attribute should work correctly.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetReverseVideo(true);

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Negative) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsReverseVideo());
    }

    TEST_METHOD(PushPopBlink)
    {
        Log::Comment(L"Push/pop of Blink attribute should work correctly.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetBlinking(true);

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Blink) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsBlinking());
    }

    TEST_METHOD(PushPopFaintness)
    {
        Log::Comment(L"Push/pop of Faintness attribute should work correctly.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetFaint(true);

        VTParameter params[] = { VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::Faintness) } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        const auto restored = stack.Pop(currentAttr);

        VERIFY_IS_TRUE(restored.IsFaint());
    }

    TEST_METHOD(PushWithUnsupportedOptionIsEffectivelyEmptyPush)
    {
        Log::Comment(L"Pushing with out-of-range option should effectively be an empty push.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetIntense(true);

        // Use a value that's out of the valid range (0 is All, which is handled separately)
        // Option 0 is "All" which is only set when options are empty.
        // If we specify option 0 explicitly, it won't match the > All && <= Max check,
        // so nothing gets saved selectively.
        VTParameter params[] = { VTParameter{ 0 } };
        VTParameters options{ params, 1 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;
        currentAttr.SetItalic(true);

        // Pop should return current attributes since the push saved nothing meaningful
        const auto restored = stack.Pop(currentAttr);
        VERIFY_IS_TRUE(restored.IsItalic());
        VERIFY_IS_FALSE(restored.IsIntense());
    }

    TEST_METHOD(PushPopBothFgAndBgColors)
    {
        Log::Comment(L"Push with both foreground and background color options should save both.");

        SgrStack stack;

        TextAttribute savedAttr;
        savedAttr.SetForeground(COLORREF(0xFF0000));
        savedAttr.SetBackground(COLORREF(0x00FF00));
        savedAttr.SetIntense(true);

        VTParameter params[] = {
            VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::SaveForegroundColor) },
            VTParameter{ static_cast<VTInt>(SgrSaveRestoreStackOptions::SaveBackgroundColor) }
        };
        VTParameters options{ params, 2 };
        stack.Push(savedAttr, options);

        TextAttribute currentAttr;

        const auto restored = stack.Pop(currentAttr);

        // Both colors should be restored
        VERIFY_ARE_EQUAL(savedAttr.GetForeground(), restored.GetForeground());
        VERIFY_ARE_EQUAL(savedAttr.GetBackground(), restored.GetBackground());
        // Intense was NOT saved selectively, should remain as current (false)
        VERIFY_IS_FALSE(restored.IsIntense());
    }
};
