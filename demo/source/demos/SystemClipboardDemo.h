/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SystemClipboardDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Edit and display the contents of the system's clipboard.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics
 exporters:        xcode_mac, vs2022, linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        SystemClipboardDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"

//==============================================================================
class SystemClipboardDemo final : public Component,
                                  private Timer
{
public:
    SystemClipboardDemo()
    {
        typeDetails.setColour (Label::ColourIds::textColourId,
                               getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::defaultText,
                                                       Colours::white));
        updateText();
        addAndMakeVisible (typeDetails);

        setOpaque (true);
        setSize (750, 650);
        startTimer (2000);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground,
                                           Colour::greyLevel (0.8f)));
    }

    void resized() override
    {
        const auto marginPx = 8;

        auto b = getLocalBounds().reduced (marginPx);

        typeDetails.setBounds (b.removeFromTop (64));

        if (dataViewer != nullptr)
        {
            b.removeFromTop (marginPx);
            dataViewer->setBounds (b);
        }
    }

    void timerCallback() override
    {
        if (const auto type = SystemClipboard::getDataType(); lastKnownType != type)
        {
            lastKnownType = type;
            updateText();
        }
    }

private:
    Label typeDetails;
    std::unique_ptr<Component> dataViewer;
    SystemClipboard::Type lastKnownType = SystemClipboard::Type::unknown;

    static String toString (SystemClipboard::Type type)
    {
        switch (type)
        {
            case SystemClipboard::Type::empty:      return TRANS ("(empty)");
            case SystemClipboard::Type::unknown:    return TRANS ("(unknown)");
            case SystemClipboard::Type::text:       return TRANS ("Text");
            case SystemClipboard::Type::image:      return TRANS ("Image");
            case SystemClipboard::Type::audioPCM:   return TRANS ("Audio (PCM)");

            default:
            break;
        };

        jassertfalse; // New type?
        return {};
    }

    void updateText()
    {
        const auto typeString = TRANS ("Type: {}")
                                    .replace ("{}", toString (lastKnownType));
        typeDetails.setText (typeString, sendNotification);

        switch (lastKnownType)
        {
            case SystemClipboard::Type::text:
            {
                auto te = std::make_unique<TextEditor>();
                te->setMultiLine (true);
                te->setText (SystemClipboard::getTextFromClipboard());
                dataViewer = std::move (te);
            }
            break;

            case SystemClipboard::Type::image:
            {
                auto ic = std::make_unique<ImageComponent>();
                ic->setImage (SystemClipboard::getImageFromClipboard(),
                              RectanglePlacement::onlyReduceInSize);
                dataViewer = std::move (ic);
            }
            break;

            case SystemClipboard::Type::audioPCM:
            {
                auto te = std::make_unique<TextEditor>();
                te->setText ("audio");
                dataViewer = std::move (te);
            }
            break;

            default:
                dataViewer.reset();
            break;
        };

        if (dataViewer != nullptr)
        {
            addAndMakeVisible (dataViewer.get());
            resized();
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemClipboardDemo)
};
