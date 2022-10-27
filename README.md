Plurm:
A VCV rack plugin.

Stacktrace:
A sample and hold module with 8 stack frames. When the Push gate is triggered, the In input is sampled and added to the stack. Each additional push pushes an additional value onto the stack. Pop removes a value from the top of the stack. The value that is output is the value at the top of the stack, and is 0 when the stack is empty.

randomchordrecorder:
A polyphonic sequencer which allows the user to directly record chords which will be randomly sequenced based on user-assigned weights. Here are descriptions of the knobs and inputs and outputs of this module:
1. sequencing section: Each slot in the sequence has a knob to assign weight, a button to jump to that slot for playback and editing, and an led to signify state. The weights of each slot are added up to get a total weight, and individual slot weights are picked in proportion to the total weight. As said before, the buttons allow the user to jump to a slot, which both triggers playback of that slot, and allows editing. a blank LED is one that falls outside of the sequence boundary, a white one is one that falls within the sequence boundary but has nothing recorded into it, a green one has something recorded, and the current slot is marked red. it also blinks when the user is recording.
2.poly v/oct and gate in and out: the cv and gate out number of polyphony  channels can be configured in the right-click menu.
3.Length: the number of possible slots that can be randomly picked from.
4. Trig and Freeze: trig triggers a new slot to be played, and freeze locks trig to retrigger the same slot when the gate is high.

RECORDING: When an LED under a slot is red, you can record to that slot by having input into the poly cv and gate inputs. the recording will begin when any gate becomes high and ends when all gates are low. while editing, the notes you play will come out of the cv and gate output.

ADDITIONAL: the number of output polyphony channels can be configured in the right-click menu. The default is 5 channels.

randomchordexpndr: an expander for randomchordrecorder. currently there is one output for the current slot/sequence number (0 through 11 volts output). The intent for this is to control other sequencers with the output of this one, to play certain melodies for each chord.

simplerouter: Simple concept, base input goes into the "in" input. effect A recieves audio from send1 and recieves from ret1. effect B recieves audio from send2 and recieves from ret2. the switch switches the order in which effects A and B process audio, in the initial state (with light off) effect A is first, in the "on" state (with light on), effect B is first.

chordscalequantizer: the intent of this module is to play a chord into the chord input the chord is used to pick a scale that matches its notes, and the scale is used to quantize incoming cv. this module does not function fully as intended, but i thought i might as well release a somewhat incomplete module rather than hold onto it forever.
