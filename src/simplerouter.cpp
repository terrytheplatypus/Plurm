#include "plugin.hpp"


struct Simplerouter : Module {
	enum ParamId {
		SWITCH_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_INPUT,
		RET1_INPUT,
		RET2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SEND1_OUTPUT,
		SEND2_OUTPUT,
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		SWITCH_LIGHT,
		LIGHTS_LEN
	};

	Simplerouter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SWITCH_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_INPUT, "");
		configInput(RET1_INPUT, "");
		configInput(RET2_INPUT, "");
		configOutput(SEND1_OUTPUT, "");
		configOutput(SEND2_OUTPUT, "");
		configOutput(OUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		/*
		if switch is off, fx1 is first, if on, fx2 is first.
		if fx1 is first, in is sent to send1, ret1 is sent to send2,
		ret2 is sent out.

		if fx2 is first, in is sent to send2, ret2 is sent to send1, ret1 is sent out.
		
		
		*/

		int numChannels = inputs[IN_INPUT].getChannels();
		outputs[SEND1_OUTPUT].setChannels(numChannels);
		outputs[SEND2_OUTPUT].setChannels(numChannels);
		outputs[OUT_OUTPUT].setChannels(numChannels);
		int switchVal = params[SWITCH_PARAM].getValue();
		for (int i = 0; i < numChannels; i++)
		{
			if(switchVal < 1) {
				outputs[SEND1_OUTPUT].setVoltage(inputs[IN_INPUT].getVoltage(i), i);
				outputs[SEND2_OUTPUT].setVoltage(inputs[RET1_INPUT].getVoltage(i), i);
				outputs[OUT_OUTPUT].setVoltage(inputs[RET2_INPUT].getVoltage(i), i);
				lights[0].value = 0;
			} else {
				outputs[SEND2_OUTPUT].setVoltage(inputs[IN_INPUT].getVoltage(i), i);
				outputs[SEND1_OUTPUT].setVoltage(inputs[RET2_INPUT].getVoltage(i), i);
				outputs[OUT_OUTPUT].setVoltage(inputs[RET1_INPUT].getVoltage(i), i);

				lights[0].value = .9;
			}
		}

	}
};


struct SimplerouterWidget : ModuleWidget {
	SimplerouterWidget(Simplerouter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/simplerouter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(51.46, 80.693)), module, Simplerouter::SWITCH_LIGHT));
		addParam(createParamCentered<VCVLatch>(mm2px(Vec(51.46, 64.693)), module, Simplerouter::SWITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.319, 38.412)), module, Simplerouter::IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(77.742, 71.126)), module, Simplerouter::RET1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(79.947, 108.618)), module, Simplerouter::RET2_INPUT));

		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(78.293, 38.595)), module, Simplerouter::OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.871, 71.861)), module, Simplerouter::SEND1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.054, 106.597)), module, Simplerouter::SEND2_OUTPUT));
	}
};


Model* modelSimplerouter = createModel<Simplerouter, SimplerouterWidget>("simplerouter");