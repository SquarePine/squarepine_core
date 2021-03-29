ProgramAudioProcessorEditor::ProgramAudioProcessorEditor (AudioProcessor& p) :
    AudioProcessorEditor (p)
{
    setOpaque (true);

    Array<PropertyComponent*> programs;

    auto numPrograms = p.getNumPrograms();
    int totalHeight = 0;

    for (int i = 0; i < numPrograms; ++i)
    {
        auto name = p.getProgramName (i).trim();

        if (name.isEmpty())
            name = "Unnamed";

        auto* pc = new PropertyComp (name, p);
        programs.add (pc);
        totalHeight += pc->getPreferredHeight();
    }

    panel.addProperties (programs);

    addAndMakeVisible (panel);
    setSize (400, std::clamp (totalHeight, 25, 400));
}

void ProgramAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
}

void ProgramAudioProcessorEditor::resized()
{
    panel.setBounds (getLocalBounds());
}
