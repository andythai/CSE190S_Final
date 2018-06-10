/* Audio class to play BGMs, sound effects, etc
 * Made by Ronald Allan Baldonado and Andy Thai
 */
#pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include "al.h"
#include "alc.h""
#endif
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define TEST_ERROR(_msg)			\
	error = alGetError();			\
	if (error != AL_NO_ERROR) {		\
		fprintf(stderr, _msg "\n");	\
		return;						\
	}

using namespace std;
class Audio {
public:
	/* Data */
	ALCdevice * device;
	ALCcontext* context;
	ALenum format;
	std::vector<ALuint> buffers;
	std::vector<ALuint> sources;
	ALCenum error;
	ALint source_state;
	bool muteAll = false;

	/* Functions */
	Audio(std::vector<string> filenames);	// Constructor
	~Audio();								// Destructor

	void toggleMuteAll();					// Turn mute on or off
	void play(int index);					// Play music at given index

	// Check if machine is Big Endian
	static bool checkBigEndian() {
		int i = 1;
		return !((char*)&i)[0];
	}

	// Convert given buffer to an int 
	static int convertToInt(char* buffer, int len) {
		int a = 0;
		if (!checkBigEndian())
			for (int i = 0; i < len; i++)
				((char*)&a)[i] = buffer[i];
		else
			for (int i = 0; i < len; i++)
				((char*)&a)[3 - i] = buffer[i];
		return a;
	}
};
#endif /* AUDIO_H */