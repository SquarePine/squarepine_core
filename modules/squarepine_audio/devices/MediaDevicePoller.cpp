MediaDevicePoller::MediaDevicePoller (AudioDeviceManager& audioDeviceManager) :
    deviceManager (audioDeviceManager)
{
    if (auto* type = deviceManager.getCurrentDeviceTypeObject())
        driverAPI = type->getTypeName();

    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        lastNumInputs = numInputs = device->getInputChannelNames().size();
        lastNumOutputs = numOutputs = device->getOutputChannelNames().size();
    }

    setupDeviceLists();
    startTimer (2000);
}

MediaDevicePoller::~MediaDevicePoller()
{
    stopTimer();
    handleUpdateNowIfNeeded();
}

//==============================================================================
StringArray MediaDevicePoller::getListOfAudioDevices (bool giveMeInputDevices) const
{
    if (auto* currentDevice = deviceManager.getCurrentDeviceTypeObject())
    {
        currentDevice->scanForDevices();
        return currentDevice->getDeviceNames (giveMeInputDevices);
    }

    return {};
}

MediaDevicePoller::DeviceInfo MediaDevicePoller::getCurrentDeviceInfo (bool giveMeInputDevices) const
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        if (auto* type = deviceManager.getCurrentDeviceTypeObject())
        {
            DeviceInfo deviceInfo;
            deviceInfo.deviceName   = type->getDeviceNames (giveMeInputDevices)[type->getIndexOfDevice (device, giveMeInputDevices)];
            deviceInfo.channelInfo  = giveMeInputDevices ? device->getInputChannelNames() : device->getOutputChannelNames();
            const auto channels     = giveMeInputDevices ? device->getActiveInputChannels() : device->getActiveOutputChannels();

            //Mark the channels which are non-active and remove them from the list
            //so that we display only active channels.
            Array<int> deviceIndexesToBeRemoved;
            for (int i = 0; i < channels.getHighestBit(); ++i)
                if (! channels[i])
                    deviceIndexesToBeRemoved.add (i);

            for (int i = deviceIndexesToBeRemoved.size(); --i > 0;)
                deviceInfo.channelInfo.remove (deviceIndexesToBeRemoved[i]);

            return deviceInfo;
        }
    }

    return {};
}

StringArray MediaDevicePoller::getListOfChannelNames (bool giveMeInputDevices) const
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
        return giveMeInputDevices ? device->getInputChannelNames() : device->getOutputChannelNames();

    return {};
}

//==============================================================================
StringArray MediaDevicePoller::getListOfAudioInputDevices() const                   { return getListOfAudioDevices (true); }
StringArray MediaDevicePoller::getListOfAudioOutputDevices() const                  { return getListOfAudioDevices (false); }
Array<MidiDeviceInfo> MediaDevicePoller::getListOfMIDIInputDevices() const          { return MidiInput::getAvailableDevices(); }
Array<MidiDeviceInfo> MediaDevicePoller::getListOfMIDIOutputDevices() const         { return MidiOutput::getAvailableDevices(); }
StringArray MediaDevicePoller::getListOfInputChannelNames() const                   { return getListOfChannelNames (true); }
StringArray MediaDevicePoller::getListOfOutputChannelNames() const                  { return getListOfChannelNames (false); }
MediaDevicePoller::DeviceInfo MediaDevicePoller::getCurrentInputDeviceInfo() const  { return getCurrentDeviceInfo (true); }
MediaDevicePoller::DeviceInfo MediaDevicePoller::getCurrentOutputDeviceInfo() const { return getCurrentDeviceInfo (false); }

void MediaDevicePoller::setupDeviceLists()
{
    currentAudioInputDevices    = getListOfAudioInputDevices();
    currentAudioOutputDevices   = getListOfAudioOutputDevices();
    currentMidiInputDevices     = getListOfMIDIInputDevices();
    currentMidiOutputDevices    = getListOfMIDIOutputDevices();
}

//==============================================================================
void MediaDevicePoller::addListener (Listener* listener)    { listeners.add (listener); }
void MediaDevicePoller::removeListener (Listener* listener) { listeners.remove (listener); }

void MediaDevicePoller::handleAsyncUpdate()
{
    switch (changeType)
    {
        case ChangeType::driverChanged:
        {
            lastAPI = driverAPI;
            listeners.call (&Listener::driverChanged, driverAPI);
        }
        break;

        case ChangeType::deviceAdded:
            listeners.call (&Listener::deviceAdded, lastDeviceChanged);
        break;

        case ChangeType::deviceRemoved:
            listeners.call (&Listener::deviceRemoved, lastDeviceChanged);
        break;

        case ChangeType::inputsChanged:
        {
            listeners.call (&Listener::numInputsChanged, numInputs, numInputs > lastNumInputs);
            lastNumInputs = numInputs;
        }
        break;

        case ChangeType::outputsChanged:
        {
            listeners.call (&Listener::numOutputsChanged, numOutputs, numOutputs > lastNumOutputs);
            lastNumOutputs = numOutputs;
        }
        break;

        case ChangeType::noChange:
        break;

        default:
            jassertfalse;
        break;
    };
}

//==============================================================================
template<typename ArrayType>
bool MediaDevicePoller::wasDeviceAdded (const ArrayType& newList, const ArrayType& oldList)
{
    return newList.size() > oldList.size();
}

String MediaDevicePoller::getChangedDeviceName (const StringArray& newList, const StringArray& oldList)
{
    for (const auto& s : newList)
        if (! oldList.contains (s))
            return s;

    return newList.isEmpty()
            ? oldList.strings.getFirst()
            : newList.strings.getFirst();
}

String MediaDevicePoller::getChangedDeviceName (const Array<MidiDeviceInfo>& newList, const Array<MidiDeviceInfo>& oldList)
{
    for (const auto& s : newList)
        if (! oldList.contains (s))
            return s.name;

    return newList.isEmpty()
            ? oldList.getFirst().name
            : newList.getFirst().name;
}

template<typename ArrayType>
void MediaDevicePoller::checkForDeviceChange (const ArrayType& arrayNew, ArrayType& arrayOld)
{
    jassert (&arrayNew != &arrayOld);

    if (arrayOld != arrayNew)
    {
        changeType = wasDeviceAdded (arrayNew, arrayOld) ? ChangeType::deviceAdded : ChangeType::deviceRemoved;
        lastDeviceChanged = getChangedDeviceName (arrayNew, arrayOld);
        arrayOld = arrayNew;
    }
}

void MediaDevicePoller::timerCallback()
{
    if (auto* type = deviceManager.getCurrentDeviceTypeObject())
        driverAPI = type->getTypeName();

    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        numInputs = device->getInputChannelNames().size();
        numOutputs = device->getOutputChannelNames().size();
    }

    const auto audioInputDevices = getListOfAudioDevices (true);
    const auto audioOutputDevices = getListOfAudioDevices (false);
    const auto midiInputDevices = getListOfMIDIInputDevices();
    const auto midiOutputDevices = getListOfMIDIOutputDevices();

    changeType = ChangeType::noChange;

    //N.B.: These checks are done in order of importance.
    checkForDeviceChange (audioInputDevices, currentAudioInputDevices);
    checkForDeviceChange (audioOutputDevices, currentAudioOutputDevices);
    checkForDeviceChange (midiInputDevices, currentMidiInputDevices);
    checkForDeviceChange (midiOutputDevices, currentMidiOutputDevices);

    if (lastNumInputs != numInputs)     changeType = ChangeType::inputsChanged;
    if (lastNumOutputs != numOutputs)   changeType = ChangeType::outputsChanged;
    if (lastAPI != driverAPI)           changeType = ChangeType::driverChanged;

    if (changeType != ChangeType::noChange)
        triggerAsyncUpdate();
}
