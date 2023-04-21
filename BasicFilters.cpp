#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;

enum gColor {BLUE, GREEN, RED};/*Blue = 0, GREEN = 1, RED = 2*/

Mat get_negative(Mat image)
{
    Vec3b color;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3b>(i,j);

            color[0] = 255 - color[0];
            color[1] = 255 - color[1];
            color[2] = 255 - color[2];

            image.at<Vec3b>(i,j) = color;
        }
    }

    return image;
}

Mat get_single_channel(Mat image, gColor color_id)
{
    Vec3b color;

    const int n_channels = 2;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3b>(i,j);

            for(int k = 0; k <= n_channels; k++)
            {
                if(k != color_id)
                    color[k] = 0;
            }

            image.at<Vec3b>(i,j) = color;
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
    
    Mat image;
    image = imread(argv[1], 1);

    if(!image.data){
        std::cout << "No image data\n";
        return -1;
    }

    image = get_single_channel(image, BLUE);



    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    imwrite("finlandia-invertida.jpg", image);
    waitKey(0);
    
    return 0;
}