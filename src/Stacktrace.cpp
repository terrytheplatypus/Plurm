#include "plugin.hpp"
#include <stack>

struct Stacktrace : Module {

    enum ParamIds {
        NUM_PARAMS
    };

    enum InputIds {
        POP_INPUT,
        PUSH_INPUT,
        IN_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        OUT_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds {
        FRAME8_LIGHT,
        FRAME7_LIGHT,
        FRAME6_LIGHT,
        FRAME5_LIGHT,
        FRAME4_LIGHT,
        FRAME3_LIGHT,
        FRAME2_LIGHT,
        FRAME1_LIGHT,
        NUM_LIGHTS
    };

    Stacktrace() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    dsp::SchmittTrigger st1;
    dsp::SchmittTrigger st2;
    std::stack<float> stak;
    bool pushGateState = false;
    bool popGateState = false;

    void process(const ProcessArgs& args) override {
        float currSample;
        //if there is nothing on the stack, output 0

        if (stak.empty()) {
            outputs[OUT_OUTPUT].setVoltage(0);
        } else {
            outputs[OUT_OUTPUT].setVoltage(stak.top());
        }

        //if trigger is received from push, push input to stak

        if (st1.process(inputs[PUSH_INPUT].getVoltage()) && stak.size() < 7) {
            currSample = inputs[IN_INPUT].getVoltage();
            stak.push(currSample);
        }

        //if trigger is received from pop, pop output from stak
        if (st2.process(inputs[POP_INPUT].getVoltage()) && !stak.empty()) {
            stak.pop();
        }

        //iterate through lights and turn on the ones that are in the stack

        //i don't understand why this has to be 1-indexed but ok
        for (size_t i = 1; i < NUM_LIGHTS; i++) {
            lights[i].setBrightness(i < stak.size()+1 ? 1.f : 0.f);
        }
    }
};


struct StacktraceWidget : ModuleWidget {
    
    //hacky bullshit to nudge the lights into the right position
    float nudge = 4;
    
	StacktraceWidget(Stacktrace* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Stacktrace.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.02, 102.621)), module, Stacktrace::POP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.843, 102.828)), module, Stacktrace::PUSH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.615, 120.952)), module, Stacktrace::IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.564, 120.953)), module, Stacktrace::OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 24.946-nudge)), module, Stacktrace::FRAME8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 24.946-nudge)), module, Stacktrace::FRAME7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 35.53-nudge)), module, Stacktrace::FRAME6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 46.302-nudge)), module, Stacktrace::FRAME5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 56.696-nudge)), module, Stacktrace::FRAME4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 67.28-nudge)), module, Stacktrace::FRAME3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 77.863-nudge)), module, Stacktrace::FRAME2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.686, 88.824-nudge)), module, Stacktrace::FRAME1_LIGHT));
	}
};


Model* modelStacktrace = createModel<Stacktrace, StacktraceWidget>("Stacktrace");