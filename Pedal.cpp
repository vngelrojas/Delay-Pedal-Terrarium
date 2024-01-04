#include "daisy_petal.h"
#include "daisysp.h"
#include "Terrarium.h"
#include "Delay.h"


using namespace daisy;
using namespace daisysp;
using namespace terrarium;


DaisyPetal hw;
Delay delay;
DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delayMems[4];

bool bypass = true;


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
		
		out[0][i] = delay.process(in[0][i]);
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();

	delay.initDelay(delayMems);
	delay.setBPM(90);
	hw.StartAudio(AudioCallback);
	while(1) {}
}


void processControls()
{
	/***************PROCESS FOOTSWITCHES*****************/
	if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
		bypass = !bypass;

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

}