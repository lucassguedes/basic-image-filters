#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <fstream>
#include <stdio.h>

using namespace cv;

enum gColor {BLUE, GREEN, RED};/*Blue = 0, GREEN = 1, RED = 2*/

enum YIQScale {Q, I, Y}; /*Q = 0, I = 1, Y = 2*/

Vec3f absolute(Vec3f v)
{
    v = Vec3f(abs(v[0]), abs(v[1]), abs(v[2]));

    return v;
}

void treat_rgb_color(Vec3f &color)
{
    const int n_channels = 3;
    for(int i = 0; i < n_channels; i++)
    {
        if(color[i] < 0)
        {
            color[i] = 0;
        }else if(color[i] > 255)
        {
            color[i] = 255;
        }
    }
}

Mat3f get_negative(Mat3f image)
{
    Vec3f color;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3f>(i,j);

            color[RED] = 1.0f - color[RED];
            color[GREEN] = 1.0f - color[GREEN];
            color[BLUE] = 1.0f - color[BLUE];

            image.at<Vec3f>(i,j) = color;
        }
    }

    return image;
}

Mat get_negative(Mat image)
{
    Vec3b color;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3b>(i,j);

            color[RED] = 255 - color[RED];
            color[GREEN] = 255 - color[GREEN];
            color[BLUE] = 255 - color[BLUE];

            image.at<Vec3b>(i,j) = color;
        }
    }

    return image;
}

Mat3f get_negative_y(Mat3f image)
{
    Vec3f color;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            color = image.at<Vec3f>(i,j);

            color[Y] = 1.0f - color[Y];

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


void show_mask(std::vector<std::vector<double> > mask)
{
    int rows = mask.size();
    int cols = mask[0].size();

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            std::cout << mask[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

Mat3f apply_mask(Mat3f image, std::vector<std::vector<double> > mask, int pivotX, int pivotY)
{

    int m = mask.size();
    int n = mask[0].size();


    Mat3f result_image(image.rows, image.cols);

    Vec3f pixelValue;

    for(int pixelY = 0; pixelY < image.rows; pixelY++)
    {
        for(int pixelX = 0; pixelX < image.cols; pixelX++)
        {
            /*Applying mask*/
            pixelValue[RED] = 0;
            pixelValue[GREEN] = 0;
            pixelValue[BLUE] = 0;
            // std::cout << "PixelX = " << pixelX << ", PixelY = " << pixelY << std::endl; 
            // std::cout << "Iniciando i em " << pixelX - pivotX << std::
            for(int i = pixelY-pivotY, mask_row=0; i <= pixelY + (m-pivotY-1); i++, mask_row++)
            {
                if(i < 0 || i > (image.rows - 1))
                {
                    // std::cout << "i = " << i << ", continuando...\n";
                    continue;
                }
                for(int j = pixelX - pivotX, mask_col=0; j <= pixelX + (n-pivotX-1); j++, mask_col++)
                {
                    // std::cout << "i = " << i << ", j = " << j << " mask_row = " << mask_row << ", mask_col = " << mask_col << std::endl;
                    if(j < 0 || j > (image.cols - 1))
                    {
                        // std::cout << "j = " << j << ", continuando...\n";
                        continue;
                    }
                    pixelValue += mask[mask_row][mask_col]*image.at<Vec3f>(i,j);

                }
            }

            result_image.at<Vec3f>(pixelY, pixelX) = pixelValue;
            
        }
    }

    return result_image;
}


Mat3f apply_border_detection_filter(Mat3f image)
{
    Vec3f pixelValue;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            pixelValue = Vec3f(0,0,0);
            if((i + 1) < image.rows)
                pixelValue += absolute(image.at<Vec3f>(i+1,j) - image.at<Vec3f>(i,j));
 
            if(j+1 < image.cols)
                pixelValue += absolute(image.at<Vec3f>(i,j+1) - image.at<Vec3f>(i,j));

            image.at<Vec3f>(i, j) = pixelValue;
        }
    }

    return image;
}

Mat3f apply_sobel_gradient(Mat3f image)
{
    Vec3f pixelValue[2];

    Mat3f result_image = image;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            pixelValue[0] = pixelValue[1] = Vec3f(0,0,0);

            if((i+1) < image.rows)
            {
                for(int k = j - 1; k <= j+1; k++)
                {
                    if(k >= 0 && k  < image.cols)
                    {
                        if(k == j)
                            pixelValue[0] += 2*image.at<Vec3f>(i+1, k);
                        else
                            pixelValue[0] += image.at<Vec3f>(i+1, k);

                    }
                }
            }

            if((i-1) >= 0)
            {
                for(int k = j - 1; k <= j+1; k++)
                {
                    if(k >= 0 && k  < image.cols)
                    {
                        if(k == j)
                            pixelValue[0] -= 2*image.at<Vec3f>(i-1, k);
                        else
                            pixelValue[0] -= image.at<Vec3f>(i-1, k);

                    }
                }
            }

            pixelValue[0] = absolute(pixelValue[0]);

            if((j+1) < image.cols)
            {
                for(int k = i - 1; k <= i+1; k++)
                {
                    if(k >= 0 && k  < image.rows)
                    {
                        
                        pixelValue[1] += image.at<Vec3f>(k, j+1);

                    }
                }
            }


            if((j-1) >= 0)
            {
                for(int k = i - 1; k <= i+1; k++)
                {
                    if(k >= 0 && k  < image.rows)
                    {
                        pixelValue[1] -= image.at<Vec3f>(k, j-1);

                    }
                }
            }

            pixelValue[1] = absolute(pixelValue[1]);

            result_image.at<Vec3f>(i, j) = (pixelValue[0] + pixelValue[1]);
        }
    }

    return result_image;
}


Mat3f get_gray_scale(Mat3f image)
{
    Vec3f value;
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            value[RED] = value[GREEN] = value[BLUE] = 0.299* image.at<Vec3f>(i,j)[RED] + 0.587* image.at<Vec3f>(i,j)[GREEN] + 0.114* image.at<Vec3f>(i,j)[BLUE];

            image.at<Vec3f>(i,j) = value;
        }
    }

    return image;

}



Mat3f apply_sobel_filter(Mat3f image, std::vector<std::vector<double> >horizontal, std::vector<std::vector<double> >vertical, bool histogram_exp=false)
{
    Mat3f imhorizontal, imvertical, result(image.rows, image.cols);

    imhorizontal = apply_mask(image, horizontal, 1, 1);
    imvertical = apply_mask(image, vertical, 1, 1);

    double redmin, redmax;
    double greenmin, greenmax;
    double bluemin, bluemax;

    redmin = greenmin = bluemin = std::numeric_limits<double>::max();
    redmax = greenmax = bluemax = -std::numeric_limits<double>::max();

    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            result.at<Vec3f>(i,j) = absolute(imhorizontal.at<Vec3f>(i,j)) + absolute(imvertical.at<Vec3f>(i,j));

            treat_rgb_color(result.at<Vec3f>(i,j));

            if(result.at<Vec3f>(i,j)[RED] < redmin)
                redmin = result.at<Vec3f>(i,j)[RED];

            if(result.at<Vec3f>(i,j)[GREEN] < greenmin)
                greenmin = result.at<Vec3f>(i,j)[GREEN];

            if(result.at<Vec3f>(i,j)[BLUE] < bluemin)
                bluemin = result.at<Vec3f>(i,j)[BLUE];


            if(result.at<Vec3f>(i,j)[RED] > redmax)
                redmax = result.at<Vec3f>(i,j)[RED];

            if(result.at<Vec3f>(i,j)[GREEN] > greenmax)
                greenmax = result.at<Vec3f>(i,j)[GREEN];

            if(result.at<Vec3f>(i,j)[BLUE] > bluemax)
                bluemax = result.at<Vec3f>(i,j)[BLUE];

        }
    }


    if(histogram_exp)
    {
        for(int i = 0; i < image.rows; i++)
        {
            for(int j = 0; j < image.cols; j++)
            {
                result.at<Vec3f>(i,j)[RED] = ((result.at<Vec3f>(i,j)[RED] - redmin)/(redmax - redmin))*1.0f;
                result.at<Vec3f>(i,j)[GREEN] = ((result.at<Vec3f>(i,j)[GREEN] - greenmin)/(greenmax - greenmin))*1.0f;
                result.at<Vec3f>(i,j)[BLUE] = ((result.at<Vec3f>(i,j)[BLUE] - bluemin)/(bluemax - bluemin))*1.0f;
            }
        }
    }

    return result;
}


Mat3f get_median(Mat3f image, int m, int n)
{
    Mat3f result_image(image.rows, image.cols);

    Vec3f pixelValue;

    int pivotX=0,pivotY=0;
    std::vector<double> reds, greens, blues;

    for(int pixelY = 0; pixelY < image.rows; pixelY++)
    {
        for(int pixelX = 0; pixelX < image.cols; pixelX++)
        {
            /*Applying mask*/
            pixelValue = Vec3f(0,0,0);

            reds = greens = blues = {};
            // std::cout << "PixelX = " << pixelX << ", PixelY = " << pixelY << std::endl; 
            // std::cout << "Iniciando i em " << pixelX - pivotX << std::
            for(int i = pixelY-pivotY, mask_row=0; i <= pixelY + (m-pivotY-1); i++, mask_row++)
            {
                if(i < 0 || i > (image.rows - 1))
                {
                    // std::cout << "i = " << i << ", continuando...\n";
                    reds.push_back(0);
                    greens.push_back(0);
                    blues.push_back(0);
                    continue;
                }
                for(int j = pixelX - pivotX, mask_col=0; j <= pixelX + (n-pivotX-1); j++, mask_col++)
                {
                    // std::cout << "i = " << i << ", j = " << j << " mask_row = " << mask_row << ", mask_col = " << mask_col << std::endl;
                    if(j < 0 || j > (image.cols - 1))
                    {
                        // std::cout << "j = " << j << ", continuando...\n";
                        reds.push_back(0);
                        greens.push_back(0);
                        blues.push_back(0);
                        continue;
                    }
                    reds.push_back(image.at<Vec3f>(i,j)[RED]);
                    greens.push_back(image.at<Vec3f>(i,j)[GREEN]);
                    blues.push_back(image.at<Vec3f>(i,j)[BLUE]);

                }
            }

            std::sort(reds.begin(), reds.end());
            std::sort(greens.begin(), greens.end());
            std::sort(blues.begin(), blues.end());

            pixelValue[RED] = reds[(reds.size()-1)/2];
            pixelValue[GREEN] = greens[(greens.size()-1)/2];
            pixelValue[BLUE] = blues[(blues.size()-1)/2];

            result_image.at<Vec3f>(pixelY, pixelX) = pixelValue;
            
        }
    }

    return result_image;
}




int main(int argc, char ** argv)
{

    std::ifstream filter;

    if(argc < 2)
    {
        std::cout << "Modo de utilização: BasicFilters.out <Image_Path> <Filter_Path(optional)>\n";
        return -1;
    }
    
    Mat3f image;
    imread(argv[1], 1).convertTo(image, CV_32FC3, 1.0f/255.0f);

    if(!image.data){
        std::cout << "No image data\n";
        return -1;
    }


    if(argc == 3)
    {
        filter.open(argv[2]);
    }


    int n_masks;
    std::vector<int>m,n; /*mask dimension*/
    std::vector<int> pivotX, pivotY;
    std::vector<std::vector<std::vector<double> >> masks;

    filter >> n_masks;

    m = n = pivotX = pivotY = std::vector<int>(n_masks, 0);

    std::cout << "n_masks: " << n_masks << std::endl;
    for(int i = 0; i < n_masks; i++)
    {
        filter >> m[i] >> n[i];
        filter >> pivotX[i] >> pivotY[i];


        masks.push_back(std::vector<std::vector<double> > (m[i], std::vector<double> (n[i], 0)));

        for(int j = 0; j < m[i]; j++)
        {
            for(int k = 0; k < n[i]; k++)
            {
                filter >> masks[i][j][k];
            }
        }

        std::cout << "Mask " << i + 1 << ": \n";

        show_mask(masks[i]);
    }

  

    






    // image = from_rgb_to_yiq(image);

    // image = get_negative_y(image);

    // image = from_yiq_to_rgb(image);



    // image = apply_mask(image, mask, pivotX, pivotY);

    // image = apply_border_detection_filter(image);

    Mat3f colorImage = image;

    Vec3f A = Vec3f(1,2,3);

    A = 2*A;

    std::cout << "A: " << A[0] << ", " << A[1] << ", " << A[2] << std::endl;

    image = get_gray_scale(image);

    image = apply_sobel_filter(image, masks[0], masks[1],true);


 


    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);

    Mat imagem255;

    image.convertTo(imagem255, CV_8UC3, 255);

    imwrite("media7x7.jpeg", imagem255);
    waitKey(0);
    
    return 0;
}