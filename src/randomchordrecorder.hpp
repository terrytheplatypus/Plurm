#ifndef RANDOMCHORDRECORDER_H
#define RANDOMCHORDRECORDER_H

#include "plugin.hpp"
#include <random>
#ifdef ARCH_WIN
#include <ctime>
#endif

using namespace std;

struct Randomchordrecorder : Module {
	enum ParamIds {
		WEIGHT1_PARAM,
		WEIGHT2_PARAM,
		WEIGHT3_PARAM,
		WEIGHT4_PARAM,
		WEIGHT5_PARAM,
		WEIGHT6_PARAM,
		WEIGHT8_PARAM,
		WEIGHT7_PARAM,
		WEIGHT9_PARAM,
		WEIGHT10_PARAM,
		WEIGHT11_PARAM,
		WEIGHT12_PARAM,
		EDIT1_PARAM,
		EDIT2_PARAM,
		EDIT3_PARAM,
		EDIT4_PARAM,
		EDIT5_PARAM,
		EDIT6_PARAM,
		EDIT7_PARAM,
		EDIT8_PARAM,
		EDIT9_PARAM,
		EDIT10_PARAM,
		EDIT11_PARAM,
		EDIT12_PARAM,
		LENGTH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CVIN_INPUT,
		GATEIN_INPUT,
		TRIG_INPUT,
		FREEZE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CVOUT_OUTPUT,
		GATEOUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ACTIVE1_LIGHT,
		ACTIVE2_LIGHT = ACTIVE1_LIGHT + 3,
		ACTIVE3_LIGHT = ACTIVE2_LIGHT + 3,
		ACTIVE4_LIGHT = ACTIVE3_LIGHT + 3,
		ACTIVE5_LIGHT = ACTIVE4_LIGHT + 3,
		ACTIVE6_LIGHT = ACTIVE5_LIGHT + 3,
		ACTIVE7_LIGHT = ACTIVE6_LIGHT + 3,
		ACTIVE8_LIGHT = ACTIVE7_LIGHT + 3,
		ACTIVE9_LIGHT = ACTIVE8_LIGHT + 3,
		ACTIVE10_LIGHT = ACTIVE9_LIGHT + 3,
		ACTIVE11_LIGHT = ACTIVE10_LIGHT + 3,
		ACTIVE12_LIGHT = ACTIVE11_LIGHT + 3,
		NUM_LIGHTS = ACTIVE12_LIGHT + 3
	};

	enum RecordingState {
		NOT_REC,
		STARTED_REC,
		FINISHED_REC
	};

	struct chord {
		float notes[16];
		bool gates[16];
		bool occupied = false;
	};

	struct chord chords[12];
	float tempNotes[16];
	float tempGates[16];

	const int gateHigh = 5;

	int currSlot = 0;

	int numPolyChannels = 5;

	int SLOT_NUM = 12;

	RecordingState recState = NOT_REC;

	//this is a slightly hacky flag to reset the gates
	bool initialPlayback = true;

	dsp::SchmittTrigger trig;

	int lightTimer = 0;

	int gateTimer = 0;

	//https://stackoverflow.com/a/19728404

	std::mt19937 rng;

	std::mt19937 getSeededTwister() {
		#ifdef ARCH_WIN
			std::mt19937 twister(time(0));
		#else
			std::random_device rd;     // only used once to initialise (seed) engine
			std::mt19937 twister(rd());    // random-number engine used (Mersenne-Twister in this case)
		#endif
		return twister;
	}

	Randomchordrecorder() {
		rng = getSeededTwister();
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		// configParam(WEIGHT1_PARAM, 0.f, 1.f, 0.f, "");
		for(int n=0; n < SLOT_NUM; n++) {
			//configure weight knobs and edit buttons
			//0 and 1 are not limits because those cause segfault
			configParam(n, 0, 1, .5, "weight");
			configParam(n + SLOT_NUM, 0.0, 1.0, 0.0, "edit");
		}
		//length param should be discrete, from 1 to 12
		// configParam(LENGTH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LENGTH_PARAM, 1.0, (double) SLOT_NUM , (double) SLOT_NUM, "length");
		outputs[CVOUT_OUTPUT].setChannels(numPolyChannels);
		outputs[GATEOUT_OUTPUT].setChannels(numPolyChannels);
		//just init temp chord arrays
		clearTempChord();

		for(int n = 0; n < SLOT_NUM; n++) {
			clearChord(n);
		}
	}

	void onReset() override {
		for(int n = 0; n < SLOT_NUM; n++) {
			clearChord(n);
		}
	}

	int getLength() {
		return params[LENGTH_PARAM].getValue();
	}

	void clearChord(int index) {
		for(int n = 0; n < numPolyChannels; n++) {
			chords[index].gates[n] = false;
			chords[index].notes[n] = 0;
		}
		chords[index].occupied = false;
	}

	void clearCurrChord() {
		clearChord(currSlot);
	}

	void copyCurrChord() {
		for(int n = 0; n < numPolyChannels; n++) {
			chords[currSlot].gates[n] = tempGates[n];
			chords[currSlot].notes[n] = tempNotes[n];
		}
		chords[currSlot].occupied = true;
	}

	void clearTempChord() {
		for(int n = 0; n < numPolyChannels; n++) {
			tempGates[n] = false;
			tempNotes[n] = 0;
		}
	}

	void nextEmptySlot() {
		// currSlot = currSlot<getLength()-1?currSlot+1:0;
		int currIndex = currSlot;
		for(int n=0;n<getLength()-1;n++) {
			currIndex = (currIndex<getLength()-1)?currIndex+1:0;
			if(!chords[currIndex].occupied) {
				currSlot = currIndex;
				break;
			}
		}
	}

	void resetGates() {
		for(int n = 0; n < numPolyChannels; n++) {
			outputs[GATEOUT_OUTPUT].setVoltage(0, n);
		}
		gateTimer = 0;
		initialPlayback = true;
	}

	void process(const ProcessArgs& args) override {

		
		outputs[CVOUT_OUTPUT].setChannels(numPolyChannels);
		outputs[GATEOUT_OUTPUT].setChannels(numPolyChannels);

		//timers are used for blinking light and gate reset
		lightTimer = (lightTimer + 1) % ( (int) args.sampleRate);
		gateTimer = (gateTimer + 1) % ( (int) args.sampleRate);

		

		// if(chords[currSlot].occupied) {
			int inChannels = inputs[GATEIN_INPUT].getChannels();

			bool anyGateHigh = false;
			for(int n = 0; n < inChannels; n++ ) {
				if(inputs[GATEIN_INPUT].getVoltage(n) > gateHigh) {
					anyGateHigh = true;

					if(recState == STARTED_REC) {
						//copy v/oct and gate to the current slot
						tempNotes[n] = inputs[CVIN_INPUT].getVoltage(n);
						tempGates[n] = true;
					}

					if(recState == NOT_REC) {
						recState = STARTED_REC;
						// cout << "startd recording"<<endl;
						// bump the gates to 0 for a single cycle just so the gate is retriggered for the next chord
						resetGates();
						
						
					}

					

				}
			}
			//if all gates are low after recording started change to finished recording state, and copy all 
			//temp chord values into the current slot, then clear the temp arrays
			if(!anyGateHigh && recState == STARTED_REC) {
				copyCurrChord();
				clearTempChord();
				recState = NOT_REC;
				//go to the next empty slot because it's natural to record to the next empty slot,
				//it would be unnatural to go to an occupied slot though
				nextEmptySlot();
			}

		

		//playback
		
		//reasonable behavior: if channel has gate, just output constant high gate
		//the user is responsible for how they want to manage the gates
		//user input overrides the playback
		
		if(gateTimer>10) initialPlayback = false;
		
		bool gateOn = (gateTimer>10 || !initialPlayback);
		if(recState == STARTED_REC) {
			//user input
				for(int n = 0; n < numPolyChannels; n++) {
					outputs[CVOUT_OUTPUT].setVoltage(tempNotes[n], n);
					//the 10 sample latency is necessary to shut the gate on and off
					//a bit hacky, but seemed unavoidable
					//at 44100 hz that's .25 ms which is negligible
					outputs[GATEOUT_OUTPUT].setVoltage(
						tempGates[n]*10*gateOn, n);
				}
		} else {
			for(int n = 0; n < numPolyChannels; n++) {
				outputs[CVOUT_OUTPUT].setVoltage(chords[currSlot].notes[n], n);
				outputs[GATEOUT_OUTPUT].setVoltage(chords[currSlot].gates[n]*10*gateOn, n);
			}
		}

		

		//when trig is pressed, if freeze gate is is not high,
		// switch to random slot out of the current occupied ones, and play that one
		
		//trig should obviously only change once the gate changes from low to high

		//if freeze is high, play the same thing, don't switch
		//switching to a different chord while editing is not done will abort the edit, even if freeze is enabled
		
		if(trig.process(inputs[TRIG_INPUT].getVoltage())) {

			clearTempChord();
			recState = NOT_REC;
			// bump the gates to 0 for a single cycle just so the gate is retriggered for the next chord
			resetGates();

			std::vector<int> occupiedSlots;
			std::vector<double> tempWeights;
			std::vector<double> weights;

			//weight is relative to the sum of all the weight knobs
			//only the weights of active slots are used
			double totalWeight = 0;
			for(int n =0; n < getLength(); n++) {
				if(chords[n].occupied) {
					occupiedSlots.push_back(n);
					totalWeight += params[n].getValue();
					tempWeights.push_back(params[n].getValue());
				}
			}
			for(size_t n =0; n < occupiedSlots.size(); n++) {
				//if all weights are set to 0, make it uniform
				if(totalWeight != 0) {
					weights.push_back(tempWeights[n]/totalWeight);
				} else weights.push_back(1);
			}

			if(	occupiedSlots.size()>1 &&
				inputs[FREEZE_INPUT].getVoltage() < gateHigh && 
				getLength() > 1) {
				//this allows repetitions
				std::discrete_distribution<> d(weights.begin(), weights.end());
				currSlot = occupiedSlots[d(rng)];
			} 
		}

		//if a edit/play button is pressed, abort recording and jump to that node
		//should just do that for the first one that's pressed, ignore mapping hax
		//also do not abort recording if you're pressing the slot it's currently on
		for(int n =12; n < 12 + getLength(); n++) {
			if (params[n].value > 0 && n != currSlot) {
				// bump the gates to 0 for a single cycle just so the gate is retriggered for the next chord
				resetGates();
				currSlot = n - 12;
				recState = NOT_REC;
				clearTempChord();
				break;
			}
		}

		//design for lights:
		//blinking red means recording
		//unblinking red means current
		//green means not current, but occupied,
		//white means not occupied, but within the sequence boundary,
		//no light means not in the sequence boundary

		for (int i = 0; i < SLOT_NUM; i++) {
			if(i < getLength()) {
				if(i == currSlot && !(recState == STARTED_REC)) {
					//not recording
					//red
					lights[i*3].value = .8;
					lights[i*3+1].value = 0;
					lights[i*3+2].value = 0;

				}
				else if(i == currSlot && recState == STARTED_REC) {
					//recording, blink red
					if(lightTimer% (int) args.sampleRate < args.sampleRate/2) {
						lights[i*3].value = 1;
						lights[i*3+1].value = 0;
						lights[i*3+2].value = 0;
					} else {
						lights[i*3].value = .2;
						lights[i*3+1].value = 0;
						lights[i*3+2].value = 0;
					}
				}
				else if(i < getLength() && i != currSlot) {
					if(chords[i].occupied) {
						lights[i*3].value = 0;
						lights[i*3+1].value = .8;
						lights[i*3+2].value = 0;
					} else {
						lights[i*3].value = .8;
						lights[i*3+1].value = .8;
						lights[i*3+2].value = .8;
					}
				}
			}
			else if(i >= getLength()) {
				lights[i*3].value = 0;
				lights[i*3+1].value = 0;
				lights[i*3+2].value = 0;
			}
        }

		//could have rightclick menu for polyphony channels
		
	}

	//chords and weights should be saved and loaded
	//get json save/load working after you get chord recording working
	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		json_t *chordsJ = json_object();
		//each chord should be a json object with occupied flag, notes array, and
		//gates array

		//store num of polyphony channels
		json_t *polyJ = json_integer(numPolyChannels);
		json_object_set_new(rootJ, "numPolyChannels", polyJ);
		
		for(int n = 0; n < SLOT_NUM; n++) {
			json_t *chordJ = json_object();

			json_object_set_new(chordJ, "occupied", 
						json_boolean(chords[n].occupied));
			json_t *notesJ = json_array();
			json_t *gatesJ = json_array();
			
			for(int m = 0; m < 16; m++) {
				json_t *gateJ = json_boolean(chords[n].gates[m]);
				json_t *noteJ = json_real((double) chords[n].notes[m]);
				json_array_append_new(notesJ, noteJ);
				json_array_append_new(gatesJ, gateJ);
			}
			json_object_set_new(chordJ, "notes", notesJ);
			json_object_set_new(chordJ, "gates", gatesJ);

			json_object_set_new(chordsJ, ("chord" + to_string(n)).c_str(), chordJ);

		}

		json_object_set_new(rootJ, "chords", chordsJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// json_t *lengthJ = json_object_get(rootJ, "length");

		json_t *polyJ = json_object_get(rootJ, "numPolyChannels");
		if(polyJ) {
			numPolyChannels = json_integer_value(polyJ);
		}

		json_t *chordsJ = json_object_get(rootJ, "chords");
		if(chordsJ) {
			for(int n = 0; n < SLOT_NUM; n++) {
				json_t *chordJ = json_object_get(chordsJ, ("chord" + to_string(n)).c_str());
				if(chordJ) {
					json_t *occupied = json_object_get(chordJ, "occupied");
					if(occupied) {
						chords[n].occupied = json_is_true(occupied);
					}
					json_t *notesJ = json_object_get(chordJ, "notes");
					json_t *gatesJ = json_object_get(chordJ, "gates");
					if(notesJ) {
						for(int m = 0; m < 16; m++) {
							json_t *noteJ = json_array_get(notesJ, m);
							if(noteJ) {
								chords[n].notes[m] = json_real_value(noteJ);
							}
						}
					}
					if(gatesJ) {
						for(int m = 0; m < 16; m++) {
							json_t *gateJ = json_array_get(gatesJ, m);
							if(gateJ) {
								chords[n].gates[m] = json_is_true(gateJ);
							}
						}
					}
				}
			}
		}
	}


};

#endif