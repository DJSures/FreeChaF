/*
	This file is part of FreeChaF.

	FreeChaF is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FreeChaF is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeChaF.  If not, see http://www.gnu.org/licenses/
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "libretro.h"
#include <file/file_path.h>
#include <retro_miscellaneous.h>

#include "memory.h"
#include "channelf.h"
#include "controller.h"
#include "audio.h"
#include "video.h"
#include "osd.h"

#define DefaultFPS 60
#define frameWidth 306
#define frameHeight 192
#define frameSize 58752

unsigned int frame[frameSize];

char *SystemPath;

retro_environment_t Environ;
retro_video_refresh_t Video;
retro_audio_sample_t Audio;
retro_audio_sample_batch_t AudioBatch;
retro_input_poll_t InputPoll;
retro_input_state_t InputState;

void retro_set_environment(retro_environment_t fn) { Environ = fn; }
void retro_set_video_refresh(retro_video_refresh_t fn) { Video = fn; }
void retro_set_audio_sample(retro_audio_sample_t fn) { Audio = fn; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t fn) { AudioBatch = fn; }
void retro_set_input_poll(retro_input_poll_t fn) { InputPoll = fn; }
void retro_set_input_state(retro_input_state_t fn) { InputState = fn; }

struct retro_game_geometry Geometry;

int joypad0[26]; // joypad 0 state
int joypad1[26]; // joypad 1 state
int joypre0[26]; // joypad 0 previous state
int joypre1[26]; // joypad 1 previous state

bool console_input = false;

// at 44.1khz, read 735 samples (44100/60) 
int audioSamples = 735;

static void Keyboard(bool down, unsigned keycode,
      uint32_t character, uint16_t key_modifiers)
{
	/* Keyboard Input */
}

void retro_init(void)
{
	char PSU_1_Update_Path[PATH_MAX_LENGTH];
	char PSU_1_Path[PATH_MAX_LENGTH];
	char PSU_2_Path[PATH_MAX_LENGTH];
	struct retro_keyboard_callback kb = { Keyboard };

	// init buffers, structs
	memset(frame, 0, frameSize*sizeof(unsigned int));

	OSD_setDisplay(frame, frameWidth, frameHeight);

	// init console
	CHANNELF_init();

	// get paths
	Environ(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &SystemPath);

	// load PSU 1 Update
	fill_pathname_join(PSU_1_Update_Path, SystemPath, "sl90025.bin", PATH_MAX_LENGTH);
	if(!CHANNELF_loadROM(PSU_1_Update_Path, 0))
	{
		printf("[ERROR] [FREECHAF] Failed loading Channel F II BIOS(1) from: %s\n", PSU_1_Update_Path);

		// load PSU 1 Original
		fill_pathname_join(PSU_1_Path, SystemPath, "sl31253.bin", PATH_MAX_LENGTH);
		if(!CHANNELF_loadROM(PSU_1_Path, 0))
		{
			printf("[ERROR] [FREECHAF] Failed loading Channel F BIOS(1) from: %s\n", PSU_1_Path);
			exit(0);
		}
	}

	// load PSU 2
	fill_pathname_join(PSU_2_Path, SystemPath, "sl31254.bin", PATH_MAX_LENGTH);
	if(!CHANNELF_loadROM(PSU_2_Path, 0x400))
	{
		printf("[ERROR] [FREECHAF] Failed loading Channel F BIOS(2) from: %s\n", PSU_2_Path);
		exit(0);
	}

	// Setup keyboard input
	Environ(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &kb);
}

bool retro_load_game(const struct retro_game_info *info)
{
	return CHANNELF_loadROM(info->path, 0x800);
}

void retro_unload_game(void)
{

}

void retro_run(void)
{
	int i = 0;
	bool consolePressedShow = false;
	int consolePressedIndex = 0;

	InputPoll();

	for(i=0; i<18; i++) // Copy previous state 
	{
		joypre0[i] = joypad0[i];
		joypre1[i] = joypad1[i];
	}

	/* JoyPad 0 */

	joypad0[0] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
	joypad0[1] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
	joypad0[2] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
	joypad0[3] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);

	joypad0[4] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
	joypad0[5] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
	joypad0[6] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
	joypad0[7] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);

	joypad0[8] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
	joypad0[9] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);

// 	DJ: Removed because these were being picked up by the START/SELECT buttons on J0
//	joypad0[10] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L);
//	joypad0[11] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R);
//	joypad0[12] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
//	joypad0[13] = InputState(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);

	joypad0[14] = InputState(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
	joypad0[15] = InputState(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
	joypad0[16] = InputState(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
	joypad0[17] = InputState(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);

	// DJ: added the first joystick to also have push/pull/tiwst ability lke the second joystick did
	// analog up, down, left, right
	// left analog:  18,19,20,21
	// right analog: 22,23,24,25
	joypad0[18] = (joypad0[15]/4096) <-1; // ALeft UP
	joypad0[19] = (joypad0[15]/4096) > 1; // ALeft DOWN
	joypad0[20] = (joypad0[14]/4096) <-1; // ALeft LEFT
	joypad0[21] = (joypad0[14]/4096) > 1; // ALeft RIGHT
	joypad0[22] = (joypad0[17]/4096) <-1; // ARight UP
	joypad0[23] = (joypad0[17]/4096) > 1; // ARight DOWN
	joypad0[24] = (joypad0[16]/4096) <-1; // ARight LEFT
	joypad0[25] = (joypad0[16]/4096) > 1; // ARight RIGHT

	/* JoyPad 1 */

	joypad1[0] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
	joypad1[1] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
	joypad1[2] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
	joypad1[3] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);

	joypad1[4] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
	joypad1[5] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
	joypad1[6] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
	joypad1[7] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);

	joypad1[8] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
	joypad1[9] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);

// 	DJ: Removed because these were being picked up by the START/SELECT buttons on J0
//	joypad1[10] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L);
//	joypad1[11] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R);
//	joypad1[12] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
//	joypad1[13] = InputState(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);

	joypad1[14] = InputState(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
	joypad1[15] = InputState(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
	joypad1[16] = InputState(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
	joypad1[17] = InputState(1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);

	// analog up, down, left, right
	// left analog:  18,19,20,21
	// right analog: 22,23,24,25
	joypad1[18] = (joypad1[15]/4096) <-1; // ALeft UP
	joypad1[19] = (joypad1[15]/4096) > 1; // ALeft DOWN
	joypad1[20] = (joypad1[14]/4096) <-1; // ALeft LEFT
	joypad1[21] = (joypad1[14]/4096) > 1; // ALeft RIGHT
	joypad1[22] = (joypad1[17]/4096) <-1; // ARight UP
	joypad1[23] = (joypad1[17]/4096) > 1; // ARight DOWN
	joypad1[24] = (joypad1[16]/4096) <-1; // ARight LEFT
	joypad1[25] = (joypad1[16]/4096) > 1; // ARight RIGHT

	// swap console/controller input //
	if((joypad0[8]==1 && joypre0[8]==0) || (joypad1[8]==1 && joypre1[8]==0))
	{
		console_input = !console_input;
	}

	// swap left/right controllers //
	// DJ: We got real hardware so no need for this
//	if((joypad0[9]==1 && joypre0[9]==0) || (joypad1[9]==1 && joypre1[9]==0))
//	{
//		CONTROLLER_swap();
//	}

	if(console_input) // console input
	{
		if((joypad0[2]==1 && joypre0[2]==0) || (joypad1[2]==1 && joypre1[2]==0)) // left
		{
			CONTROLLER_consoleInput(0, 1);
		}
		if((joypad0[20]==1 && joypre0[20]==0) || (joypad1[20]==1 && joypre1[20]==0)) // left analog left
		{
			CONTROLLER_consoleInput(0, 1);
		}
		if((joypad0[24]==1 && joypre0[24]==0) || (joypad1[24]==1 && joypre1[24]==0)) // right analog left
		{
			CONTROLLER_consoleInput(0, 1);
		}

		if((joypad0[3]==1 && joypre0[3]==0) || (joypad1[3]==1 && joypre1[3]==0)) // right
		{
			CONTROLLER_consoleInput(1, 1);
		}
		if((joypad0[22]==1 && joypre0[22]==0) || (joypad1[22]==1 && joypre1[22]==0)) // left analog right
		{
			CONTROLLER_consoleInput(1, 1);
		}
		if((joypad0[25]==1 && joypre0[25]==0) || (joypad1[25]==1 && joypre1[25]==0)) // right analog right
		{
			CONTROLLER_consoleInput(1, 1);
		}

		for(i=4; i<8; i++)
		{
			if((joypad0[i]==1 && joypre0[i]==0) || (joypad1[i]==1 && joypre1[i]==0)) // a,b,x,y
			{
				CONTROLLER_consoleInput(2, 1);
			}
			if((joypad0[i]==0 && joypre0[i]==1) || (joypad1[i]==0 && joypre1[i]==1)) // a,b,x,y
			{
				CONTROLLER_consoleInput(2, 0);
			}
		}

	}
	else
	{
		// DJ: Console buttons
		// Call dj's new Controller set button in game play if the console assigned buttons are pressed
		for(i=4; i<8; i++)
		{
			if(joypad0[i]==1 || joypad1[i]==1) // a,b,x,y
			{
				consolePressedShow = true;
				consolePressedIndex = i - 3;

				if(joypre0[i]==0 || joypre1[i]==0) // a,b,x,y
				{
					CONTROLLER_setButton(0, i - 4, 1);
				}
			}

			if((joypad0[i]==0 && joypre0[i]==1) || (joypad1[i]==0 && joypre1[i]==1)) // a,b,x,y
			{
				CONTROLLER_setButton(0, i - 4, 0);
			}
		}


		// DJ: Removed input from buttons that are now console switches
		// The console switches are checked above, and here we only process joystick movements
		// ordinary controller input
		CONTROLLER_setInput(1,
		((joypad0[23])<<7)|               /* push             - ALeft Down  -         */
		((joypad0[22])<<6)|               /* pull             - ALeft Up    -         */
		((joypad0[25] | joypad0[11])<<5)| /* rotate right     - ALeft Right - ShRight */
		((joypad0[24] | joypad0[10])<<4)| /* rotate left      - ALeft rLeft - ShLeft  */
		((joypad0[0] | joypad0[18])<<3)|               /* forward      - Up   - ARight Up   -         */
		((joypad0[1] | joypad0[19])<<2)|               /* back         - Down - ARight Down -         */
		((joypad0[2] | joypad0[20])<<1)|               /* left         - Left - ARight Left -         */
		((joypad0[3] | joypad0[21])) );                /* right        - Right- ARight Right-         */

		CONTROLLER_setInput(2,
		((joypad1[23])<<7)|               /* push             - ALeft Down  -         */
		((joypad1[22])<<6)|               /* pull             - ALeft Up    -         */
		((joypad1[25] | joypad1[11])<<5)| /* rotate right     - ALeft Right - ShRight */
		((joypad1[24] | joypad1[10])<<4)| /* rotate left      - ALeft rLeft - ShLeft  */
		((joypad1[0] | joypad1[18])<<3)|               /* forward      - Up   - ARight Up   -         */
		((joypad1[1] | joypad1[19])<<2)|               /* back         - Down - ARight Down -         */
		((joypad1[2] | joypad1[20])<<1)|               /* left         - Left - ARight Left -         */
		((joypad1[3] | joypad1[21])) );                /* right        - Right- ARight Right-         */
	}

	// grab frame
	CHANNELF_run();

	// sample audio from buffer
	for(i=0; i<audioSamples; i++)
	{
		Audio(AUDIO_Buffer[i], AUDIO_Buffer[i]); // Audio(left, right)
	}
	AUDIO_frame(); // notify audio to start new audio frame

	// send frame to libretro
	VIDEO_drawFrame();

	// 3x upscale (gives more resolution for OSD)
	int offset = 0;
	int color = 0;
	int row;
	int col;

	for(row=0; row<64; row++)
	{
		offset = (row*3)*306;
		for(col=0; col<102; col++)
		{
			color =  VIDEO_Buffer[row*128+col+4];
			frame[offset]   = color;
			frame[offset+1] = color;
			frame[offset+2] = color;

			frame[offset+306] = color;
			frame[offset+307] = color;
			frame[offset+308] = color;

			frame[offset+612] = color;
			frame[offset+613] = color;
			frame[offset+614] = color;
			offset+=3;
		}
	}

	// DJ; Real hardware, no need for this and we don't want this
	// OSD
//	if((joypad0[9]==1) || (joypad1[9]==1)) // Show Controller Swap State 
//	{
//		if(CONTROLLER_swapped())
//		{
//			OSD_drawP1P2();
//		}
//		else
//		{
//			OSD_drawP2P1();
//		}
//	}

	if(console_input) // Show Console Buttons
	{
		 OSD_drawConsole(CONTROLLER_cursorPos(), CONTROLLER_cursorDown());
	}

	if (consolePressedShow)
	{
		 OSD_drawConsole(consolePressedIndex, consolePressedIndex);
	}

	// Output video
	Video(frame, frameWidth, frameHeight, sizeof(unsigned int) * frameWidth);
}

unsigned retro_get_region(void)
{
	return RETRO_REGION_NTSC;
}

void retro_get_system_info(struct retro_system_info *info)
{
	memset(info, 0, sizeof(*info));
	info->library_name = "FreeChaF";
	info->library_version = "1.0";
	info->valid_extensions = "bin|rom";
	info->need_fullpath = true;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   int pixelformat = RETRO_PIXEL_FORMAT_XRGB8888;

	memset(info, 0, sizeof(*info));
	info->geometry.base_width   = frameWidth;
	info->geometry.base_height  = frameHeight;
	info->geometry.max_width    = frameWidth;
	info->geometry.max_height   = frameHeight;
	info->geometry.aspect_ratio = ((float)frameWidth) / ((float)frameHeight);

	info->timing.fps = DefaultFPS;
	info->timing.sample_rate = 44100.0;

	Environ(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixelformat);
}


void retro_deinit(void)
{

}

void retro_reset(void)
{
	CHANNELF_reset();
}

/* Stubs */
unsigned int retro_api_version(void) { return RETRO_API_VERSION; }
void *retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id){ return 0; }
size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void *data, size_t size) { return false; }
bool retro_unserialize(const void *data, size_t size) { return false; }
void retro_cheat_reset(void) {  }
void retro_cheat_set(unsigned index, bool enabled, const char *code) {  }
bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) { return false; }
void retro_set_controller_port_device(unsigned port, unsigned device) {  }
