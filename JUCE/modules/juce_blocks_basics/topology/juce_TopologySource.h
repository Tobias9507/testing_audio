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

/** Base class for an entity that provides access to a blocks topology.

    @tags{Blocks}
*/
class TopologySource
{
public:
    //==========================================================================
    /** Destructor. */
    virtual ~TopologySource() = default;

    /** Returns the current topology that this object manages. */
    virtual BlockTopology getCurrentTopology() const = 0;

    /** Sets the TopologySource as active, occupying the midi port and trying to connect to the block devices */
    virtual void setActive (bool shouldBeActive) = 0;

    /** Returns true, if the TopologySource is currently trying to connect the block devices */
    virtual bool isActive() const = 0;

    //==========================================================================
    /** Used to receive callbacks for topology changes */
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void topologyChanged() = 0;
    };

    void addListener (Listener* l)       { listeners.add (l); }
    void removeListener (Listener* l)    { listeners.remove (l); }

    /** Invoke this to force touches-off on all physical devices. */
    virtual void cancelAllActiveTouches() noexcept {}

protected:
    //==========================================================================
    juce::ListenerList<Listener> listeners;
};

} // namespace juce
