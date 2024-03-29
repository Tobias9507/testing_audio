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

//==============================================================================
/**
    Utility class for linearly smoothed values like volume etc. that should
    not change abruptly but as a linear ramp, to avoid audio glitches.

    @tags{Audio}
*/
template <typename FloatType>
class LinearSmoothedValue
{
public:
    /** Constructor. */
    LinearSmoothedValue() = default;

    /** Constructor. */
    LinearSmoothedValue (FloatType initialValue) noexcept
        : currentValue (initialValue), target (initialValue)
    {
    }

    //==============================================================================
    /** Reset to a new sample rate and ramp length.
        @param sampleRate The sampling rate
        @param rampLengthInSeconds The duration of the ramp in seconds
    */
    void reset (double sampleRate, double rampLengthInSeconds) noexcept
    {
        jassert (sampleRate > 0 && rampLengthInSeconds >= 0);
        stepsToTarget = (int) std::floor (rampLengthInSeconds * sampleRate);
        currentValue = target;
        countdown = 0;
    }

    //==============================================================================
    /** Set a new target value.

        @param newValue     The new target value
        @param force        If true, the value will be set immediately, bypassing the ramp
    */
    void setValue (FloatType newValue, bool force = false) noexcept
    {
        if (force)
        {
            target = currentValue = newValue;
            countdown = 0;
            return;
        }

        if (target != newValue)
        {
            target = newValue;
            countdown = stepsToTarget;

            if (countdown <= 0)
                currentValue = target;
            else
                step = (target - currentValue) / (FloatType) countdown;
        }
    }

    //==============================================================================
    /** Compute the next value.
        @returns Smoothed value
    */
    FloatType getNextValue() noexcept
    {
        if (countdown <= 0)
            return target;

        --countdown;
        currentValue += step;
        return currentValue;
    }

    /** Returns true if the current value is currently being interpolated. */
    bool isSmoothing() const noexcept
    {
        return countdown > 0;
    }

    /** Returns the target value towards which the smoothed value is currently moving. */
    FloatType getTargetValue() const noexcept
    {
        return target;
    }

    //==============================================================================
    /** Applies a linear smoothed gain to a stream of samples
        S[i] *= gain
        @param samples Pointer to a raw array of samples
        @param numSamples Length of array of samples
    */
    void applyGain (FloatType* samples, int numSamples) noexcept
    {
        jassert(numSamples >= 0);

        if (isSmoothing())
        {
            for (int i = 0; i < numSamples; i++)
                samples[i] *= getNextValue();
        }
        else
        {
            FloatVectorOperations::multiply (samples, target, numSamples);
        }
    }

    //==============================================================================
    /** Computes output as linear smoothed gain applied to a stream of samples.
        Sout[i] = Sin[i] * gain
        @param samplesOut A pointer to a raw array of output samples
        @param samplesIn  A pointer to a raw array of input samples
        @param numSamples The length of the array of samples
    */
    void applyGain (FloatType* samplesOut, const FloatType* samplesIn, int numSamples) noexcept
    {
        jassert (numSamples >= 0);

        if (isSmoothing())
        {
            for (int i = 0; i < numSamples; i++)
                samplesOut[i] = samplesIn[i] * getNextValue();
        }
        else
        {
            FloatVectorOperations::multiply (samplesOut, samplesIn, target, numSamples);
        }
    }

    //==============================================================================
    /** Applies a linear smoothed gain to a buffer */
    void applyGain (AudioBuffer<FloatType>& buffer, int numSamples) noexcept
    {
        jassert (numSamples >= 0);

        if (isSmoothing())
        {
            if (buffer.getNumChannels() == 1)
            {
                auto samples = buffer.getWritePointer(0);

                for (int i = 0; i < numSamples; ++i)
                    samples[i] *= getNextValue();
            }
            else
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    auto gain = getNextValue();

                    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
                        buffer.setSample (channel, i, buffer.getSample (channel, i) * gain);
                }
            }
        }
        else
        {
            buffer.applyGain (0, numSamples, target);
        }
    }

    //==============================================================================
    /** Skip the next numSamples samples.
        This is identical to calling getNextValue numSamples times. It returns
        the new current value.
        @see getNextValue
    */
    FloatType skip (int numSamples) noexcept
    {
        if (numSamples >= countdown)
        {
            currentValue = target;
            countdown = 0;
            return target;
        }

        currentValue += (step * static_cast<FloatType> (numSamples));
        countdown -= numSamples;
        return currentValue;
    }

private:
    //==============================================================================
    FloatType currentValue = 0, target = 0, step = 0;
    int countdown = 0, stepsToTarget = 0;
};

} // namespace juce
