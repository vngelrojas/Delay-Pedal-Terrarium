/*
 * TapTempo.cpp
 * An Arduino library that times consecutive button presses to calculate Beats Per Minute. Corrects for missed beats and resets phase with single taps. 
 *
 * Copyright (c) 2016 Damien Clarke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.  
 */


#include "TapTempo.h"


void TapTempo::setSkippedTapThresholdLow(float threshold)
{
  if(threshold > 1.0 && threshold < 2.0)
    skippedTapThresholdLow = threshold;
}

void TapTempo::setSkippedTapThresholdHigh(float threshold)
{
  if(threshold > 2.0 && threshold < 4.0)
    skippedTapThresholdHigh = threshold;
}

float TapTempo::getBPM()
{
  return 60000.0 / beatLengthMS;
}

void TapTempo::setBPM(float bpm)
{
  beatLengthMS = 60000 / bpm;
}

bool TapTempo::onBeat()
{
  return fmod((float)millisSinceReset, (float)beatLengthMS) < fmod((float)millisSinceResetOld, (float)beatLengthMS);
}

bool TapTempo::isChainActive()
{
  return isChainActive(daisy::System::GetNow());
}

bool TapTempo::isChainActive(unsigned long ms)
{
  return lastTapMS + maxBeatLengthMS > ms && lastTapMS + (beatLengthMS * beatsUntilChainReset) > ms;
}

float TapTempo::beatProgress()
{
  return fmod((float)millisSinceReset / (float)beatLengthMS, 1.0);
}

void TapTempo::update(bool buttonDown)
{
  unsigned long ms = daisy::System::GetNow();

  // if a tap has occured...
  if(buttonDown && !buttonDownOld)
    tap(ms);

  buttonDownOld = buttonDown;
  millisSinceResetOld = millisSinceReset;
  millisSinceReset = ms - lastResetMS;
}

void TapTempo::tap(unsigned long ms)
{
  // start a new tap chain if last tap was over an amount of beats ago
  if(!isChainActive(ms))
    resetTapChain(ms);

  addTapToChain(ms);
}

void TapTempo::addTapToChain(unsigned long ms)
{
  // get time since last tap
  unsigned long duration = ms - lastTapMS;

  // reset beat to occur right now
  lastTapMS = ms;

  tapsInChain++;
  if(tapsInChain == 1)
    return;
  
  // detect if last duration was approximately twice the length of the current beat length
  // and if so then we've simply missed a beat and can halve the duration to get the real beat length
  if(skippedTapDetection
     && tapsInChain > 2
     && !lastTapSkipped
     && duration > beatLengthMS * skippedTapThresholdLow
     && duration < beatLengthMS * skippedTapThresholdHigh)
  {
    duration = duration >> 1;
    lastTapSkipped = true;
  }
  else
  {
    lastTapSkipped = false;
  }
  
  tapDurations[tapDurationIndex] = duration;
  tapDurationIndex++;
  if(tapDurationIndex == totalTapValues) {
    tapDurationIndex = 0;
  }
  
  beatLengthMS = getAverageTapDuration();
}

void TapTempo::resetTapChain()
{
  resetTapChain(daisy::System::GetNow());
}

void TapTempo::resetTapChain(unsigned long ms)
{
  tapsInChain = 0;
  tapDurationIndex = 0;
  lastResetMS = ms;
  for(int i = 0; i < totalTapValues; i++) {
    tapDurations[i] = 0;
  }
}

unsigned long TapTempo::getAverageTapDuration()
{
  int amount = tapsInChain - 1;
  if(amount > totalTapValues)
    amount = totalTapValues;
  
  unsigned long runningTotalMS = 0;
  for(int i = 0; i < amount; i++) {
    runningTotalMS += tapDurations[i];
  }
  unsigned long avgTapDurationMS = runningTotalMS / amount;
  if(avgTapDurationMS < minBeatLengthMS) {
    return minBeatLengthMS;
  }
  return avgTapDurationMS;
}

void TapTempo::setBeatsUntilChainReset(int beats)
{
  if(beats < 2)
  {
    beats = 2;
  }
  beatsUntilChainReset = beats;
}

void TapTempo::setTotalTapValues(int total)
{
  if(total < 2)
  {
    total = 2;
  }
  else if(total > TapTempo::MAX_TAP_VALUES)
  {
    total = TapTempo::MAX_TAP_VALUES;
  }
  totalTapValues = total;
}
