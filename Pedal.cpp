#include "daisy_petal.h"
#include "daisysp.h"
#include "Terrarium.h"
#include "Delay.h"
#include "ToneFilter.h"
#include "TapTempo.h"

using namespace daisy;
using namespace daisysp;
using namespace terrarium;




DaisyPetal hw;
Delay delay;
Balance balance;
TapTempo tapTempo;
ToneFilter tone(48000.f);
static CrossFade crossFade;
DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delayMems[4];

//DelayLineReverse<float, MAX_DELAY> DSY_SDRAM_BSS delayMems[4];

Parameter toneParam;
Parameter bpmParam;

bool bypass = true;
float dryWet; 
float feedback;
float toneVal;
float bpm;
float color;



/**
 * @brief Initialize special parameters for the terrarium pedal knobs
 * (Mostly just setting min/max values)
 */
void initParams();

/**
 * @brief Processes all controls for the terrarium pedal
 * 
 */
void processControls();

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();
	processControls();
	for (size_t i = 0; i < size; i++)
	{
		if(bypass)
			out[0][i] = in[0][i];
		else
		{
			// Needed for crossFade process
			float nonConstInput = in[0][i];
			float finalMix = 0;
			float allDelaySignals = delay.process(in[0][i]);
			float preFilterDelays = allDelaySignals;

			allDelaySignals = tone.process(allDelaySignals);
			
            //allDelaySignals = balance.Process(allDelaySignals,preFilterDelays*tone.getFactor());

			finalMix = crossFade.Process(nonConstInput, allDelaySignals);


			out[0][i] = finalMix;
			
		}
			
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();

	delay.initDelay(delayMems);
	//delay.initDelayReverse(delayMems);
	delay.setBPM(90);
	crossFade.Init();
	crossFade.SetCurve(CROSSFADE_CPOW);
	balance.Init(hw.AudioSampleRate());
	initParams();

	hw.StartAudio(AudioCallback);
	while(1) {}
}

void initParams()
{
	toneParam.Init(hw.knob[Terrarium::KNOB_3], -1.0f, 1.0f, Parameter::LINEAR);
	bpmParam.Init(hw.knob[Terrarium::KNOB_4], 30.0f, 240.0f, Parameter::LINEAR);

}

void processControls()
{
	bool tapping = false;

	/***************PROCESS FOOTSWITCHES*****************/
	if(hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
		bypass = !bypass;
	if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
		tapping = !tapping;

	tapTempo.update(tapping);
	delay.setBPM(tapTempo.getBPM());

	/***************************************************/

	/***************PROCESS SWITCHES*****************/
	if(hw.switches[Terrarium::SWITCH_1].Pressed())
		delay.enableHead(0);
	else
		delay.disableHead(0);

	if(hw.switches[Terrarium::SWITCH_2].Pressed())
		delay.enableHead(1);
	else
		delay.disableHead(1);

	if(hw.switches[Terrarium::SWITCH_3].Pressed())
		delay.enableHead(2);
	else
		delay.disableHead(2);	

	if(hw.switches[Terrarium::SWITCH_4].Pressed())
		delay.enableHead(3);
	else
		delay.disableHead(3);
	/************************************************/

	/***************PROCESS KNOBS*****************/
	dryWet = hw.knob[Terrarium::KNOB_1].Process();
	crossFade.SetPos(dryWet);

	feedback = hw.knob[Terrarium::KNOB_2].Process();
	delay.setFeedback(feedback);

	toneVal = toneParam.Process();
	tone.setFreq(toneVal);

	float bpmFromKnob = bpmParam.Process();

	// Only update the delay time from the knob if it falls within + or - the current bpm to prevent extreme jumps in bpm. 
    if( bpmFromKnob > tapTempo.getBPM() - 1 &&  bpmFromKnob < tapTempo.getBPM() + 1 )
    {    
		tapTempo.setBPM(bpmFromKnob);
		delay.setBPM(bpm);
	}

	color = hw.knob[Terrarium::KNOB_5].Process();
	
	/*********************************************/

	bpm = tapTempo.getBPM();
}