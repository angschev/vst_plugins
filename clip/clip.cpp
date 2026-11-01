#include "clip.h"

#define NOMINMAX
#include <windows.h>

const char* labels[3] = {
	"MIX", "THRESHOLD", "CLOGGING"
};

#define min(x,m) ((x)>(m))?(m):(x)
#define max(x,m) ((x)<(m))?(m):(x)
#define clamp(x,a,b) min(max(x,b),a) // min and max already parenthesizes so no need
#define sign(x) ((x) < 0.f)?-1.f:1.f
#define abs(x) (x)<0.f?(-x):(x)

#define eps 0.00000000001f

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
#ifdef EXTDEB
	MessageBoxA(
		NULL,
		"1",
		"Clipz Debug",
		MB_OK | MB_ICONINFORMATION
	);
#endif
	return new Clipz(audioMaster);
}

Clipz::Clipz(audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, 3) {
#ifdef EXTDEB
	MessageBoxA(
		NULL,
		"2",
		"Clipz Debug",
		MB_OK | MB_ICONINFORMATION
	);
#endif
	setNumInputs(2); setNumOutputs(2);
	setUniqueID('Clpz');
	canProcessReplacing();

	mix = 0.0f;
	thr = inv = inv1 = 1.0f;
	clg = 0.5f;
	vst_strncpy(programName, "Clipz", 5);
#ifdef EXTDEB
	MessageBoxA(
		NULL,
		"3",
		"Clipz Debug",
		MB_OK | MB_ICONINFORMATION
	);
#endif
}

Clipz::~Clipz() {}

void Clipz::setProgramName(char* name) { vst_strncpy(programName, name, kVstMaxProgNameLen); }
void Clipz::getProgramName(char* name) { vst_strncpy(name, programName, kVstMaxProgNameLen); }

void Clipz::setParameter(VstInt32 idx, float value) {
	value = clamp(value, 1, 0);
	switch (idx) {
	case 0:
		mix = value; break;
	case 1:
		thr = value;
		if (value == 1.f || value == 0.f) {
			inv = inv1 = 0.f;
		} else { 
			inv = 1.f / value;
			inv1 = 1.f / (1.f - value);
		}
		break;
	case 2:
		clg = value; break;
	}
}
float Clipz::getParameter(VstInt32 idx) {
	float o = 0.f;
	switch (idx) {
	case 0:
		o = mix; break;
	case 1:
		o = thr; break;
	case 2:
		o = clg; break;
	}
	return o;
}
void Clipz::getParameterLabel(VstInt32 index, char* label) { vst_strncpy(label, labels[index], 10); }
void Clipz::getParameterDisplay(VstInt32 index, char* text) { 
	switch (index) {
	case 0: float2string(mix, text, 3); break;
	case 1: float2string(thr, text, 3); break;
	case 2: float2string(clg, text, 3); break;
	}
}
void Clipz::getParameterName(VstInt32 index, char* text)    { vst_strncpy(text, labels[index], 10); }

VstPlugCategory Clipz::getPlugCategory() { return kPlugCategEffect; }
bool Clipz::getVendorString(char* text) {
	vst_strncpy(text, "butte", 5);
	return true;
}
bool Clipz::getEffectName(char* name) {
	vst_strncpy(name, programName, kVstMaxEffectNameLen);
	return true;
}
bool Clipz::getProductString(char* text) {
	vst_strncpy(text, programName, kVstMaxProductStrLen);
	return true;
}
VstInt32 Clipz::getVendorVersion() { return 1000; }

void Clipz::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) {
	float* i0 = inputs[0], * i1 = inputs[1];
	float* o0 = outputs[0], *o1 = outputs[1];

	while (--sampleFrames >= 0) {
		z = x = 0.f;
		if (thr == 0.f) {
			if (abs(*i0) > eps) z = *i0 + ((sign(*i0) - *i0) * (1.f-clg));
			if (abs(*i1) > eps) x = *i1 + ((sign(*i1) - *i1) * (1.f-clg));
		} else if (thr == 1.f) {
			z = *i0 * (1.f-clg);
			x = *i1 * (1.f-clg);
		} else {
			if (abs(*i0) > eps) {
					c = clamp(*i0, thr, -thr);
					v = (*i0 - c) * inv1;
					c *= inv;
				z = c + ((v - c) * clg);
			}
			if (abs(*i1) > eps) {
					c = clamp(*i1, thr, -thr);
					v = (*i1 - c) * inv1;
					c *= inv;
				x = c + ((v - c) * clg);
			} 
		}

		*o0++ = *i0 + ((z-*i0++) * mix);
		*o1++ = *i1 + ((x-*i1++) * mix);
	}
}