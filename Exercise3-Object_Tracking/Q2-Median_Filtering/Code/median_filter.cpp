/* 
 * Author: Sarthak Jain
 * Dated: June 28, 2019
 * For the use of Embedded Machine Vision under Prof. Sam Siewert, CU Boulder
 *
 * Code for median filter, applied on single stationary bright spot image
 * The code accepts a single image file as input, splits said image into three 
 * channels and applies median filter to Green channel of the image.
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
	namedWindow("Green", CV_WINDOW_AUTOSIZE);

	Mat src, channel[3], dest;		//Source, destination and channel array objects
	src =  imread(argv[1], CV_LOAD_IMAGE_COLOR);

	split(src, channel);			//Split source image into three distinct channels
						//[0] for B, [1] for G, [2] for R
	medianBlur(channel[1], dest, 5);	//Median filter applied to G channel

	imshow("Green", channel[1]);
	imshow("Median-filtered grayscale", dest);

	imwrite("./Before_filter.png", channel[1]);
	imwrite("./After_filter.png", dest);

    	while(1)
	{
        // set pace on shared memory sampling in msecs
        	char c = cvWaitKey(10);

        	if(c == 27)
        	{
           	 exit(1);
		}

    	}
	return 0;
}
