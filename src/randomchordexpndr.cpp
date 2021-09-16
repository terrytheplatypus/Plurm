#include "plugin.hpp"
#include "randomchordrecorder.hpp"

struct Randomchordexpndr : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		SEQOUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	Randomchordrecorder const* mother = nullptr;

	Randomchordexpndr() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs& args) override {
		if (leftExpander.module) {
			if (leftExpander.module->model == modelRandomchordrecorder) {
				mother = reinterpret_cast<Randomchordrecorder*>(leftExpander.module);
			}
			if (mother) {
				outputs[SEQOUT].setVoltage(mother->currSlot);
			}
		}
	}
};


struct RandomchordexpndrWidget : ModuleWidget {
	RandomchordexpndrWidget(Randomchordexpndr* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/randomchordexpndr.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.594, 30.9)), module, Randomchordexpndr::SEQOUT));
	}
};


Model* modelRandomchordexpndr = createModel<Randomchordexpndr, RandomchordexpndrWidget>("randomchordexpndr");