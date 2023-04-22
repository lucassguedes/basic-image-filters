#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <stdio.h>

using namespace cv;

enum gColor {BLUE, GREEN, RED};/*Blue = 0, GREEN = 1, RED = 2*/

enum YIQScale {Q, I, Y}; /*Y = 0, I = 1, Q = 2*/

Mat get_negative(Mat image)
{
    Vec3f color;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3f>(i,j);

            color[0] = 255 - color[0];
            color[1] = 255 - color[1];
            color[2] = 255 - color[2];

            image.at<Vec3f>(i,j) = color;
        }
    }

    return image;
}

Mat get_single_channel(Mat image, gColor color_id)
{
    Vec3f color;

    const int n_channels = 3;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3f>(i,j);

            for(int k = 0; k < n_channels; k++)
            {
                if(k != color_id)
                    color[k] = 0;
            }

            image.at<Vec3f>(i,j) = color;
        }
    }

    return image;
}

Vec3f from_rgb_to_yiq(Vec3f colorRGB)
{
    Vec3f colorYIQ;

    colorYIQ[Y] = 0.299*colorRGB[RED] + 0.587*colorRGB[GREEN] + 0.114*colorRGB[BLUE];
    colorYIQ[I] = 0.596*colorRGB[RED] - 0.274*colorRGB[GREEN] - 0.322*colorRGB[BLUE];
    colorYIQ[Q] = 0.211*colorRGB[RED] - 0.523*colorRGB[GREEN] + 0.312*colorRGB[BLUE];

    return colorYIQ;

}

Vec3f from_yiq_to_rgb(Vec3f colorYIQ)
{
    Vec3f colorRGB;

    colorRGB[RED] = 1.0 * colorYIQ[Y] + 0.956*colorYIQ[I] + 0.621*colorYIQ[Q];
    colorRGB[GREEN] = 1.0 * colorYIQ[Y] - 0.272*colorYIQ[I] - 0.647*colorYIQ[Q];
    colorRGB[BLUE] = 1.0 * colorYIQ[Y] - 1.106*colorYIQ[I] + 1.703*colorYIQ[Q];

    return colorRGB;
    
}

void treat_rgb_color(Vec3f &color)
{
    const int n_channels = 3;
    for(int i = 0; i < n_channels; i++)
    {
        if(color[i] < 0)
        {
            // std::cout << "color[" << i << "]=" << (int)color[i] << std::endl;
            // getchar();
            color[i] = 0;
        }else if(color[i] > 255)
        {
            // std::cout << "color[" << i << "]=" << (int)color[i] << std::endl;
            // getchar();
            color[i] = 255;
        }
    }
}

Mat from_yiq_to_rgb(Mat image)
{
    Vec3f colorYIQ, colorRGB;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            colorYIQ = image.at<Vec3f>(i,j);
             
            colorRGB = from_yiq_to_rgb(colorYIQ);
            
            treat_rgb_color(colorRGB);            

            
            image.at<Vec3f>(i,j) = colorRGB;
        }
    }

    return image;
}

Mat from_rgb_to_yiq(Mat image)
{
    Vec3f colorYIQ, colorRGB;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            colorRGB = image.at<Vec3f>(i,j);

            colorYIQ = from_rgb_to_yiq(colorRGB);

            image.at<Vec3f>(i,j) = colorYIQ;
        }
    }

    return image;
}


int main(int argc, char ** argv)
{

    if(argc != 2)
    {
        std::cout << "Modo de utilização: BasicFilters.out <Image_Path>\n";
        return -1;
    }
    
    Mat3f image;
    imread(argv[1], 1).convertTo(image, CV_32FC3, 1.0f/255.0f);

    if(!image.data){
        std::cout << "No image data\n";
        return -1;
    }

    // Vec3f colorA;

    // colorA[RED] = 0;
    // colorA[GREEN] = 100;
    // colorA[BLUE] = 0;

    // image.at<Vec3f>(0,0) = colorA;

    // std::cout << "image.at<Vec3f>(0,0):  (" << (int)image.at<Vec3f>(0,0)[RED] << ", " << (int)image.at<Vec3f>(0,0)[GREEN] << ", " << (int)image.at<Vec3f>(0,0)[BLUE] << ")\n";

    // std::cout << "RGB to YIQ...\n";
    // colorA = from_rgb_to_yiq(colorA);


    // image.at<Vec3f>(0,0) = colorA;


    // std::cout << "image.at<Vec3f>(0,0):  (" << (int)image.at<Vec3f>(0,0)[RED] << ", " << (int)image.at<Vec3f>(0,0)[GREEN] << ", " << (int)image.at<Vec3f>(0,0)[BLUE] << ")\n";

    // std::cout << "YIQ to RGB...\n";
    // colorA = from_yiq_to_rgb(colorA);

    // image.at<Vec3f>(0,0) = colorA;

    // std::cout << "image.at<Vec3f>(0,0):  (" << (int)image.at<Vec3f>(0,0)[RED] << ", " << (int)image.at<Vec3f>(0,0)[GREEN] << ", " << (int)image.at<Vec3f>(0,0)[BLUE] << ")\n";

    image = from_rgb_to_yiq(image);
    image = from_yiq_to_rgb(image);

    

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    imwrite("finlandia-invertida.jpg", image);
    waitKey(0);
    
    return 0;
}