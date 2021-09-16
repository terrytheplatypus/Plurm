#include "randomchordrecorder.hpp"

template <class T> class MenuOption {
	public: 
		std::string name;
		T value;
		MenuOption(std::string _name, T _value) : name(_name), value(_value) {}
};

struct RandomchordrecorderWidget : ModuleWidget {
	
	std::vector<MenuOption<int>> polyOptions;

	RandomchordrecorderWidget(Randomchordrecorder* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/randomchordrecorder.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(15.497, 23.812)), module, Randomchordrecorder::WEIGHT1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(43.383, 23.817)), module, Randomchordrecorder::WEIGHT3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(29.266, 24.054)), module, Randomchordrecorder::WEIGHT2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(72.783, 24.084)), module, Randomchordrecorder::WEIGHT5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(86.335, 24.374)), module, Randomchordrecorder::WEIGHT6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(57.548, 24.619)), module, Randomchordrecorder::WEIGHT4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(29.455, 49.613)), module, Randomchordrecorder::WEIGHT8_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(43.667, 49.943)), module, Randomchordrecorder::WEIGHT9_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(15.214, 50.033)), module, Randomchordrecorder::WEIGHT7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(85.529, 50.238)), module, Randomchordrecorder::WEIGHT12_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(57.737, 50.745)), module, Randomchordrecorder::WEIGHT10_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(72.688, 50.777)), module, Randomchordrecorder::WEIGHT11_PARAM));

		addParam(createParamCentered<BefacoPush>(mm2px(Vec(15.497, 30.805)), module, Randomchordrecorder::EDIT1_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(30.006, 30.98)), module, Randomchordrecorder::EDIT2_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(72.877, 31.455)), module, Randomchordrecorder::EDIT5_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(42.911, 31.471)), module, Randomchordrecorder::EDIT3_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(85.957, 31.556)), module, Randomchordrecorder::EDIT6_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(57.359, 32.462)), module, Randomchordrecorder::EDIT4_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(30.195, 56.917)), module, Randomchordrecorder::EDIT8_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(43.1, 57.03)), module, Randomchordrecorder::EDIT9_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(85.39, 57.587)), module, Randomchordrecorder::EDIT12_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(15.592, 57.687)), module, Randomchordrecorder::EDIT7_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(57.926, 58.21)), module, Randomchordrecorder::EDIT10_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(72.587, 58.627)), module, Randomchordrecorder::EDIT11_PARAM));

		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(18.426, 76.635)), module, Randomchordrecorder::LENGTH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.588, 103.852)), module, Randomchordrecorder::CVIN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.428, 113.5)), module, Randomchordrecorder::GATEIN_INPUT));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.85, 100.71)), module, Randomchordrecorder::TRIG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.74, 113.89)), module, Randomchordrecorder::FREEZE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.357, 103.53)), module, Randomchordrecorder::CVOUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(67.0, 113.822)), module, Randomchordrecorder::GATEOUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(29.788, 37.918)), module, Randomchordrecorder::ACTIVE2_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(15.368, 38.303)), module, Randomchordrecorder::ACTIVE1_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(43.254, 38.307)), module, Randomchordrecorder::ACTIVE3_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(73.126, 38.669)), module, Randomchordrecorder::ACTIVE5_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(57.419, 39.109)), module, Randomchordrecorder::ACTIVE4_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(86.017, 39.242)), module, Randomchordrecorder::ACTIVE6_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(29.315, 64.328)), module, Randomchordrecorder::ACTIVE8_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(42.971, 64.527)), module, Randomchordrecorder::ACTIVE9_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(15.084, 64.712)), module, Randomchordrecorder::ACTIVE7_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(57.23, 65.235)), module, Randomchordrecorder::ACTIVE10_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(86.206, 65.557)), module, Randomchordrecorder::ACTIVE12_LIGHT));
		addChild(createLightCentered<MediumLight<RedGreenBlueLight>>(mm2px(Vec(72.087, 65.645)), module, Randomchordrecorder::ACTIVE11_LIGHT));

		// polyOptions.emplace_back(std::string("Lower"), 12);
		// polyOptions.emplace_back(std::string("Repeat"), 24);
		// offsetOptions.emplace_back(std::string("Upper"), 36);
		// offsetOptions.emplace_back(std::string("Random"), 0);

		for(int n = 1; n <17; n++) {
			polyOptions.emplace_back(to_string(n), n);
		}
	
	}

	void appendContextMenu(Menu *menu) override {
		Randomchordrecorder *recorder = dynamic_cast<Randomchordrecorder*>(module);
		assert(recorder);

		struct RecorderMenu : MenuItem {
			Randomchordrecorder *module;
			RandomchordrecorderWidget *parent;
		};

		struct PolyphonyItem : RecorderMenu {
			int numPoly;
			void onAction(const rack::event::Action &e) override {
				module->poly = numPoly;
			}
		};

		struct PolyphonyMenu : RecorderMenu {
			Menu *createChildMenu() override {
				Menu *menu = new Menu;
				for (auto opt: parent->polyOptions) {
					PolyphonyItem *item = createMenuItem<PolyphonyItem>(opt.name, CHECKMARK(module->poly == opt.value));
					item->module = module;
					item->numPoly = opt.value;
					menu->addChild(item);
				}
				return menu;
			}
		};

		menu->addChild(construct<MenuLabel>());
		PolyphonyMenu *polyphonyItem = createMenuItem<PolyphonyMenu>("Polyphony Channels");
		polyphonyItem->module = recorder;
		polyphonyItem->parent = this;
		menu->addChild(polyphonyItem);
	}
};

Model* modelRandomchordrecorder = createModel<Randomchordrecorder, RandomchordrecorderWidget>("randomchordrecorder");