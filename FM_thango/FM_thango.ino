#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       Modulator;      //xy=81.49999618530273,663.0000190734863
AudioSynthWaveform       Modulator1; //xy=107.66666412353516,872.3333129882812
AudioEffectEnvelope      modEnv;      //xy=301.66666412353516,665.0000190734863
AudioEffectEnvelope      modEnv1; //xy=327.8333320617676,874.3333129882812
AudioAmplifier           modAmp;           //xy=444.6666679382324,663.0000190734863
AudioAmplifier           modAmp1; //xy=470.83333587646484,872.3333129882812
AudioSynthNoiseWhite     noise;         //xy=611.6667060852051,573.0000152587891
AudioSynthWaveformModulated Carrier;   //xy=613.6667060852051,661.0000190734863
AudioSynthNoiseWhite     noise1; //xy=637.8333740234375,782.333309173584
AudioSynthWaveformModulated Carrier1; //xy=639.8333740234375,870.3333129882812
AudioEffectBitcrusher    bitcrush;    //xy=750.6667098999023,690.000020980835
AudioEffectBitcrusher    bitcrush1; //xy=776.8333778381348,899.3333148956299
AudioMixer4              mixer;         //xy=938.6667137145996,673.0000190734863
AudioMixer4              mixer1; //xy=964.833381652832,882.3333129882812
AudioEffectEnvelope      env;      //xy=1074.6666564941406,673.0000190734863
AudioEffectEnvelope      env1; //xy=1100.833324432373,882.3333129882812
AudioMixer4              sum;         //xy=1248.666660308838,764.3333358764648
AudioOutputI2S           i2s1;           //xy=1442.0006523132324,750.0001621246338
AudioConnection          patchCord1(Modulator, modEnv);
AudioConnection          patchCord2(Modulator1, modEnv1);
AudioConnection          patchCord3(modEnv, modAmp);
AudioConnection          patchCord4(modEnv1, modAmp1);
AudioConnection          patchCord5(modAmp, 0, Carrier, 0);
AudioConnection          patchCord6(modAmp, 0, Carrier, 1);
AudioConnection          patchCord7(modAmp1, 0, Carrier1, 0);
AudioConnection          patchCord8(modAmp1, 0, Carrier1, 1);
AudioConnection          patchCord9(noise, 0, mixer, 0);
AudioConnection          patchCord10(Carrier, 0, mixer, 1);
AudioConnection          patchCord11(Carrier, bitcrush);
AudioConnection          patchCord12(noise1, 0, mixer1, 0);
AudioConnection          patchCord13(Carrier1, 0, mixer1, 1);
AudioConnection          patchCord14(Carrier1, bitcrush1);
AudioConnection          patchCord15(bitcrush, 0, mixer, 2);
AudioConnection          patchCord16(bitcrush1, 0, mixer1, 2);
AudioConnection          patchCord17(mixer, env);
AudioConnection          patchCord18(mixer1, env1);
AudioConnection          patchCord19(env, 0, sum, 0);
AudioConnection          patchCord20(env1, 0, sum, 1);
AudioConnection          patchCord21(sum, 0, i2s1, 0);
AudioConnection          patchCord22(sum, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=542.0001983642578,506.00017833709717
// GUItool: end automatically generated code

bool mode1; //using bool to store state as true or false
bool mode;
bool decayMode;
bool releaseMode;

float freq; //setting up global variables for controlling parameters
float ModulatorRDtime;
float CarrierRDtime;
float modScaled;
float inverseMix;
float mixScaled;
float nScaled;
float myRatio;
int BITS;
int SAMPLERATE;
int modPitchRatio[] = {0.25,0.5,1,1.5,2,3,4,6,8}; //an array for storing values that can be used to modulate the pitch information

float freq1; // same again but for synth track 2
float ModulatorRDtime1;
float CarrierRDtime1;
float modScaled1;
float inverseMix1;
float mixScaled1;
float nScaled1;
float myRatio1;
int BITS1;
int SAMPLERATE1;
int modPitchRatio1[] = {0.25,0.5,1,1.5,2,3,4,6,8};

//setting up sensors/controls..

int pot1 = A17; //Pitch Ratio
int pot2 = A16; //Mod Amount
int pot3 = A15; //Release / Decay -- Modulator
int pot4 = A12; //Release  / Decay -- Carrier
int pot5 = A11; //Noise amount
int pot6 = A10; //FX Send / Track Vol
int pot7 = A0; //FX bits and sample-rate
int ldr = A1; //Pitch Control with Light

int Switch1 = 0; //Release / Decay Mode
int Switch2 = 1; //LED2 control
int ShiftButton = 2; //Shift for secondary functions
int button1 = 4; // button
int button2 = 5; // button

//setting up LEDS...

#define LED1 12
#define LED2 10
#define LED3 11

//Setting up BOUNCE objects using BOUNCE library for buttons(JAYCAR BUTTONS = TRASH = LONG DEBOUNCE TIME).....

Bounce Sw1 = Bounce(Switch1, 250);
Bounce Sw2 = Bounce(Switch2, 250);
Bounce Shift = Bounce(ShiftButton, 100);
Bounce BOne = Bounce(button1, 100);
Bounce BTwo = Bounce(button2, 100);

void setup() {
 //initialise serial, audio memory and the controller on the audio shield
 Serial.begin(9600); 
  
 AudioMemory (12);
 sgtl5000_1.enable();
 sgtl5000_1.volume(0.7);

 //set up Pins
 pinMode(Switch1, INPUT);
 pinMode(Switch2, INPUT);
 pinMode(ShiftButton, INPUT);
 pinMode(button1, INPUT);
 pinMode(button2, INPUT);

 pinMode(LED1, OUTPUT);
 pinMode(LED2, OUTPUT);
 pinMode(LED3, OUTPUT);

 digitalWrite(LED1, LOW);
 digitalWrite(LED2, LOW);
 digitalWrite(LED3, LOW);

 //initialise our synth engines and organise our patch...
 
 Modulator.begin(1, 440.0, WAVEFORM_SAWTOOTH);
 Carrier.begin(1, 440.0, WAVEFORM_SAWTOOTH);
 noise.amplitude(1.0);

 Modulator1.begin(1, 440.0, WAVEFORM_SAWTOOTH);
 Carrier1.begin(1, 440.0, WAVEFORM_SAWTOOTH);
 noise1.amplitude(1.0);

 modEnv.attack(0);
 modEnv.decay(25);
 modEnv.sustain(1);
 modEnv.release(500);
 
 modEnv1.attack(0);
 modEnv1.decay(25);
 modEnv1.sustain(1);
 modEnv1.release(500);
 
 modAmp.gain(1.0);
 modAmp1.gain(1.0);

 bitcrush.bits(16);
 bitcrush1.bits(16);

 mixer.gain(0,0.0);
 mixer.gain(1,0.5);
 mixer.gain(2,0.5);
 
 mixer1.gain(0,0.0);
 mixer1.gain(1,0.5);
 mixer1.gain(2,0.5);
 
 env.attack(0);
 env.decay(25);
 env.sustain(1);
 env.release(750);

 env1.attack(0);
 env1.decay(25);
 env1.sustain(1);
 env1.release(750);

 sum.gain(0,1.0);
 sum.gain(1,1.0);

}

void loop() {
  //run.....
  
  trig();
  control();
  mode_select();
  parameters();
}

//TRIG function handles note on and note off with our two "track" buttons
void trig(void) {
    BOne.update();
  BTwo.update();

  if (BOne.risingEdge()){
    env.noteOn();
    modEnv.noteOn();
    }
  if (BOne.fallingEdge()){
    env.noteOff();
    modEnv.noteOff();
    }
    if (BTwo.risingEdge()){
    env1.noteOn();
    modEnv1.noteOn();
    }
  if (BTwo.fallingEdge()){
    env1.noteOff();
    modEnv1.noteOff();
    }
  }

//control recieves data from all sensors and sends data to global variables....
void control (void) {

  //Control Pitch Ratio of modulator with Potentiometer...
  int pitchRatio = analogRead(pot1);
  int index = map(pitchRatio, 0 , 1023, 0, 8);
  myRatio = modPitchRatio[index];

  //Control Pitch of Oscillators with LDR...
  int pitchVal = analogRead(ldr);
  float F = map(pitchVal, 0, 1023, 60, 1000);
  freq = F;

  //Control Modulation Amount with potentiometer...
  int modVal = analogRead(pot2);
  float MOD = map(modVal, 0, 1023, 0, 100);
  modScaled = (MOD/100);

  //Control Release or Decay Values of modulator env

  int modRel = analogRead(pot3);
  float Rtime = map(modRel, 0, 1023, 0, 3000);
  ModulatorRDtime = Rtime;

  int Rel = analogRead(pot4);
  float rtime = map(Rel, 0, 1023, 0, 3000);
  CarrierRDtime = rtime;

  //Noise amplitude..

  int n = analogRead(pot5);
  float Namount = map(n, 0, 1023, 0, 100);
  nScaled = (Namount / 100);


  //FX and Dry Mix...
  int m = analogRead(pot6);
  float mix = map(m, 0, 1023, 0, 100);
  mixScaled = (mix / 100);
  inverseMix = 1.0 - mixScaled;

  //Bitrate

  int Bits = analogRead(pot7);
  int b = map(Bits, 0, 1023, 1, 16);
  BITS = b;

  //Shift Functions..

  Shift.update();
  if(Shift.read() == HIGH)
  {
    digitalWrite(LED3, HIGH);

    //SAMPLERATE select...
    int sampleR = analogRead(pot7);
    int rate = map(sampleR, 0, 1023, 0, 44100);
    SAMPLERATE = rate;

    int modVal = analogRead(ldr);
    float modAmount = map(modVal, 0, 1023, 0, 100);
    modScaled = 1.0 - (modAmount / 100);

    int pitchVal = analogRead(pot2);
    float f = map(pitchVal, 0, 1023, 60, 320);
    freq = f;
    
    }
    else
    {
      digitalWrite(LED3, LOW);
      }

  }

//mode_select function deals with which mode we are in and stores it in global variable.. 
//except for decay/release mode which seemed to work better in here
void mode_select (void) {
  Sw1.update();
  Sw2.update();
  
  if(Sw1.read() == HIGH) //this is decay mode...
  {       
      if(mode == true) // this is track 1
      {
      env.attack(0);
      env.decay(CarrierRDtime);
      env.sustain(0);
      env.release(1);

      modEnv.attack(0);
      modEnv.decay(ModulatorRDtime);
      modEnv.sustain(0);
      modEnv.release(1);
      }
      
      else(mode1 == true); // this is track 2
      {
      env1.attack(0);
      env1.decay(CarrierRDtime);
      env1.sustain(0);
      env1.release(1);

      modEnv1.attack(0);
      modEnv1.decay(ModulatorRDtime);
      modEnv1.sustain(0);
      modEnv1.release(1);
      }
digitalWrite(LED1, HIGH);
      } 
      
      else {   
digitalWrite(LED1,LOW);
      
      if (mode == true) //this is track 1 release mode
      {
      env.attack(0);
      env.decay(25);
      env.sustain(1);
      env.release(CarrierRDtime);

      modEnv.attack(0);
      modEnv.decay(25);
      modEnv.sustain(1);
      modEnv.release(ModulatorRDtime);
      }
      
      else (mode1 == true);{ // this is track 2 release mode
      env1.attack(0);
      env1.decay(25);
      env1.sustain(1);
      env1.release(CarrierRDtime);

      modEnv1.attack(0);
      modEnv1.decay(25);
      modEnv1.sustain(1);
      modEnv1.release(ModulatorRDtime);
      }
      }

   if(Sw2.read() == HIGH) // this is choosing between track 1 or track 2
   {
    mode1 = true;
    mode = false;
    digitalWrite(LED2, HIGH);
    } else {
      mode1 = false;
      mode = true;
      digitalWrite(LED2, LOW);
    }
  }

// the parameters functions takes our global variables and implements them to our synth engines parameters 
//and decideds which track to effect based on which mode we are in 
void parameters(){
  if (mode == true && mode1 == false)
  {
    modAmp.gain(modScaled);
    bitcrush.bits(BITS);
    bitcrush.sampleRate(SAMPLERATE);
    mixer.gain(0,nScaled);
    mixer.gain(1,mixScaled);
    mixer.gain(2,inverseMix);
    Carrier.frequency(freq);
    Modulator.frequency(freq / myRatio);
  }
  else if (mode == false && mode1 == true)
  {
    modAmp1.gain(modScaled);
    bitcrush1.bits(BITS);
    bitcrush1.sampleRate(SAMPLERATE);
    mixer1.gain(0,nScaled);
    mixer1.gain(1,mixScaled);
    mixer1.gain(2,inverseMix);
    Carrier1.frequency(freq);
    Modulator1.frequency(freq / myRatio);    
    }
  }
