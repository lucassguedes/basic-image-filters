#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;

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

    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            Vec3b cor = image.at<Vec3b>(i, j);

            cor[0] = 255 - cor[0];
            cor[1] = 255 - cor[1];
            cor[2] = 255 - cor[2];

            image.at<Vec3b>(i, j) = cor;
        }
    }

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    imwrite("finlandia-invertida.jpg", image);
    waitKey(0);
    
    return 0;
}