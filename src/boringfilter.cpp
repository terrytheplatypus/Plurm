#include "plugin.hpp"
#include <dsp/fft.hpp>


struct Boringfilter : Module {
	enum ParamId {
		FILTER_KNOB_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	//256 length recording buffer to start with 
	//(yes this is really shitty latency for a filter)
	//keep track number of outputs since last fft
	//have to have both output buffer and last fft frame

	//if the window size changes this SHOULD NOT be static
	const static int WINDOW_SIZE = 256;
	const static int HOP_SIZE = WINDOW_SIZE/4;
	const static int SAMPLERATE= 44100;

	float window [WINDOW_SIZE];
	float * inBuffer = new float [WINDOW_SIZE];
	float outBuffer [WINDOW_SIZE];
	float lastIFFTFrame [WINDOW_SIZE];
	bool initialized = false;
	int windowIndex = -1;
	int hopIndex = -1;
	rack::dsp::RealFFT * realFftEngine;

	Boringfilter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		//assume sampling rate is 44100 (yeah whatever)
		configParam(FILTER_KNOB_PARAM, 0.f, SAMPLERATE/2, SAMPLERATE/4, "Hz");
		// configInput(IN_INPUT, "");
		// configOutput(OUT_OUTPUT, "");

		realFftEngine = new rack::dsp::RealFFT(WINDOW_SIZE);

		for (int k = 0; k < WINDOW_SIZE; k++) {
                window[k] = -.5 * cos(2.*M_PI*(float)k/(float)WINDOW_SIZE) + .5; // Hamming window 
                //window[k] = .5 * (1. - cos(2.*M_PI*(float)k/(float)sizenumValues));  // Hann window
				inBuffer[k] = 0;
				outBuffer[k] = 0;
				lastIFFTFrame[k] = 0;
				
        }
	}

	void process(const ProcessArgs& args) override {
		if(windowIndex == WINDOW_SIZE - 1) {
			initialized = true;
		}
		windowIndex = (windowIndex + 1) % WINDOW_SIZE;

		//shift the whole buffer left

		for(int n = 0; n < WINDOW_SIZE - 1; n++) {
			outBuffer[n] = outBuffer[n + 1];
		}		
		//wait to do fft until buffer fills for the first time
		if(initialized) {

			hopIndex = (hopIndex + 1) % HOP_SIZE;
			if(hopIndex == 0) {

				float * tempFftBuf = new float[WINDOW_SIZE*2];
				//do fft
				//rfft is bad performance compared to unordered, maybe optimize later
				realFftEngine -> rfft(inBuffer, tempFftBuf);
				
				//convert knob freq value to buffer bucket position (bufsize/(samplerate/freq))
				int bucketPos = WINDOW_SIZE/(SAMPLERATE/params[FILTER_KNOB_PARAM].getValue());

				for(int n = bucketPos; n < WINDOW_SIZE; n ++) {
					//set both real and imaginary parts to 0
					tempFftBuf[n] = 0;
					tempFftBuf[n+1] = 0;
				}
				//put ifft in output buffer with window
				realFftEngine->irfft(tempFftBuf, lastIFFTFrame);
				for(int n = 0; n < WINDOW_SIZE; n ++) { 
					outBuffer[n] += lastIFFTFrame[n]*window[n];
				}

				//then count until 1/4 of the window size, when that's reached, do the same as before

				free(tempFftBuf);
			}
		}
		//initially put input into recording buffer without outputting anything
		//need to do this after fft
		inBuffer[windowIndex] = inputs[IN_INPUT].getVoltage();
		outputs[OUT_OUTPUT].setVoltage(outBuffer[0]);
		
	}
};


struct BoringfilterWidget : ModuleWidget {
	BoringfilterWidget(Boringfilter* module) {
		setModule(module);
		// setPanel(createPanel(asset::plugin(pluginInstance, "res/boringfilter.svg")));
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/boringfilter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.511, 45.703)), module, Boringfilter::FILTER_KNOB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.354, 98.622)), module, Boringfilter::IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(28.063, 98.622)), module, Boringfilter::OUT_OUTPUT));
	}
};


Model* modelBoringfilter = createModel<Boringfilter, BoringfilterWidget>("boringfilter");