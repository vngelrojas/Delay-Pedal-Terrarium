#include "Delay.h"


void Delay::initDelay(DelayLine<float, MAX_DELAY> delayMems[4])
{
    for (int i = 0; i < 4; i++)
    {
        delayMems[i].Init();
        delayHeads[i].delay = &delayMems[i];
    }
}

void Delay::initDelayReverse(DelayLineReverse<float, MAX_DELAY> delayMems[4])
{
    for (int i = 0; i < 4; i++)
    {
        delayMems[i].Init();
        delayHeads[i].delayReverse = &delayMems[i];
    }
}

Delay::Delay()
{
    //delayMems = temp;
    for(int i = 0; i < 4; i++)
    {
        // Init the feedback
        delayHeads[i].feedback = 0; 
        // Make sure they are all off
        delayHeadOn[i] = false;
    }
    bpm = 20;

}

void Delay::stopAll()
{
    for(int i = 0;i < NUM_OF_DELAY_HEADS;i++)
        delayHeadOn[i] = false;
}

void Delay::setBPM(float bpm)
{
    this->bpm = bpm;
}

void Delay::setFeedback(const float& feed)                           
{
    
    this->feedback = feed;

}

float Delay::process(float in)
{


        float allDelaySignals = 0;

        for (int i = 0; i < NUM_OF_DELAY_HEADS; i++)
        {
            delayHeads[i].feedback = this->feedback;
            delayHeads[i].modulation = this->modulation;
            //The (i+0.25-i*0.75) just sets the delay intervals to (1/16 note, 1/8 note, dotted 1/8 note, 1/4 note - or 0.25,0.5,0.75,1) for i=0,1,2,3
            delayHeads[i].delayTarget = (i+0.25-i*0.75)* (48000*(60/bpm));
            if(delayHeadOn[i])
                allDelaySignals += delayHeads[i].process(in);
        }

        return allDelaySignals;
    //}
}

void Delay::toggleHead(const int& headNum)
{
    if(headNum >= NUM_OF_DELAY_HEADS || headNum < 0)
        return;
    else
        delayHeadOn[headNum] = !delayHeadOn[headNum];
}

void Delay::enableHead(const int& headNum)
{
    if(headNum >= NUM_OF_DELAY_HEADS || headNum < 0)
        return;
    else
        delayHeadOn[headNum] = true;
}

void Delay::disableHead(const int& headNum)
{
    if(headNum >= NUM_OF_DELAY_HEADS || headNum < 0)
        return;
    else
    {
        delayHeadOn[headNum] = false;
        delayHeads[headNum].feedback = 0;
        delayHeads[headNum].process(0);
    }
}

void Delay::clear()
{
    
    for (int i = 0; i < NUM_OF_DELAY_HEADS; i++)
    {
        feedback = 0;
        delayHeads[i].feedback = 0;
        delayHeads[i].process(0);
    }
    
    
}
void  Delay::setModulation(const float& mod)
{
    modulation = mod;
}
float Delay::getBPM()
{
    return bpm;
}