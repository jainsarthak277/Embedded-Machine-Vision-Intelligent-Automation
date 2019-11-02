/*
 * Simpler capture.cpp code provided by Prof. Sam Siewert
 * Authored by Sarthak Jain for the use of Embedded Machine Vision, CU Boulder
 * Date: June 21, 2019
 *
 * Reference is provided to seqgen.c for delta_t function used to calculate time 
 * difference provided by Prof. Sam Siewert during the course 
 * of  Real-Time Embedded Systems, Spring 2019.
 * Time is stamped to log file in /var/log/syslog and average frame rate is 
 * calculated basis this difference and total number of frames.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <syslog.h>
#include <sys/time.h>
#include <time.h>

#define NSEC_PER_SEC	(1000000000)
#define FRAMES		(30)

using namespace cv;
using namespace std;


int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }
  else
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }

  return(1);
}

int main( int argc, char** argv )
{
    struct timespec start_time, stop_time, diff_time, current_time;
    long frame_cnt = 0;
    int no_secs = 0, fps = FRAMES;
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(0);
    IplImage* frame;

    clock_gettime(CLOCK_REALTIME, &start_time);
    while(1)
    {
        frame=cvQueryFrame(capture);
   	clock_gettime(CLOCK_REALTIME, &current_time);
	syslog(LOG_CRIT, "Time-stamp image save to file @ sec = %lu, nsec = %lu\n", current_time.tv_sec - start_time.tv_sec, current_time.tv_nsec);
     
        if(!frame) break;

	frame_cnt++;
	clock_gettime(CLOCK_REALTIME, &stop_time);
	stop_time.tv_sec -= no_secs;
	delta_t(&stop_time, &start_time, &diff_time);
	if(diff_time.tv_sec > 1)
	{
		printf("in loop");
		no_secs++;
		if((frame_cnt / no_secs) < fps)
			fps = frame_cnt / no_secs;
	}
        cvShowImage("Capture Example", frame);

        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    clock_gettime(CLOCK_REALTIME, &stop_time);
    delta_t(&stop_time, &start_time, &diff_time);
    printf("\nframes = %lu", (frame_cnt));
    printf("\nTime = %lu s : %lu ns", diff_time.tv_sec, diff_time.tv_nsec);
    printf("\nAverage frame rate = %lu fps", (frame_cnt/diff_time.tv_sec));
    printf("\nWC frame rate = %d fps", fps);
    printf("\nJitter = %lu\n", (int)(diff_time.tv_sec*30) - frame_cnt);
   
    cvReleaseCapture(&capture);
    cvDestroyWindow("Capture Example");

};
