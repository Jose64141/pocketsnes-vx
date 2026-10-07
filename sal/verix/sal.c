
#include <stdio.h>
//#include <dirent.h>
#include "n2DLib.h"
//#include <sys/time.h>
#include <time.h>
#include "sal.h"
#include "menu.h"

#define PALETTE_BUFFER_LENGTH	256*2*4 

/*static SDL_Surface *mScreen = NULL;*/
static u32 mSoundThreadFlag=0;
static u32 mSoundLastCpuSpeed=0;
static u32 mPaletteBuffer[PALETTE_BUFFER_LENGTH];
static u32 *mPaletteCurr=(u32*)&mPaletteBuffer[0];
static u32 *mPaletteLast=(u32*)&mPaletteBuffer[0];
static u32 *mPaletteEnd=(u32*)&mPaletteBuffer[PALETTE_BUFFER_LENGTH];
static u32 mInputFirst=0;

s32 mCpuSpeedLookup[1]={0};

extern int console;
extern int kbd;
#include <sal_common.h> 

/*#define CASE(sym, key) \
  case SDLK_##sym: \
	inputHeld &= ~(SAL_INPUT_##key); \
	inputHeld |= type << SAL_INPUT_INDEX_##key; \
	break
*/

static u32 inputHeld = 0;
static u32 kbdInputMask = 0;

u32 verix_KbdPoll(int kbdHdl, char *key, char *clear)
{
	*clear = 0;
	read(kbd, key, 1);
	// Based on default Snes9x mapping
	if (key[0] == 0xE0) {
		int keyTemp = key[0] << 8;
		read(kbdHdl, key, 1);
		if (key[0] == 0xF0)
		{
			*clear = 1;
			read(kbdHdl, key, 1);
		}
		keyTemp |=  key[0];
		switch(keyTemp) 
		{
			case KBD_VERIX_UP: 
				return SAL_INPUT_UP;
			case KBD_VERIX_DOWN: 
				return SAL_INPUT_DOWN;
			case KBD_VERIX_LEFT: 
				return SAL_INPUT_LEFT;
			case KBD_VERIX_RIGHT: 
				return SAL_INPUT_RIGHT;
			default:
				break;
		}
	}
	
	if (key[0] == 0xF0)
	{
		*clear = 1;
		read(kbdHdl, key, 1);
	}
	switch(key[0]) 
		{
			case KBD_VERIX_V: 
				return SAL_INPUT_A;
			case KBD_VERIX_C: 
				return SAL_INPUT_B;
			case KBD_VERIX_D: 
				return SAL_INPUT_X;
			case KBD_VERIX_X: 
				return SAL_INPUT_Y;
			case KBD_VERIX_A: 
				return SAL_INPUT_L;
			case KBD_VERIX_S: 
				return SAL_INPUT_R;
				
			case KBD_VERIX_SPACE: 
				return SAL_INPUT_START;
			case KBD_VERIX_ENTER: 
				return SAL_INPUT_SELECT;
				
			case KBD_VERIX_ESC: 
				return SAL_INPUT_MENU;
			
			default:
				return 0;
		}
}

static u32 sal_Input(int held)
{
	/*SDL_Event event;
	int i=0;
	u32 timer=0;

	if (!SDL_PollEvent(&event)) {
		if (held)
			return inputHeld;
		return 0;
	}

	Uint8 type = (event.key.state == SDL_PRESSED);
	switch(event.key.keysym.sym) {
		CASE(TAB, A);
		CASE(LCTRL, B);
		CASE(MENU, X);
		CASE(BACKSPACE, Y);
		CASE(PLUS, L);
		CASE(MINUS, R);
		CASE(RETURN, START);
		CASE(SPACE, SELECT);
		CASE(UP, UP);
		CASE(DOWN, DOWN);
		CASE(LEFT, LEFT);
		CASE(RIGHT, RIGHT);
		CASE(ESCAPE, MENU);
		default: break;
	}

	mInputRepeat = inputHeld;
	*/
	
	int i=0;
	u32 inputHeld=0;
	u32 timer=0;
	char keyBuffer[20];
	char key=0;
	
	int pending = kbd_pending_count(); 
	read(console, keyBuffer, 1);
	if(pending)
	{
		key = keyBuffer[i];
		switch(key) 
		{
			case KEY_VERIX_A: 
				inputHeld|=SAL_INPUT_A;
				break;
			case KEY_VERIX_B: 
				inputHeld|=SAL_INPUT_B;
				break;
			case KEY_VERIX_X: 
				inputHeld|=SAL_INPUT_X;
				break;
			case KEY_VERIX_Y: 
				inputHeld|=SAL_INPUT_Y;
				break;
			case KEY_VERIX_L: 
				inputHeld|=SAL_INPUT_L;
				break;
			case KEY_VERIX_R: 
				inputHeld|=SAL_INPUT_R;
				break;
				
			case KEY_VERIX_START: 
				inputHeld|=SAL_INPUT_START;
				break;
			case KEY_VERIX_SELECT: 
				inputHeld|=SAL_INPUT_SELECT;
				break;
				
			case KEY_VERIX_UP: 
				inputHeld|=SAL_INPUT_UP;
				break;
			case KEY_VERIX_DOWN: 
				inputHeld|=SAL_INPUT_DOWN;
				break;
			case KEY_VERIX_LEFT: 
				inputHeld|=SAL_INPUT_LEFT;
				break;
			case KEY_VERIX_RIGHT: 
				inputHeld|=SAL_INPUT_RIGHT;
				break;
				
			case KEY_VERIX_MENU: 
				inputHeld|=SAL_INPUT_MENU;
				break;
		}
	}
	
	if ((kbd >= 0) && (read_event() & EVT_SNES_KBD)) 
	{
		char clear = 0;
		u32 kbdInput = verix_KbdPoll(kbd, keyBuffer, &clear);
		if (clear)
			kbdInputMask &= ~(kbdInput);
		else
			kbdInputMask |= kbdInput;
	}
	
	// Process key repeats
	timer=sal_TimerRead();
	for (i=0;i<19;i++)
	{
		if (inputHeld&(1<<i)) 
		{
			if(mInputFirst&(1<<i))
			{
				if (mInputRepeatTimer[i]<timer)
				{
					mInputRepeat|=1<<i;
					mInputRepeatTimer[i]=timer+10;
				}
				else
				{
					mInputRepeat&=~(1<<i);
				}
			}
			else
			{
				//First press of button
				//set timer to expire later than usual
				mInputFirst|=(1<<i);
				mInputRepeat|=1<<i;
				mInputRepeatTimer[i]=timer+50;
			}
		}
		else			
		{
			mInputRepeatTimer[i]=timer-10;
			mInputRepeat&=~(1<<i);
			mInputFirst&=~(1<<i);
		}
		
	}

	/*if(mInputIgnore)
	{
		//A request to ignore all key presses until all keys have been released has been made
		//check for release and clear flag, otherwise clear inputHeld and mInputRepeat
		if (inputHeld == 0)
		{
			mInputIgnore=0;
		}
		inputHeld=0;
		mInputRepeat=0;
	}*/

	return inputHeld | kbdInputMask;
}

static int key_repeat_enabled = 1;

u32 sal_InputPollRepeat()
{
	/*if (!key_repeat_enabled) {
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		key_repeat_enabled = 1;
	}*/
	return sal_Input(0);
}

u32 sal_InputPoll()
{
	/*if (key_repeat_enabled) {
		SDL_EnableKeyRepeat(0, 0);
		key_repeat_enabled = 0;
	}*/
	return sal_Input(1);
}

const char* sal_DirectoryGetTemp(void)
{	
	return "I:1";
	/*
	char d[] = "M:SNES/";

	struct stat s = {0};

	if (!stat(d, &s))
	{
		return "M:SNES/";
	}
	else
	{
		return "M:SNES/";
	}
	*/
}

void sal_CpuSpeedSet(u32 mhz)
{

}

u32 sal_CpuSpeedNext(u32 currSpeed)
{
	u32 newSpeed=currSpeed+1;
	if(newSpeed > 500) newSpeed = 500;
	return newSpeed;
}

u32 sal_CpuSpeedPrevious(u32 currSpeed)
{
	u32 newSpeed=currSpeed-1;
	if(newSpeed > 500) newSpeed = 0;
	return newSpeed;
}

u32 sal_CpuSpeedNextFast(u32 currSpeed)
{
	u32 newSpeed=currSpeed+10;
	if(newSpeed > 500) newSpeed = 500;
	return newSpeed;
}

u32 sal_CpuSpeedPreviousFast(u32 currSpeed)
{
	u32 newSpeed=currSpeed-10;
	if(newSpeed > 500) newSpeed = 0;
	return newSpeed;
}

s32 sal_Init(void)
{
	sal_TimerInit(60);

	memset(mInputRepeatTimer,0,sizeof(mInputRepeatTimer));

	return SAL_OK;
}

u32 sal_VideoInit(u32 bpp)
{
	initBuffering();
	clearBufferB();
	updateScreen();
	mBpp = 16;
   
	return SAL_OK;
}

u32 sal_VideoGetWidth()
{
	return 320;
}

u32 sal_VideoGetHeight()
{
	return 240;
}

u32 sal_VideoGetPitch()
{
	return 320*2;
}

void sal_VideoEnterGame(u32 fullscreenOption, u32 pal, u32 refreshRate)
{
}

void sal_VideoSetPAL(u32 fullscreenOption, u32 pal)
{
	if (fullscreenOption == 3) /* hardware scaling */
	{
		sal_VideoEnterGame(fullscreenOption, pal, mRefreshRate);
	}
}

void sal_VideoExitGame()
{
}

void sal_VideoBitmapDim(u16* img, u32 pixelCount)
{
	u32 i;
	for (i = 0; i < pixelCount; i += 2)
		*(u32 *) &img[i] = (*(u32 *) &img[i] & 0xF7DEF7DE) >> 1;
	if (pixelCount & 1) 
		img[i - 1] = (img[i - 1] & 0xF7DE) >> 1;
}

void sal_VideoFlip(s32 vsync)
{
	 updateScreen();
}

void *sal_VideoGetBuffer()
{
	return (void*)BUFF_BASE_ADDRESS;
}

void sal_VideoPaletteSync() 
{ 	
	
} 

void sal_VideoPaletteSet(u32 index, u32 color)
{
	*mPaletteCurr++=index;
	*mPaletteCurr++=color;
	if(mPaletteCurr>mPaletteEnd) mPaletteCurr=&mPaletteBuffer[0];
}

void sal_Reset(void)
{
	deinitBuffering();
}



