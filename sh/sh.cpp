#include "sh.h"

#define min(x,m) (((x)>(m))?(m):(x))

const char* labels[4] = {
	"MIX", "UPDATE RATE", "INTERPOLATE", "NOISE"
};

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new Bity(audioMaster);
}

Bity::Bity(audioMasterCallback audioMaster)
	: AudioEffectX(audioMaster, 1, 4)
{
	setNumInputs(2);		// stereo in
	setNumOutputs(2);		// stereo out
	setUniqueID('Bity');	// identify
	canProcessReplacing(true);	// supports replacing output

	fMix = 0.f;
	updt = 1;
	lerp = 0.f;
	noiz = 0.f;
	r = 0.f;
	rs = 0;
	c = 0;

	vst_strncpy(programName, "Bity", kVstMaxProgNameLen);	// default program name
}

Bity::~Bity()
{
	// nothing to do here
}

void Bity::setProgramName(char* name) {
	vst_strncpy(programName, name, kVstMaxProgNameLen);
}

void Bity::getProgramName(char* name) {
	vst_strncpy(name, programName, kVstMaxProgNameLen);
}

void Bity::setParameter(VstInt32 index, float value) {
	switch (index) {
	case 0:
		fMix = value;
		break;
	case 1:
		updt = (short)(value * 1023.f + 1.f);
		r = 1.f / (float)updt;
		break;
	case 2:
		lerp = value;
		break;
	case 3:
		noiz = value;
		break;
	}
}

float Bity::getParameter(VstInt32 index)
{
	float o = 0.f;
	switch (index) {
	case 0:
		o = fMix;
		break;
	case 1:
		o = ((float)updt - 1.f) / 1023.f;
		break;
	case 2:
		o = lerp;
		break;
	case 3:
		o = noiz;
		break;
	}
	return o;
}

void Bity::getParameterName(VstInt32 index, char* label)
{
	vst_strncpy(label, labels[index], 12);
}

void Bity::getParameterDisplay(VstInt32 index, char* text)
{
	switch (index) {
	case 0:
		float2string(fMix, text, 3);
		break;
	case 1:
		int2string(updt, text, 4);
		break;
	case 2:
		float2string(lerp, text, 3);
		break;
	case 3:
		float2string(noiz, text, 3);
		break;
	}
}

void Bity::getParameterLabel(VstInt32 index, char* label)
{
	vst_strncpy(label, labels[index], 12);
}

bool Bity::getEffectName(char* name)
{
	vst_strncpy(name, programName, kVstMaxEffectNameLen);
	return true;
}

bool Bity::getProductString(char* text)
{
	vst_strncpy(text, programName, kVstMaxProductStrLen);
	return true;
}

bool Bity::getVendorString(char* text)
{
	vst_strncpy(text, "butte", kVstMaxVendorStrLen);	
	return true;
}

VstInt32 Bity::getVendorVersion()
{
	return 1000;
}

void Bity::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in1 = inputs[0];
	float* in2 = inputs[1];
	float* out1 = outputs[0];
	float* out2 = outputs[1];
	float noise = 0.f;
	float N = 0.f;

	while (--sampleFrames >= 0)
	{
		const float dryL = *in1++;
		const float dryR = *in2++;

		if (c == 0) {
			// Remember previous sample
			z1 = z;
			x1 = x;

			// Generate noise
			rs = rs * 1664525u + 1013904223u;
			float noise = ((float)rs / 4294967295.f) * 2.f - 1.f;
			float N = (1.f - noiz) + noise * noiz;

			// Capture new sample
			z = dryL * N;
			x = dryR * N;
		}

		// Interpolation duration
		float rampLen = lerp * (float)updt;

		float t = (rampLen < 1.f)
			? 1.f
			: (float)(c + 1) / rampLen;

		if (t > 1.f) t = 1.f;

		float wetL = z1 + (z - z1) * t;
		float wetR = x1 + (x - x1) * t;

		*out1++ = dryL + (wetL - dryL) * fMix;
		*out2++ = dryR + (wetR - dryR) * fMix;

		if (++c >= updt) c = 0;
	}
}