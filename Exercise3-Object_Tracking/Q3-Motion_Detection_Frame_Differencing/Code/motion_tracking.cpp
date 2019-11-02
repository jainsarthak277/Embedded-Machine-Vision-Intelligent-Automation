/* 
 * Author: Sarthak Jain
 * Dated: June 28, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 *
 * Code for motion tracking of a bright object, applied on a video which has a moving bright spot
 * with a dark background. The code uses frame differencing between adjacent frames to remove the 
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

int main(int argc, char** argv)
{

	cvNamedWindow("Difference Frame", CV_WINDOW_AUTOSIZE);

	VideoCapture cap(argv[1]);		//Capture object
	VideoWriter output_v;			//Writer object

	Size size = Size((int) cap.get(CAP_PROP_FRAME_WIDTH),
			 (int) cap.get(CAP_PROP_FRAME_HEIGHT));		//Size of capture object, height and width

	output_v.open(argv[2], cap.get(CV_CAP_PROP_FOURCC), cap.get(CAP_PROP_FPS), size, true);	//Opens output object
									//Creating instance with same dimensions as that of input file
	Mat og_mat, new_mat, diff_mat;
	cap>>og_mat;				//Streams first frame

    	while(1)
	{
        	char c = cvWaitKey(10);

        	if(c == 27)
		{exit(1);}

		if(og_mat.empty()) break;	//Empty frame indicating end of video file, break from loop
		cap>>new_mat;			//Stream next frame
		diff_mat = new_mat - og_mat;
		og_mat = new_mat.clone();	
		
		imshow("Difference Frame", diff_mat);
		output_v << diff_mat;   	//Stream each frame into output file
	}

	return 0;
}
