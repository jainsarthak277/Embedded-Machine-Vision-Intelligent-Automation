/* 
 * Author: Sarthak Jain
 * Dated: June 28, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 *
 * The program simply breaks down the video provided as input into its individual frames,
 * by first splitting the video into it's channels, selecting the 'G' channel
 * and streaming each frame to an output image. 
 *
 * As a continuation to this code, ffmpeg command for recombining each processed frame into a 
 * video is as:
 * ffmpeg -i Grayscale%d.pgm output_video.mpeg
 *
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
	namedWindow("Grayscale", CV_WINDOW_AUTOSIZE);
	int i=0;
	char output_frame[40];

	Mat src, channel[3];		//Source and channel array objects
	VideoCapture cap(argv[1]);		//Capture object


    	while(1)
	{
        // set pace on shared memory sampling in msecs
        	char c = cvWaitKey(10);

        	if(c == 27)
        	{
           	 exit(1);
		}

		cap>>src;			//Stream next frame
		if(src.empty()) break;	//Empty frame indicating end of video file, break from loop
		split(src, channel);			//Split source image into three distinct channels
							//[0] for B, [1] for G, [2] for R
		imshow("Grayscale", channel[1]);

		sprintf(output_frame, "./Grayscales/Grayscale%d.pgm", i);
		i++;
		imwrite(output_frame, channel[1]);
    	}
	return 0;
}
