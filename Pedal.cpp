#include "daisy_petal.h"
#include "daisysp.h"
#include "Terrarium.h"
#include "Delay.h"


using namespace daisy;
using namespace daisysp;
using namespace terrarium;


DaisyPetal hw;
Delay delay;

/**
 * @brief Processes all controls for the pedal
 * 
 */
void processControls();

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	while(1) {}
}


void processControls()
{
	if(hw.switches[Terrarium::SWITCH_1].Pressed())
	{
		//
	}
	else
	{

	}
}