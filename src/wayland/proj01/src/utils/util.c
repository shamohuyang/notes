
#include <stdio.h>
#include <sys/time.h>
#include <utils/log/log.h>

int FPS()
{
    static float step = 10.0f;
    static struct timeval t1, t2;
    static struct timezone tz;
    static float deltatime;
    static float totaltime = 0.0f;
    static unsigned int frames = 0;
    static int first_flag = 0;
    if (first_flag == 0) {
        first_flag = 1;
        gettimeofday (&t1 , &tz);
    }

    gettimeofday(&t2, &tz);
    deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
    t1 = t2;

    totaltime += deltatime;
    frames++;
    if (totaltime > step) {
        log_i("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n",
              frames, totaltime, frames/totaltime);
        totaltime -= step;
        frames = 0;
    }

    return 0;
}
