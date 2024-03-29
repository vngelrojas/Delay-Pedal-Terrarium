#include "daisysp.h"
#include "daisy_seed.h"
#include "delayline_reverse.h"

#pragma once
using namespace daisysp;
using namespace daisy;
using namespace daisy::seed;

#define MAX_DELAY static_cast<size_t>(48000 * 2.f)  // Max delay of 2 seconds which is 30 bpm
const int NUM_OF_DELAY_HEADS = 4;                   // # of delay heads, simply change this number if you want more or less delay heads



class Delay
{
    private:
        struct DelayHead
        {
            DelayLine<float, MAX_DELAY> *delay; // Will point to a delayMem
            DelayLineReverse<float, MAX_DELAY> *delayReverse; // Will point to a delayMem
            float currentDelay;                 // The current delay 
            float delayTarget;                  // The delay target that currentDelay will ramp up/down to 
            float feedback;                     // Feedback level of the delay
            float modulation = 0;                   // Modulation level of the delay
            float pos = 0;
            float process(float in)
            {

                float readSample; // Will store a sample from the delay line

                // This smoothes out the delay when you turn the delay control?
                fonepole(currentDelay, delayTarget, .0002f); 
                // Set delay time
                delay->SetDelay(currentDelay + modulation);     
                 
                readSample = delay->Read(); // Read in the next sample from the delay line

                // Write the readSample * the feedback amount + the input sample into the delay line
                delay->Write((feedback * readSample) + in); 
                
                return readSample;
            }

            float processReverse(float in)
            {
                float readSample;

                fonepole(currentDelay, delayTarget, .0002f); 

                delayReverse->SetDelay1(currentDelay);

                readSample = delayReverse->ReadRev();

                delayReverse->Write((feedback * readSample) + in);

                return readSample;

            }

            float readSampleTwiceAsFast()
            {
                // Assuming 'pos' is the current position in the delay line
                float readSample = delay->Read(currentDelay + pos);
                pos += 2; // Move two samples ahead
                if (pos >= MAX_DELAY) // If we've reached the end of the delay line
                {
                    pos = 0; // Wrap around to the start
                }
                return readSample;
            }
        };
    public:
        /**
         * @brief Constructs the delay heads
         */
        Delay();

        /**
         * @brief Initializes the delay heads
         * 
         * @param delayMems Delay lines that will be used by the delay heads
         */
        void initDelay(DelayLine<float, MAX_DELAY> delayMems[4]);

        /**
         * @brief Initializes the reverse delay heads
         * 
         * @param delayMems Delay lines that will be used by the delay heads
         */
        void initDelayReverse(DelayLineReverse<float, MAX_DELAY> delayMems[4]);

        /**
         * @brief Stops all the delays from playing but does not delete their sound, 
         * meaning if you toggle a head, it will keep playing previous input
         * 
         */
        void stopAll();

        /**
         * @brief Sets the bpm for delay
         * 
         * @param bpm The new bpm for delay
         */
        void setBPM(float bpm);

        /**
         * @brief Set the Feedback of delay
         * 
         * @param feed The new feedback for delay
         */
        void setFeedback(const float& feed);

        /**
         * @brief Will proccess all delays using delayline class from DaisySP
         * 
         * @param in Current audio input
         * @return Summation of all processed delay signals
         */
        float process(float in);

        /**
         * @brief Will toggle on/off a single delay head
         * 
         * @param headNumber The delay head to toggle on/off
         */
        void toggleHead(const int& headNum);
        
        /**
         * @brief Enable a single delay head
         * 
         * @param headNum Delay head to enable
         */
        void enableHead(const int& headNum);
        
        /**
         * @brief Disable a single delay head and clears its delays
         * 
         * @param headNum Delay head to Disable
         */
        void disableHead(const int& headNum);
        
        /**
         * @brief Get the current bpm of delay
         * 
         * @return BPM 
         */
        float getBPM();
        
        /**
         * @brief Clears the delay signals 
         * 
         */
        void clear();

        /**
         * @brief Sets the modulation of the delay
         * 
         * @param mod The new modulation of the delay
         */
        void setModulation(const float& mod);
        /**
         * @brief Set the Reverse flag
         * 
         * @param reverse 
         */
        void setReverse(bool reverse);
        
    private:
        bool delayHeadOn[NUM_OF_DELAY_HEADS]; // Each delay head will be turned on/off independently
        float bpm;                            // The bpm being used being used by all heads, might be useful if you add other features that need bpm
        float feedback;                       // The feedback of delays
        float modulation;                     // The modulation of delays
        bool proccessReverse;
    public:
        DelayHead delayHeads[NUM_OF_DELAY_HEADS]; // Array of however many delays you want 

    
};