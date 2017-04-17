#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TeensyAudioPlotter.h>
#include "effect_moddelay.h"

AudioSynthWaveform waveform;
AudioSynthWaveform waveform2;
AudioAnalyzeLogger dryLogger;
AudioAnalyzeLogger modLogger;
AudioEffectModdelay moddelay;
AudioAnalyzeLogger wetLogger;
AudioOutputStepper stepper;

AudioConnection patchCord1(waveform, 0, dryLogger, 0);
AudioConnection patchCord2(dryLogger, 0, moddelay, 0);
AudioConnection patchCord3(waveform2, 0, modLogger, 0);
AudioConnection patchCord4(modLogger, 0, moddelay, 1);
AudioConnection patchCord5(moddelay, 0, wetLogger, 0);
AudioConnection patchCord6(wetLogger, 0, stepper, 0);

TeensyAudioPlotter plotter;

void setup() {
  Serial.begin(9600);
  AudioMemory(117);

  plotter.setStepper(stepper);
  plotter.addLogger(dryLogger);
  plotter.addLogger(modLogger);
  plotter.addLogger(wetLogger);
  //plotter.continuous(false);

  waveform.begin(1.0, 344.5, WAVEFORM_PULSE);
  waveform.pulseWidth(0.1);
  waveform2.begin(1.0, 344.5 * 0.125, WAVEFORM_SINE);
  //moddelay.delay(0.1);
  moddelay.delay(3.0);
}

void loop() {
  plotter.step();
  plotter.done();
}
