#pragma once

/** */
class CodeEditorDemo final : public DemoBase
{
public:
    /** */
    CodeEditorDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Code Editor Demo"))
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodeEditorDemo)
};
