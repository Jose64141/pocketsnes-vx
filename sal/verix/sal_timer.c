#include <sal.h>
#include <time.h>

static u32 mFrameTime;  

//static u32 tv_sec = 0, tv_usec = 0, tv_nsec = 0;

u32 sal_TimerRead()
{
	return 1000 * read_ticks() / mFrameTime; // 1 tick / msec
	//struct timespec tval; // timing
  
  	//clock_gettime(CLOCK_REALTIME, &tval);
	//return ((tval.tv_sec * 1000000) + tval.tv_nsec / 1000) / mFrameTime;
}

s32 sal_TimerInit(s32 frametime)
{
	mFrameTime = frametime;
   	return SAL_OK;
}

void sal_TimerClose(void) 
{
}
