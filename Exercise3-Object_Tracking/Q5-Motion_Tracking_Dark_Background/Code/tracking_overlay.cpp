/* 
 * Author: Sarthak Jain
 * Dated: June 28, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 *
 * Code for motion tracking of a bright object, applied on a video which has a moving bright spot
 * with a dark background. The code calculates pixel intensities, and for above a certain threshold, 
 * counts the pixels with intensities above that value, and creates a bounding box
 * of sorts about that bright region. The COM of the region is tracked by the use of two lines and a circle. 
 *
 * The code accepts two video files as input, in the format of
 * ./motion_tracking input_video_file_name output_video_file_name
 * 
 * The program relies on the use of VideoCapture and VideoWriter classes.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#define Y_MIN	(0)
#define Y_MAX	(1080)
#define X_MIN	(0)
#define X_MAX	(1920)

int main(int argc, char** argv)
{

//	cvNamedWindow("Difference Frame", CV_WINDOW_AUTOSIZE);

	VideoCapture cap(argv[1]);		//Capture object
	VideoWriter output_v;			//Writer object

	Size size = Size((int) cap.get(CAP_PROP_FRAME_WIDTH),
			 (int) cap.get(CAP_PROP_FRAME_HEIGHT));		//Size of capture object, height and width

	output_v.open(argv[2], cap.get(CV_CAP_PROP_FOURCC)/*CV_FOURCC('M', 'J', 'P', 'G')*/, cap.get(CAP_PROP_FPS), size, true);	//Opens output object
									//Creating instance with same dimensions as that of input file
	Mat src, mat, channel[3];
	int i, j;
	int x_min, x_max, y_min, y_max;
	uint8_t once = 1, brightness = 0;

	Scalar white(255, 255, 255);

	while(1)
	{
        	char c = cvWaitKey(10);
        	if(c == 27)
		{exit(1);}

		cap>>src;			//Stream next frame
		if(src.empty()) break;	//Empty frame indicating end of video file, break from loop
		split(src, channel);
		medianBlur(channel[1], mat, 5);	
		
		for(i=0; i<mat.rows;i++)
		{
			for(j=0; j<mat.cols;j++)
			{
				if(mat.at<char>(i, j) > 200)
				{
					brightness = 1;
					if(once)
					{
						x_min = j;
						x_max = j;
						y_min = i;
						y_max = i;
						once = 0;
					}
					else
					{
						if(x_min > j)
							x_min = j;
						if(x_max < j)
							x_max = j;	
						if(y_min > i)
							y_min = i;
						if(y_max < i)
							y_max = i;	
					}
				}
			}
		}	
		once = 1;

		if(brightness)
		{
			line(mat, Point((x_min + x_max)/2, Y_MIN), Point((x_min + x_max)/2, Y_MAX), white, 2, 8, 0);
			line(mat, Point(X_MIN, (y_min + y_max)/2), Point(X_MAX, (y_min + y_max)/2), white, 2, 8, 0);
			circle(mat, Point((x_min + x_max)/2, (y_min + y_max)/2), 25, white, 2, 8, 0);
			brightness = 0;
		}
	
		imshow("Tracking Video", mat);
		output_v << mat;   	//Stream each frame into output file
		
	}

	return 0;
}
