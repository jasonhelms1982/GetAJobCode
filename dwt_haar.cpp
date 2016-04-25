/* dwt_Haar.cpp
 *
 *
 *   Author: Jason Helms
 *
 *   This is the code to perform a Discreet Wavelet Transform 4 images, twice. One for red, green, blue and the calculated luminance values. 
 *   There are only 3 methods, one for errors, one to walk the input images and one to do the actaul wavelet calculations. This code
 *   was developed from online forums, Dr. Tiens matlab script and opencv docs. Main takes 3 args, input directory of the r,g,b and l
 *   images, a working directory to keep round 1 of the wavelet transforms and the output (results for binary image transformation input)
 *   directory. This wavelet is a Haar wavelet (Deaubacies with D=1) but instead of averaging the sum of pixels for the LL, other values
 *   could be used (like I have seen root(2)/2 ~ 0.707, not sure why, may test later). 
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
void waveTransAll(string color, string input_pre, string inputMmid, string results);


// argv[1] input-dir: pre-wave
// argv[2] output,input-dir: pre-wave, mid-wave
// argv[3] output-dir : mid-wave (aka results)
// argv[4]
int main(int argc, char** argv)
{
    waveTransAll("blue", argv[1], argv[2], argv[3]);
    waveTransAll("red", argv[1], argv[2], argv[3]);
    waveTransAll("green", argv[1], argv[2], argv[3]);
    waveTransAll("lum", argv[1], argv[2], argv[3]);
    return 0;
}

    /* This is a fancy little method to walk a directory of known inputs and perfrom a wavelet transfrom. There is no
     * need to make sure the images are of any certain size, the code does not care. 
     */

void waveTransAll(string color, string input_pre, string input_mid, string results)
{
    //round 1 - in: ../data/pre-wave-proc/'color', out: ../data/mid-wave-proc/'color'
    for(int i = 0; i < 64; i++)
    {
      String input1 = input_pre + color + "/" + color + "_" + patch::to_string(i) + ".png";//add to for loop and change 0 to i
      String output1 = input_mid + color + "/" + color + "_" + patch::to_string(i) + ".png"; //same as above
      computeOneWavelet(input1, output1, filter);
      remove(input1.c_str());
    }
    //round 2 - in: ../data/mid-wave-proc/'color', out: ../data/post-wave-proc/'color'

    for(int i = 0; i < 64; i++)
    {
      String input2 = input_mid + color + "/" + color + "_" + patch::to_string(i) + ".png";//add to for loop and change 0 to i
      String output2 = results + color + "/" + color + "_" + patch::to_string(i) + ".png"; //same as above
      computeOneWavelet(input2, output2, filter);
      remove(input2.c_str());
    }
    
    
}


    /* This is a method to calculate one wavelet transform. This method will read in the image, perfrom the transfrom
     * and then save the results to a new image for later processing. There are faster ways to do this (like a method
     * that takes a mat and returns a mat, keeping it all in memory, but the images are really small and I use a SSD so
     * I don't care as much as I used to, SSDs are really really really fast). 
     *
     */

void computeOneWavelet(String picture, String dest, float val)
{
    Mat in = imread (picture, 0);

    if(! in.data)
        printError();
    Mat input(in.rows, in.cols, CV_32F);
    in.convertTo(input, CV_32F, 1.0, 0.0);

    Mat L, LL; //LH, LL, HL, HH, L, H;

    float a, b, c, d;

    L = Mat::zeros(input.rows/2, input.cols, CV_32F);
    H = Mat::zeros(input.rows/2, input.cols, CV_32F);

    LL = Mat::zeros(input.rows/2, input.cols/2, CV_32F);
    LH = Mat::zeros(input.rows/2, input.cols/2, CV_32F);

    HH = Mat::zeros(input.rows/2, input.cols/2, CV_32F);
    HL = Mat::zeros(input.rows/2, input.cols/2, CV_32F);

    for(int rcnt=0;rcnt<input.rows;rcnt+=2)
    {
        for(int ccnt=0;ccnt<input.cols;ccnt++)
        {
          a=input.at<float>(rcnt,ccnt);
          b=input.at<float>(rcnt+1,ccnt);
          c=(float)(a+b)*val;
          d=(float)(a-b)*val;
          int _rcnt=rcnt/2;
          L.at<float>(_rcnt,ccnt)=(uchar)c;
          H.at<float>(_rcnt,ccnt)=(uchar)d;
        }//end inner for
    }//end outter for 
    
    for(int rcnt=0;rcnt<input.rows/2;rcnt++)
    {
        for(int ccnt=0;ccnt<input.cols;ccnt+=2)
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

    for(int rcnt=0;rcnt<input.rows/2;rcnt++)
    {
        for(int ccnt=0;ccnt<input.cols;ccnt+=2)
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

    HL.convertTo(HL, CV_8U);
    HH.convertTo(HH, CV_8U);
    LH.convertTo(LH, CV_8U);
    LL.convertTo(LL, CV_8U);

    imwrite(dest + "ll.png", LL);
    imwrite(dest + "lh.png", LH);
    imwrite(dest + "hl.png", HL);
    imwrite(dest + "hh.png", HH);
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
