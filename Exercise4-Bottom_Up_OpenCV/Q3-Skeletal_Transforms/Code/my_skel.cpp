/* 
 * Author: Sarthak Jain
 * Dated: July 19, 2019
 * Titled: top-down approach to skeletal transforms using opencv
 * for the use of Embedded Machine Vision under Prof. Sam Siewert, CU boulder
 *
 * the code is designed to generate a live video of any objects in front of the camera. 
 * it produces a skeletal transformed set of frames, shown as a video in the code.
 * the program uses a set of opencv apis to generate the skeletal transforms. 
 * reference provided for the time differencing function and the opencv apis.
 * videocapture class is used open the camera stream and write the output video.
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

using namespace cv;
using namespace std;

#define NSEC_PER_SEC	(1000000000)
#define THRESHOLD	(100)
#define MAX_THRESHOLD	(255)

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
	char output_frame[40];
    	struct timespec start_time, stop_time, diff_time;

	cvNamedWindow("Video Stream", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("graymap", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("binary", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("skeleton", CV_WINDOW_AUTOSIZE);

	VideoCapture cap(0);			//Capture object is camera
    	if(!cap.isOpened())  			// check if we succeeded
        	return -1;
	VideoWriter output_v;			//Writer object

	Size size = Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),
			 (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));		//Size of capture object, height and width

	output_v.open("output.avi", CV_FOURCC('M','P','4','V'), cap.get(CV_CAP_PROP_FPS), size, true);	//Opens output object
										//Creating instance with same dimensions as that of input file

	Mat src, gray, binary, mfblur,temp, eroded, RGB_skel;
 	bool done;
 	int iterations=0, p = 0, frame_cnt = 0;
	char c;

    	clock_gettime(CLOCK_REALTIME, &start_time);
	while(1)
	{
		cap.read(src);
	 	imshow("Video Stream", src);		// show original source image and wait for input to next step

	 	cvtColor(src, gray, CV_BGR2GRAY);	// show graymap of source image and wait for input to next step
 		imshow("graymap", gray);
	
 		threshold(gray, binary, THRESHOLD, MAX_THRESHOLD, CV_THRESH_BINARY);	// show bitmap of source image and wait for input to next step

	 	binary = MAX_THRESHOLD - binary;			//Perform image subtraction sinceforeground is generally darker than background
 		imshow("binary", binary);

 		medianBlur(binary, mfblur, 3);		// To remove median filter, just replace blurr value with 1
 		imshow("medianblur", mfblur);

		/*This section of code was adapted from the following post, which was based in turn on the Wikipedia description of a morphological skeleton 
		 * http://felix.abecassis.me/2011/09/opencv-morphological-skeleton */

 		Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
 		Mat skel(mfblur.size(), CV_8UC1, Scalar(0));
		iterations = 0;
	 	do
 		{
   			erode(mfblur, eroded, element);		//Erode boundary edges of object
	   		dilate(eroded, temp, element);		//Extend original object to compensate for loss of data
   			subtract(mfblur, temp, temp);		//Subtract to obtain only increased sections of object
   			bitwise_or(skel, temp, skel);		//Save increased sections of each iteration in skel
	   		eroded.copyTo(mfblur);

   			done = (countNonZero(mfblur) == 0);	//Check for completely black image
   			iterations++;
 
	 	} while (!done && (iterations < 100));

		printf("Iterations: %d\n", iterations);
	 	cvtColor(skel, RGB_skel, CV_GRAY2BGR);		// show graymap of source image and wait for input to next step
	 	imshow("skeleton", RGB_skel);
		
		c = cvWaitKey(33);
		if(c == 27)
		{
    			clock_gettime(CLOCK_REALTIME, &stop_time);
			delta_t(&stop_time, &start_time, &diff_time);		//Obtain time difference
			printf("Duration: %ld seconds\n", (diff_time.tv_sec));
			printf("Average FPS: %ld\n", (frame_cnt/diff_time.tv_sec));
			exit(1);
		}
			
		sprintf(output_frame, "./frames/frame%d.jpg", p);
		p++;

		imwrite(output_frame, RGB_skel);		//save output frames in folder
		output_v.write(RGB_skel);			//write output frames to video
		frame_cnt++;
	}
	return 0;
}
