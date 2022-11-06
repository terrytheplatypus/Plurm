#include "plugin.hpp"
#include <math.h>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "external/catch.hpp"

struct Chordscalequantizer : Module
{

	std::mt19937 getSeededTwister()
	{
#ifdef ARCH_WIN
		std::mt19937 twister(time(0));
#else
		std::random_device rd;		// only used once to initialise (seed) engine
		std::mt19937 twister(rd()); // random-number engine used (Mersenne-Twister in this case)
#endif
		return twister;
	}

	std::mt19937 rng = getSeededTwister();
	;

	float gateHigh = 2.0;

	// list of notes
	const int C = 0;
	const int CS = 1;
	const int D = 2;
	const int DS = 3;
	const int E = 4;
	const int F = 5;
	const int FS = 6;
	const int G = 7;
	const int GS = 8;
	const int A = 9;
	const int AS = 10;
	const int B = 11;

	// list of scales
	int C_Scale[7] = {C, D, E, F, G, A, B};
	int CS_Scale[7] = {CS, DS, F, FS, GS, AS, C};
	int D_Scale[7] = {D, E, FS, G, A, B, CS};
	int DS_Scale[7] = {DS, F, G, GS, AS, C, D};
	int E_Scale[7] = {E, FS, GS, A, B, CS, DS};
	int F_Scale[7] = {F, G, A, AS, C, D, E};
	int FS_Scale[7] = {FS, GS, AS, B, CS, DS, F};
	int G_Scale[7] = {G, A, B, C, D, E, FS};
	int GS_Scale[7] = {GS, AS, C, CS, DS, F, G};
	int A_Scale[7] = {A, B, CS, D, E, FS, GS};
	int AS_Scale[7] = {AS, C, D, DS, F, G, A};
	int B_Scale[7] = {B, CS, DS, E, FS, GS, AS};

	int *allScales[12] = {C_Scale,
						  CS_Scale,
						  D_Scale,
						  DS_Scale,
						  E_Scale,
						  F_Scale,
						  FS_Scale,
						  G_Scale,
						  GS_Scale,
						  A_Scale,
						  AS_Scale,
						  B_Scale};

	// set to 0 to start with, which corresponds with C diatonic scale but this should be ok
	int curScale = 0;

	enum ParamId
	{
		PARAMS_LEN
	};
	enum InputId
	{
		CHORD_CV_INPUT,
		CHORD_GATE_INPUT,
		MELODY_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		QUANTIZED_CV_OUTPUT,
		ROOT_OUTPUT,
		CHORD_CHANGE_OUTPUT,
		SCALE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	std::unordered_set<int> chordSet;
	// std::uniform_real_distribution<> dis = std::uniform_real_distribution<>(0.0, 1.0);
	int currNotes[16];
	bool currGates[16];
	int currScale = 0;
	long timer = 0;
	// behavior could be that until you play a chord into the chord input,
	// it doesnt quantize anything
	bool seeded = false;

	Chordscalequantizer()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(CHORD_CV_INPUT, "");
		configInput(CHORD_GATE_INPUT, "");
		configInput(MELODY_CV_INPUT, "");
		configOutput(QUANTIZED_CV_OUTPUT, "");
	}

	void process(const ProcessArgs &args) override
	{
		timer++;
		int chordInChannels = inputs[CHORD_GATE_INPUT].getChannels();
		bool notesChanged = false;
		for (int i = 0; i < chordInChannels; i++)
		{
			int prevNote = currNotes[i];
			if (inputs[CHORD_GATE_INPUT].getVoltage(i) > gateHigh)
			{
				currGates[i] = true;
				// quantize the note

				int newNote = quantizeForChord(inputs[CHORD_CV_INPUT].getVoltage(i));
				if (prevNote != newNote)
				{
					currNotes[i] = newNote;
					// you should only be erasing prevnote if it is not one of the current
					// input notes
					// instead of doing this complicated shit you could clear the whole set
					// and insert all the new notes, which is what you actually need to do
					chordSet.erase(prevNote);

					chordSet.insert(newNote);
					notesChanged = true;
				}
				//

				// if no gates were high before, pick new scale
			}
			else
			{
				if (currGates[i] == true)
				{
					chordSet.erase(prevNote);
					currGates[i] = false;
				}
			}
		}

		// scoring process, for each scale, add one point for matching note, subtract one for non-matching
		// pick at random when there are ties
		// recalculate when a new note is added to the chord
		//  do not use polyphony settings like rcr because output polyphony should be the same as input always

		if (notesChanged)
		{
			// maybe put this into a separate func so you can unit test
			// the scores for each scale
			int scores[12];
			// loop over scales and use set to cut down on verbose code for now
			for (int i = 0; i < 12; i++)
			{
				std::unordered_set<int> scaleSet(allScales[i],
												 allScales[i] + 7);

				int score = 0;
				for (auto note : chordSet)
				{
					if (contains(scaleSet, note))
					{
						score++;
					}
					else
					{
						score--;
					}
				}
				scores[i] = score;
			}

			// now get the max scored index, picking randomly between ties
			// or just ignore ties for now and only pick the first maximum
			// or better yet, simplest approach to ties is do a coinflip for each tie whether to swap
			//(last approach is bad because the probability between more than two ties is uneven
			// could move this out to a separate function

			// largest possible score is 7, smallest is -5
			// that is 13 length array

			// int[13][12] scoreOccurrences;
			// vector<vector<int>> scoreOccurrences;

			// mapping each possible score to all the scales corresponding to it
			std::unordered_map<int, std::vector<int>> scoreMap;

			int max = INT_MIN;
			int maxIdx = -1;
			for (int i = 0; i < 12; i++)
			{
				scoreMap[scores[i]].push_back(i);
				if (scores[i] > max)
				{
					max = scores[i];
					maxIdx = i;
				}
			}

			// pick randomly between ties
			std::vector<int> maxScores = scoreMap.at(max);

			std::uniform_int_distribution<> dis(0, maxScores.size() - 1);
			currScale = maxScores[dis(rng)];
		}

		/*SETTING OUTPUT*/

		int melodyInChannels = inputs[MELODY_CV_INPUT].getChannels();
		// set output channel number to input channel number
		outputs[QUANTIZED_CV_OUTPUT].setChannels(melodyInChannels);
		for (int i = 0; i < outputs[QUANTIZED_CV_OUTPUT].getChannels(); i++)
		{
			float in = inputs[MELODY_CV_INPUT].getVoltage(i);
			bool debug = (timer % 44100 == 0);
			outputs[QUANTIZED_CV_OUTPUT].setVoltage(naiveQuantizer(in, allScales[currScale], debug), i);
		}
		outputs[ROOT_OUTPUT].setVoltage(scaleToVolt(allScales[currScale][0]), 0);
		outputs[CHORD_CHANGE_OUTPUT].setVoltage(notesChanged ? 10 : 0, 0);
		outputs[SCALE_OUTPUT].setChannels(7);
		for (int i = 0; i < 7; i++)
		{
			outputs[SCALE_OUTPUT].setVoltage(scaleToVolt(allScales[currScale][i]), i);
		}

		// potential features: add avoid notes, or probability to hit avoid notes
		// if the first three notes are a major triad in any inversion, avoid the fourth
		// add probability and length of runs that are not in the current scale

		// possible feature: for larger chords, make it more likely that you only stay within the chord tones
		// because with like 5/6 note chords it's easier to go to a note that sound bad if you dont use
		// the chord tones
	}

	float scaleToVolt(int note)
	{
		return ((float)note) / 12.0;
	}

	int quantizeForChord(float in)
	{
		float whole, frac;
		frac = std::modf(in, &whole);
		return (int)round(frac * 12);
	}

	// this is naive quantizer impl, which does linear search for closest note
	// use to prototype but really should optimize for performance
	// would have to understand vcv quantizer implementation to optimize
	// i was unit testing this in ideone cuz i dont know how to unit test here lol
	double naiveQuantizer(float in, int scale[], bool debug)
	{

		float oct, note;
		note = std::modf(in, &oct);
		// this needs to be a float otherwise it'll round to 0
		float closestDistance = 100.0;
		int closestIdx = 0;
		for (int n = 0; n < 7; n++)
		{
			float scalenote = ((float)scale[n]) / 12.0;
			if (std::abs(note - scalenote) < closestDistance)
			{
				closestIdx = n;
				closestDistance = std::abs(note - scalenote);
			}
		}
		float out = scaleToVolt(scale[closestIdx]) + oct;
		if (debug)
		{
			std::cout << "in volt was " << in << " and out volt was " << out << std::endl;
		}
		// std::cout << "what the fuck" << std::endl;
		return out;
	}

	// TODO: dunno how to do unit testing in cpp but i should figure it out
	// TEST_CASE( "quantize expected shit", "[naiveQuantizerTest]" ) {
	// int
	//}

	bool contains(std::unordered_set<int> set, int x)
	{
		auto search = set.find(x);
		if (search != set.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct ChordscalequantizerWidget : ModuleWidget
{
	ChordscalequantizerWidget(Chordscalequantizer *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/chordscalequantizer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.07, 69.104)), module, Chordscalequantizer::CHORD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.071, 81.785)), module, Chordscalequantizer::CHORD_GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.438, 108.067)), module, Chordscalequantizer::MELODY_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.579, 96.304)), module, Chordscalequantizer::QUANTIZED_CV_OUTPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.579, 110.304)), module, Chordscalequantizer::ROOT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.579, 72.304)), module, Chordscalequantizer::CHORD_CHANGE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.579, 52.304)), module, Chordscalequantizer::SCALE_OUTPUT));
	}
};

Model *modelChordscalequantizer = createModel<Chordscalequantizer, ChordscalequantizerWidget>("chordscalequantizer");