#include "Source.h"

AudioEffect* createEffectInstance(audioMasterCallback am) {
	return new RevIN(am);
}

RevIN::RevIN(audioMasterCallback a) : AudioEffectX(a, 1, 92) {
	setNumInputs(2);		// stereo in
	setNumOutputs(2);		// stereo out
	setUniqueID('ReIN');	// identify
	canProcessReplacing(true);	// supports replacing output
}

RevIN::~RevIN() {}

void RevIN::setProgramName(char* name) {}
void RevIN::getProgramName(char* name) { vst_strncpy(name, "RevIN", kVstMaxProgNameLen); }

void RevIN::setParameter(VstInt32 index, float value) {
	if (index > 1 && index < 66) { // matrix
		m[(index - 2) & 7][((index - 2) >> 3) & 7] = value * 2.f - 1.f;
		return;
	} if (index > 65 && index < 74) { // out mixing
		D[index - 66] = value * 2.f - 1.f;
		return;
	} if (index > 73 && index < 82) { // route
		unsigned char i = index - 74;
		if (value > 0.75f) {
			if (i == 7) {
				unsigned char t = r[0];
				r[0] = r[7];
				r[7] = t;
				return;
			}
			else {
				unsigned char t = r[i + 1];
				r[i + 1] = r[i];
				r[i] = t;
				return;
			}
		}
		if (value < 0.25f) {
			if (i == 0) {
				unsigned char t = r[7];
				r[7] = r[0];
				r[0] = t;
				return;
			}
			else {
				unsigned char t = r[i - 1];
				r[i - 1] = r[i];
				r[i] = t;
				return;
			}
		}
	} if (index > 81 && index < 89) { // next gain 
		M[index - 82] = value;
		return;
	}
	switch (index) {
	case 0: // size
		s = (value * 3.f) + 1.f;
		break;
	case 1: // decay
		d = value * .95f;
		break;
	case 89: // pm lc
		lc[0] = value;
		break;
	case 90: // Pm lc
		lc[1] = value;
		break;
	case 91: // mix
		mix = value;
		break;
	}
}
float RevIN::getParameter(VstInt32 index) {
	if (index > 1 && index < 66) { // matrix
		(m[(index - 2) & 7][((index - 2) >> 3) & 7] - 1.f) * .5f;
		return;
	} if (index > 65 && index < 74) { // out mixing
		D[index - 66] = value * 2.f - 1.f;
		return;
	} if (index > 73 && index < 82) { // route
		unsigned char i = index - 74;
		if (value > 0.75f) {
			if (i == 7) {
				unsigned char t = r[0];
				r[0] = r[7];
				r[7] = t;
				return;
			}
			else {
				unsigned char t = r[i + 1];
				r[i + 1] = r[i];
				r[i] = t;
				return;
			}
		}
		if (value < 0.25f) {
			if (i == 0) {
				unsigned char t = r[7];
				r[7] = r[0];
				r[0] = t;
				return;
			}
			else {
				unsigned char t = r[i - 1];
				r[i - 1] = r[i];
				r[i] = t;
				return;
			}
		}
	} if (index > 81 && index < 89) { // next gain 
		M[index - 82] = value;
		return;
	}
	switch (index) {
	case 0: // size
		s = (value * 3.f) + 1.f;
		break;
	case 1: // decay
		d = value * .95f;
		break;
	case 89: // pm lc
		lc[0] = value;
		break;
	case 90: // Pm lc
		lc[1] = value;
		break;
	case 91: // mix
		mix = value;
		break;
	}
}
void RevIN::getParameterLabel(VstInt32 index, char* label) {
	if (index != 1 && index < 89) {
		label[0] = '*';
		label[1] = '\0';
	}
	else {
		label[0] = '%';
		label[1] = '\0';
	}
}
void RevIN::getParameterDisplay(VstInt32 index, char* text) {
	if (index > 1 && index < 66) { // matrix
		float2string(m[(index - 2) & 7][((index - 2) >> 3) & 7], text, 4);
		return;
	} if (index > 65 && index < 74) { // out mixing
		float2string(D[index - 66], text, 4);
		return;
	} if (index > 73 && index < 82) { // route
		text[0] = '\0';
		return;
	} if (index > 81 && index < 89) { // next gain 
		float2string(M[index - 82], text, 4);
		return;
	}
	switch (index) {
	case 0: // size
		float2string(s, text, 4);
		break;
	case 1: // decay
		float2string(d, text, 4);
		break;
	case 89: // pm lc
		float2string(lc[0], text, 4);
		break;
	case 90: // Pm lc
		float2string(lc[1], text, 4);
		break;
	case 91: // mix
		float2string(mix, text, 4);
		break;
	}
}
void RevIN::getParameterName(VstInt32 index, char* text) {
	if (index > 1 && index < 66) { // matrix
		text[0] = 'M';
		text[1] = ((index - 2) & 7)  + '1';
		text[2] = (((index - 2) >> 3) & 7) + '1';
		text[3] = '\0';
		return;
	} if (index > 65 && index < 74) { // out mixing
		text[0] = 'd';
		text[1] = ((index - 66) & 7) + '1';
		text[2] = '\0';
		return;
	} if (index > 73 && index < 82) { // route
		text[0] = 'r';
		text[1] = r[index - 74] + '1';
		text[2] = '\0';
		return;
	} if (index > 81 && index < 89) { // next gain 
		vst_strncpy(text, "gain", 4);
		text[4] = ' ';
		text[5] = (index - 82) + '1'; 
		text[6] = '\0';
		return;
	}
	switch (index) {
	case 0:
		vst_strncpy(text, "size", 4);
		break;
	case 1:
		vst_strncpy(text, "decay", 5);
		break;
	case 89:
		vst_strncpy(text, "lowcut0", kVstMaxParamStrLen); // pre matrix low cut
		break;
	case 90:
		vst_strncpy(text, "lowcut1", kVstMaxParamStrLen); // post matrix low cut
		break;
	case 91:
		vst_strncpy(text, "mix", kVstMaxParamStrLen);
		break;
	}
}
bool RevIN::getEffectName(char* name) { vst_strncpy(name, "RevIN", kVstMaxEffectNameLen); return true; }
VstPlugCategory RevIN::getPlugCategory() { return kPlugCategRoomFx; }
bool RevIN::getVendorString(char* text) { vst_strncpy(text, "butte", kVstMaxVendorStrLen); return true; }
bool RevIN::getProductString(char* text) { vst_strncpy(text, "RevIN", kVstMaxProductStrLen); return true; }
VstInt32 RevIN::getVendorVersion() { return 1000; }

