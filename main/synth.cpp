#include <Audio.h>
#include "synth.h"

static const int NUM_VOICES = 8;

// audio objects
AudioSynthWaveform      voice[NUM_VOICES];
AudioMixer4             mix1; // mixer for voices 0-3
AudioMixer4             mix2; // mixer for voices 4-7
AudioMixer4             mixFinal; // mixer for all 8
AudioOutputI2S          i2s1;

AudioConnection         patchCord1(voice[0], 0, mix1, 0);
AudioConnection         patchCord2(voice[1], 0, mix1, 1);
AudioConnection         patchCord3(voice[2], 0, mix1, 2);
AudioConnection         patchCord4(voice[3], 0, mix1, 3);

AudioConnection         patchCord5(voice[4], 0, mix2, 0);
AudioConnection         patchCord6(voice[5], 0, mix2, 1);
AudioConnection         patchCord7(voice[6], 0, mix2, 2);
AudioConnection         patchCord8(voice[7], 0, mix2, 3);

AudioConnection         patchCord9(mix1, 0, mixFinal, 0);
AudioConnection         patchCord10(mix2, 0, mixFinal, 1);

AudioConnection         patchCord11(mixFinal, 0, i2s1, 0); // left channel
AudioConnection         patchCord12(mixFinal, 0, i2s1, 1); // right channel

AudioControlSGTL5000    audioShield;

struct VoiceState {
    bool active = false;
    float freq = 0;
    int noteIndex = -1;
};

static VoiceState vs[NUM_VOICES];

// Returns index of a free voice or steals one
static int findFreeVoice() {
    for (int i = 0; i < NUM_VOICES; i++) {
        if (!vs[i].active) return i;
    }
    // Steal voice 0 if all full
    return 0;
}

void synthInit() {
    AudioMemory(50);

    audioShield.enable();
    audioShield.volume(1.0);
    audioShield.lineOutLevel(27);

    // All voices silent and sine by default
    for (int i = 0; i < NUM_VOICES; i++) {
        voice[i].begin(0.0, 440, WAVEFORM_SINE);
        vs[i] = VoiceState();  // reset
    }

    // Mixer gains
    for (int i = 0; i < 4; i++) mix1.gain(i, 0.5);
    for (int i = 0; i < 4; i++) mix2.gain(i, 0.5);
    mixFinal.gain(0, 0.7);
    mixFinal.gain(1, 0.7);
}

void noteOn(int noteIndex, float freq) {
    int v = findFreeVoice();
    vs[v].active = true;
    vs[v].freq = freq;
    vs[v].noteIndex = noteIndex;
    voice[v].frequency(freq);
    voice[v].amplitude(0.7);
}

void noteOff(int noteIndex) {
    for (int v = 0; v < NUM_VOICES; v++) {
        if (vs[v].active && vs[v].noteIndex == noteIndex) {
            voice[v].amplitude(0);
            vs[v].active = false;
        }
    }
}
