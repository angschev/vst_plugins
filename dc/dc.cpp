#include "dc.h"

#define min(x,m) (((x)>(m))?(m):(x))
#define max(x,m) (((x)<(m))?(m):(x))
#define clamp(x,a,b) min(max(x,b),a) // min and max already parenthesizes so no need
#define sign(x) (((x) < 0.f)?-1.f:1.f)
#define abs(x) ((x)<0.f?-(x):(x))

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
	return new DCorrect(audioMaster);
}

DCorrect::DCorrect(audioMasterCallback am) : AudioEffectX(am, 1, 1) {
	setNumInputs(2); setNumOutputs(2);
	canProcessReplacing();
	setUniqueID('DCor');

	size = psize = 1;
	invs = 1.f;
	head = 0;
	vst_strncpy(pn, "DCorrect", 8);
}

DCorrect::~DCorrect() {}

void DCorrect::setProgramName(char* name) { vst_strncpy(pn, name, 8); }
void DCorrect::getProgramName(char* name) { vst_strncpy(name, pn, 8); }

void DCorrect::setParameter(VstInt32 idx, float val) {
	val = clamp(val, 1.f, 0.f);
	size = (short)((val * 2047.f) + 1.f);
	invs = 1.f / size;
	if (size < psize) for (short i = size; i < 2048; i++) buff[i][0] = buff[i][1] = 0.f;
	if (head >= size) head = 0;
	psize = size;
	dc[0] = dc[1] = 0.f;
}
float DCorrect::getParameter(VstInt32 idx) {
	return (float)(size - 1) / 2047.f;
}
void DCorrect::getParameterLabel(VstInt32 idx, char* text) { text[0] = 'S'; text[1] = '\0'; }
void DCorrect::getParameterDisplay(VstInt32 idx, char* text) { int2string(size, text, 4); }
void DCorrect::getParameterName(VstInt32 idx, char* text) { vst_strncpy(text, "SIZE", 4); }

VstPlugCategory DCorrect::getPlugCategory() { return kPlugCategRestoration; }
bool DCorrect::getVendorString(char* text) {
	vst_strncpy(text, "butte", 5);
	return true;
}
bool DCorrect::getEffectName(char* name) {
	vst_strncpy(name, pn, kVstMaxEffectNameLen);
	return true;
}
bool DCorrect::getProductString(char* text) {
	vst_strncpy(text, pn, kVstMaxProductStrLen);
	return true;
}
VstInt32 DCorrect::getVendorVersion() { return 1000; }

void DCorrect::processReplacing(float **in, float **out, VstInt32 frames) {
	float* i0 = in[0], * i1 = in[1];
	float* o0 = out[0],* o1 = out[1];

	while (frames--) {
		x[0] = *i0;
		x[1] = *i1;
		x[0] -= buff[head][0];
		x[1] -= buff[head][1];
		buff[head][0] = *i0;
		buff[head][1] = *i1;

		dc[0] += x[0] * invs;
		dc[1] += x[1] * invs;

		if (++head >= size) head = 0;

		*o0++ = *i0++ - dc[0];
		*o1++ = *i1++ - dc[1];
	}
}