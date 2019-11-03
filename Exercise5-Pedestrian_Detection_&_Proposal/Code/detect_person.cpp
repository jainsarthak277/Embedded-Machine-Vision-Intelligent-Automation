/* 
 * Author: Sarthak Jain
 * Dated: July 26, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 * Titled: Program for detection of pedestrians suing OpenCV HoG extractors
 * for the use of Embedded Machine Vision under Prof. Sam Siewert, CU boulder
 *
 * The code is designed to generate a live video of any objects in front of the camera. 
 * it produces a set of frames, shown as a video in the code. Each frame draws a bounding box around the persons it detects
 * The program uses an algorithm using OpenCV's HoG class to extract HoG features and detect people. 
 * Reference provided to pyimagesearch for the algorithm : https://www.pyimagesearch.com/2015/11/09/pedestrian-detection-opencv/
 * VideoCapture class is used open the camera stream and write the output video.
 * Provide input video name as argument.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;
using namespace std;

#define COLS	(320)
#define ROWS	(240)
#define NSEC_PER_SEC	(1000000000)


/* Function for computing time difference between two input timespec structures and saving in third timespec structure.
 * Reference is provided to seqgen.c by Prof. Sam Siewert for delta_t function 
 * */
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


int main(int argc, char** argv)
{
    	struct timespec start_time, stop_time, diff_time;
	
	cvNamedWindow("Detector", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
	
	Mat mat, resz_mat;
	//	mat = imread(argv[1]);
	vector<Rect> found_loc;
	char c;
	int frame_cnt = 0;
	
	VideoCapture cap(argv[1]);
	VideoWriter output_v;
	Size size = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH)/2,
			 (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT)/2);		//Size of capture object, height and width
	output_v.open("output.avi", CV_FOURCC('M','P','4','V'), cap.get(CV_CAP_PROP_FPS), size, true);	//Opens output object
	
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());		//Sets SVM detector to People
	
    	clock_gettime(CLOCK_REALTIME, &start_time);
	while(1)
	{
		cap.read(mat);
		if(mat.empty()) break;
		resize(mat, resz_mat, Size(COLS, ROWS));
		//	resize(mat, resz_mat, Size(800, 533));

		hog.detectMultiScale(resz_mat, found_loc, 0, Size(4, 4), Size(8, 8), 1.05, 2, false);		//Returns vector of rectangles with people within
		for(int i=0; i<found_loc.size(); i++)
		{
			rectangle(resz_mat, found_loc[i], (0, 0, 255), 4);
		}

		imshow("Detector", resz_mat);
		imshow("Video", mat);
		output_v.write(resz_mat);
		frame_cnt++;

		c = cvWaitKey(10);
		if(c == 27) break;
	}

	clock_gettime(CLOCK_REALTIME, &stop_time);
	delta_t(&stop_time, &start_time, &diff_time);
	printf("Number of frames: %d\n", frame_cnt);
	printf("Duration: %ld seconds\n", diff_time.tv_sec);
	printf("Average FPS: %ld\n", (frame_cnt/diff_time.tv_sec));

	return 0;
}
