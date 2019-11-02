/* 
 * Author: Sarthak Jain
 * Dated: June 28, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 *
 * Code for motion tracking of a bright object, applied on a video which has a moving bright spot
 * with a light background. The code uses frame differencing between adjacent frames to remove the 
 * background.
 *
 * The code accepts two video files as input, in the format of
 * ./motion_tracking input_video_file_name output_video_file_name
 * 
 * The program relies on the use of VideoCapture and VideoWriter classes.
 * The logic is to take the difference between subsequent frames, store the difference
 * and assign the new frame to the old frame variable, for use in the next iteration,
 * so that no frames are skipped.
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

int threshold_pix(Mat &mat)
{
	int thresh = 0;
	for(int i=0; i<mat.rows; i++)
	{
		for(int j=0; j<mat.cols; j++)
		{
			if(thresh < mat.at<char>(i, j))
				thresh = mat.at<char>(i, j);
			mat.at<char>(i, j) = (mat.at<char>(i, j) > 35) ? 255 : 0;
		}
	}
	return (thresh);
}


int main(int argc, char** argv)
{

//	cvNamedWindow("Difference Frame", CV_WINDOW_AUTOSIZE);

	VideoCapture cap(argv[1]);		//Capture object
	VideoWriter output_v;			//Writer object

	Size size = Size((int) cap.get(CAP_PROP_FRAME_WIDTH),
			 (int) cap.get(CAP_PROP_FRAME_HEIGHT));		//Size of capture object, height and width

	output_v.open(argv[2], CV_FOURCC('M', 'J', 'P', 'G'), cap.get(CAP_PROP_FPS), size, true);	//Opens output object
									//Creating instance with same dimensions as that of input file
	Mat og_mat, new_mat, src, mat, channel[3];
	int i, j, thresh = 0;
	int x_min, x_max, y_min, y_max;
	uint8_t once = 1, brightness = 0;

	Scalar white(0, 255, 0);
	cap>>og_mat;    

	while(1)
	{
        	char c = cvWaitKey(10);
        	if(c == 27)
		{exit(1);}

  		cap>>new_mat;
		if(new_mat.empty()) break;	//Empty frame indicating end of video file, break from loop
		src = new_mat - og_mat;
		og_mat = new_mat.clone();
//		split(src, channel);
		cvtColor(src, mat, COLOR_BGR2GRAY);
		
		thresh = threshold_pix(mat);
		medianBlur(mat, mat, 5);	
		
		for(i=0; i<mat.rows;i++)
		{
			for(j=0; j<mat.cols;j++)
			{
				if(mat.at<char>(i, j) >= thresh)
				{
					thresh = mat.at<char>(i, j);
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
//		thresh = 0;

		if(brightness)
		{
			line(/*src*/new_mat, Point((x_min + x_max)/2, Y_MIN), Point((x_min + x_max)/2, Y_MAX), white, 2, 8, 0);
			line(/*src*/new_mat, Point(X_MIN, (y_min + y_max)/2), Point(X_MAX, (y_min + y_max)/2), white, 2, 8, 0);
			circle(/*src*/new_mat, Point((x_min + x_max)/2, (y_min + y_max)/2), 25, white, 2, 8, 0);
			brightness = 0;
		}
	
		imshow("Tracking Video", /*src*/new_mat);
		output_v << new_mat;   	//Stream each frame into output file
		
	}

	return 0;
}
