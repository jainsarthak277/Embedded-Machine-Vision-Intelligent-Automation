/* 
 * Author: Sarthak Jain
 * Dated: July 19, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 * Titled: Bottom-up approach to skeletal transforms using crossing number and redundant-pixel removal
 * for the use of Embedded Machine Vision under Prof. Sam Siewert, CU boulder
 *
 * The code is designed to generate a live video of any objects in front of the camera. 
 * it produces a skeletal transformed set of frames, shown as a video in the code.
 * The program uses an algorithm for determining redundant points to generate the skeletal transforms. 
 * Reference provided to Computer and Machine Vision by E.R. Davies
 * VideoCapture class is used open the camera stream and write the output video.
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

#define NSEC_PER_SEC	(1000000000)
#define THRESHOLD 	(10)
#define MAX_THRESHOLD 	(255)

#define P0(image, i, j) 	(image.at<uchar>(i, j))
#define P1(image, i, j) 	(image.at<uchar>(i, j+1))
#define P2(image, i, j) 	(image.at<uchar>(i-1, j+1))
#define P3(image, i, j) 	(image.at<uchar>(i-1, j))
#define P4(image, i, j) 	(image.at<uchar>(i-1, j-1))
#define P5(image, i, j) 	(image.at<uchar>(i, j-1))
#define P6(image, i, j) 	(image.at<uchar>(i+1, j-1))
#define P7(image, i, j) 	(image.at<uchar>(i+1, j))
#define P8(image, i, j) 	(image.at<uchar>(i+1, j+1))

Mat src, gray, binary, skel, mfblur, RGB_skel;
Mat diff_mat;
Mat new_mat;


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

/*
 * Preliminary conversion function.
 * Performs grayscaling, thresholding to binary image, and median filtering.
 * */
void prelim_conv(void)
{
	cvtColor(diff_mat, gray, CV_BGR2GRAY);	// show graymap of source image and wait for input to next step
	imshow("graymap", gray);
	
	threshold(gray, binary, THRESHOLD, MAX_THRESHOLD, CV_THRESH_BINARY);	// show bitmap of source image and wait for input to next step

//	binary = 255 - binary;			//Comment if using frame differencing
 	imshow("binary", binary);

	medianBlur(binary, mfblur, 5); 		// To remove median filter, just replace blurr value with 1
// 	imshow("medianblur", mfblur);
	
}

/*
 * Function to perform skeletal thinning.
 * Reference is given to Chapter 9 of Computer and Machine Vision, by E.R. Davies
 * for the algorithm used.
 * The algorithm calculates the crossing number, and uses it to determine whether 
 * a particular pixel is redundant or not. If redundant, and if the change is taking 
 * place in one of four cardinal directions, the pixel value is reduced to zero. 
 *
 * Returns Mat object
 * */
Mat do_thinning(void)
{
	int sigma, chi;
 	int iterations=0, inner=0;
	bool done;
	do
	{

		done = true;
		for(int i=0; i<mfblur.rows; i++)		//traversing through all pixels of video
		{
			for(int j=0; j<mfblur.cols; j++)
			{
				if(P0(mfblur, i, j) == 255)	//enter if condition only if pixel value is 1. If already 0, no further action required.
				{
					//Sigma calculation
					sigma =   P1(mfblur, i, j) + P2(mfblur, i, j) + P3(mfblur, i, j) + P4(mfblur, i, j) 
						+ P5(mfblur, i, j) + P6(mfblur, i, j) + P7(mfblur, i, j) + P8(mfblur, i, j);
	
					//Chi calculation
					chi = (int)(P1(mfblur, i, j) != P3(mfblur, i, j)) + (int)(P3(mfblur, i, j) != P5(mfblur, i, j)) 
					    + (int)(P5(mfblur, i, j) != P7(mfblur, i, j)) + (int)(P7(mfblur, i, j) != P1(mfblur, i, j))

						+ 2 * ((int)((P2(mfblur, i, j) > P1(mfblur, i, j)) && (P2(mfblur, i, j) > P3(mfblur, i, j)))
						     + (int)((P4(mfblur, i, j) > P3(mfblur, i, j)) && (P4(mfblur, i, j) > P5(mfblur, i, j)))
						     + (int)((P6(mfblur, i, j) > P5(mfblur, i, j)) && (P6(mfblur, i, j) > P7(mfblur, i, j)))
						     + (int)((P8(mfblur, i, j) > P7(mfblur, i, j)) && (P8(mfblur, i, j) > P1(mfblur, i, j))));
		
					//Check for change in one of four directions	
					if((((P3(mfblur, i, j) == 0) && (P7(mfblur, i, j) == 255)) || 		//North points
					   ((P5(mfblur, i, j) == 0) && (P1(mfblur, i, j) == 255)) ||		//West points
					   ((P7(mfblur, i, j) == 0) && (P3(mfblur, i, j) == 255)) || 		//South points
					   ((P1(mfblur, i, j) == 0) && (P5(mfblur, i, j) == 255))) && 		//East points
					   (chi == 2) && (sigma != 255))
					{
						P0(mfblur, i, j) = 0;			//Pixel value reduced to 0
						done = false;
						inner++;
					}
				}
			}
		}
		iterations++;
	} while (!done && (iterations < 100));
	printf("Number of iterations :%d\n", iterations);
	printf("Number of inner iterations :%d\n", inner);
	return mfblur;
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
	char c;
	int p=0, frame_cnt = 0;

	cap.read(src);
    	clock_gettime(CLOCK_REALTIME, &start_time);
	while(1)
	{
		cap.read(new_mat);
	 	imshow("Video Stream", new_mat);			// show original source image and wait for input to next step

		diff_mat = new_mat - src;
		src = new_mat.clone();
	 	
		prelim_conv();					//preliminary conversions, creating graymap, binary and median-blurred images
		
		Mat skel(mfblur.size(), CV_8UC1, Scalar(0));
		skel = do_thinning();
		cvtColor(skel, RGB_skel, CV_GRAY2BGR);
		imshow("skeleton", RGB_skel);
		
		c = cvWaitKey(10);
		if(c == 27)
		{
    			clock_gettime(CLOCK_REALTIME, &stop_time);
			delta_t(&stop_time, &start_time, &diff_time);
			printf("Duration: %ld seconds\n", (diff_time.tv_sec));
			printf("Average FPS: %ld\n", (frame_cnt/diff_time.tv_sec));
			exit(1);	
		}

		sprintf(output_frame, "./frames/frame%d.jpg", p);
		p++;

		imwrite(output_frame, RGB_skel);
		output_v.write(RGB_skel);
		frame_cnt++;
	}
	return 0;
}
