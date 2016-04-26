/* dwt_Haar.cpp
 *
 *
 *   Author: Jason Helms
 *
 *   This is the code to perform a Discreet Wavelet Transform on a grayscale image. There are 
 *   only 2 methods, one for errors and two to do the actaul wavelet calculations. This code
 *   was developed from online forums, Dr. Tiens matlab script and opencv docs. Main takes 2 arguments:
 *   1) input image and 2) output directory for output images, 4 of them
 *   
 * Created by Jason Helms on 12/07/15.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define filter 0.5

using namespace std;
using namespace cv;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
void printError();
void computeOneWavelet(String picture, String dest, float val);


// argv[1] input-dir: pre-wave
// argv[2] output-dir: post-wave
int main(int argc, char** argv)
{
    computetOneWavelet(argv[1], argv[2]);
    return 0;
}


    /* This is a method to calculate one wavelet transform. This method will read in the image, perfrom the transfrom
     * and then save the results to a new image for later processing. There are faster ways to do this (like a method
     * that takes a mat and returns a mat, keeping it all in memory, but the images are really small and I use a SSD so
     * I don't care as much as I used to, SSDs are really really really fast). 
     *
     */

void computeOneWavelet(String picture, String dest)
{
    Mat in = imread (picture, 0);

    if(! in.data)
        printError();

    in.convertTo(in, CV_32F, 1.0, 0.0);

    Mat L, LL, LH, LL, HL, HH, L, H;

    float a, b, c, d;

    L = Mat::zeros(in.rows/2, in.cols, CV_32F);
    H = Mat::zeros(in.rows/2, in.cols, CV_32F);

    LL = Mat::zeros(in.rows/2, in.cols/2, CV_32F);
    LH = Mat::zeros(in.rows/2, in.cols/2, CV_32F);

    HH = Mat::zeros(in.rows/2, in.cols/2, CV_32F);
    HL = Mat::zeros(in.rows/2, in.cols/2, CV_32F);

    for(int rcnt=0;rcnt<in.rows;rcnt+=2)
    {
        for(int ccnt=0;ccnt<in.cols;ccnt++)
        {
          a=in.at<float>(rcnt,ccnt);
          b=in.at<float>(rcnt+1,ccnt);
          c=(float)(a+b)*val;
          d=(float)(a-b)*val;
          int _rcnt=rcnt/2;
          L.at<float>(_rcnt,ccnt)=(uchar)c;
          H.at<float>(_rcnt,ccnt)=(uchar)d;
        }//end inner for
    }//end outter for 
    
    in.deallocate();
    
    for(int rcnt=0;rcnt<in.rows/2;rcnt++)
    {
        for(int ccnt=0;ccnt<in.cols;ccnt+=2)
        {
          a=L.at<float>(rcnt,ccnt);
          b=L.at<float>(rcnt,ccnt+1);
          c=(a+b)*val;
          d=(a-b)*val;
          int _ccnt=ccnt/2;
          LL.at<float>(rcnt,_ccnt)=c;
          LH.at<float>(rcnt,_ccnt)=d;
        }
    }

    for(int rcnt=0;rcnt<in.rows/2;rcnt++)
    {
        for(int ccnt=0;ccnt<in.cols;ccnt+=2)
        {
          a=H.at<float>(rcnt,ccnt);
          b=H.at<float>(rcnt,ccnt+1);
          c=(a+b)*val;
          d=(a-b)*val;
          int _ccnt=ccnt/2;
          HL.at<float>(rcnt,_ccnt)=c;
          HH.at<float>(rcnt,_ccnt)=d;
        }
    }

    L.convertTo(L, CV_8U);
    H.convertTo(H ,CV_8U);
    
    imwrite(dest + "l.png", L);
    imwrite(dest + "h.png", H);
    L.deallocate();
    H.deallocate();

    HL.convertTo(HL, CV_8U);
    HH.convertTo(HH, CV_8U);
    LH.convertTo(LH, CV_8U);
    LL.convertTo(LL, CV_8U);

    imwrite(dest + "ll.png", LL);
    imwrite(dest + "lh.png", LH);
    imwrite(dest + "hl.png", HL);
    imwrite(dest + "hh.png", HH);
    
    LL.deallocate();
    HH.deallocate();
    LH.deallocate();
    HL.deallocate();
}

/* This is a simple method to print an error message when error occurs, prints to command line only. */
void printError()
{
    //error message to command line
    printf("\n\n\t\t\t---------ERROR------------ERROR------------ERROR---------\n");
    printf(    "\t\t\t|\t\t\t\t\t\t\t|\n");
    printf(    "\t\t\t|\t\t\t\t\t\t\t|\n");
    printf(    "\t\t\t|\tError   Error,\t\t\t\t\t|\n");
    printf(    "\t\t\t|\t\t                                      \t|\n");
    printf(    "\t\t\t|\tExiting now... \t\t\t\t\t|\n");
    printf(    "\t\t\t|\t\t\t\t\t\t\t|\n");
    printf(    "\t\t\t|\t\t\t\t\t\t\t|\n");
    printf(    "\t\t\t---------ERROR------------ERROR------------ERROR---------\n\n\n");
    exit(EXIT_FAILURE);
}
