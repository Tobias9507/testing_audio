/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

struct MIDIDeviceConnection  : public PhysicalTopologySource::DeviceConnection,
                               public juce::MidiInputCallback
{
    MIDIDeviceConnection() {}

    ~MIDIDeviceConnection()
    {
        JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

        listeners.call ([this] (Listener& l) { l.connectionBeingDeleted (*this); });

        if (midiInput != nullptr)
            midiInput->stop();

        if (interprocessLock != nullptr)
            interprocessLock->exit();
    }

    bool lockAgainstOtherProcesses (const String& midiInName, const String& midiOutName)
    {
        interprocessLock.reset (new juce::InterProcessLock ("blocks_sdk_"
                                                            + File::createLegalFileName (midiInName)
                                                            + "_" + File::createLegalFileName (midiOutName)));
        if (interprocessLock->enter (500))
            return true;

        interprocessLock = nullptr;
        return false;
    }

    struct Listener
    {
        virtual ~Listener() {}

        virtual void handleIncomingMidiMessage (const juce::MidiMessage& message) = 0;
        virtual void connectionBeingDeleted (const MIDIDeviceConnection&) = 0;
    };

    void addListener (Listener* l)
    {
        juce::ScopedLock scopedLock (criticalSecton);
        listeners.add (l);
    }

    void removeListener (Listener* l)
    {
        juce::ScopedLock scopedLock (criticalSecton);
        listeners.remove (l);
    }

    bool sendMessageToDevice (const void* data, size_t dataSize) override
    {
        JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED // This method must only be called from the message thread!

        jassert (dataSize > sizeof (BlocksProtocol::roliSysexHeader) + 2);
        jassert (memcmp (data, BlocksProtocol::roliSysexHeader, sizeof (BlocksProtocol::roliSysexHeader) - 1) == 0);
        jassert (static_cast<const uint8*> (data)[dataSize - 1] == 0xf7);

        if (midiOutput != nullptr)
        {
            midiOutput->sendMessageNow (juce::MidiMessage (data, (int) dataSize));
            return true;
        }

        return false;
    }

    void handleIncomingMidiMessage (juce::MidiInput*, const juce::MidiMessage& message) override
    {
        juce::ScopedTryLock lock (criticalSecton);

        if (lock.isLocked())
        {
            const auto data = message.getRawData();
            const int dataSize = message.getRawDataSize();
            const int bodySize = dataSize - (int) (sizeof (BlocksProtocol::roliSysexHeader) + 1);

            if (bodySize > 0 && memcmp (data, BlocksProtocol::roliSysexHeader, sizeof (BlocksProtocol::roliSysexHeader)) == 0)
                if (handleMessageFromDevice != nullptr)
                    handleMessageFromDevice (data + sizeof (BlocksProtocol::roliSysexHeader), (size_t) bodySize);

            listeners.call ([&] (Listener& l) { l.handleIncomingMidiMessage (message); });
        }
    }

    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;

    juce::CriticalSection criticalSecton;

private:
    juce::ListenerList<Listener> listeners;
    std::unique_ptr<juce::InterProcessLock> interprocessLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIDeviceConnection)
};

} // namespace juce
