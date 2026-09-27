#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main(){
Mat img = imread("flower.jpg");
if (img.empty())
{
    std::cout<<"Cannot read image!"<<std::endl;
    return -1;
}

//转灰度图
Mat gray;
cvtColor(img, gray, COLOR_BGR2GRAY);
imshow("Gray Image", gray);
waitKey(0);

//转HSV图
Mat hsv;
cvtColor(img, hsv, COLOR_BGR2HSV);

//拆分通道
std::vector<Mat> channels;
split(hsv,channels);    //channels[0]为H通道，channels[1]为S通道，channels[2]为V通道
imshow("H",channels[0]);
waitKey(0);
imshow("S",channels[1]);
waitKey(0);
imshow("V",channels[2]);
waitKey(0);

//均值滤波、高斯滤波、中值滤波
Mat meanImg, gaussianImg, medianImg;
blur(img,meanImg,Size(5,5));
GaussianBlur(img,gaussianImg,Size(5,5),1.5);
medianBlur(img,medianImg,5);
imshow("mean",meanImg);
waitKey(0);
imshow("gaussian",gaussianImg);
waitKey(0);
imshow("median",medianImg);
waitKey(0);

//HSV双区间红色掩膜
Mat hsv_mask;
cvtColor(img, hsv_mask, COLOR_BGR2HSV);

//区间1:色相0-10（偏橘红）
Mat mask1;
inRange(hsv_mask, Scalar(0, 80, 80), Scalar(10, 255, 255), mask1);

//区间2:色相170-180（偏紫红）
Mat mask2;
inRange(hsv_mask, Scalar(170, 80, 80), Scalar(180, 255, 255), mask2);

//合并两个区间
Mat red_mask;
bitwise_or(mask1, mask2, red_mask);

imshow("Red Mask", red_mask);
waitKey(0);

// ---------- 形态学操作 ----------
Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

Mat eroded, dilated, opened, closed;
erode(red_mask, eroded, kernel);          // 腐蚀：白色区域缩小
dilate(red_mask, dilated, kernel);        // 膨胀：白色区域扩大
morphologyEx(red_mask, opened, MORPH_OPEN, kernel);    // 开运算：先腐蚀后膨胀（去小白点）
morphologyEx(red_mask, closed, MORPH_CLOSE, kernel);   // 闭运算：先膨胀后腐蚀（填小黑洞）

imshow("Erode", eroded);
waitKey(0);
imshow("Dilate", dilated);
waitKey(0);
imshow("Open", opened);
waitKey(0);
imshow("Close", closed);
waitKey(0);

//选开运算结果提取轮廓
std::vector<std::vector<Point>> contours;
findContours(opened, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

//在原图上绘制
Mat result = img.clone();
for (size_t i = 0; i < contours.size(); i++) {
    double area = contourArea(contours[i]);
    //按面积筛选：过滤掉小于 500 像素的噪点
    if (area < 500) continue;

    //画轮廓（绿色，线宽 2）
    drawContours(result, contours, (int)i, Scalar(0, 255, 0), 2);

    //画外接矩形（蓝色，线宽 2）
    Rect rect = boundingRect(contours[i]);
    rectangle(result, rect, Scalar(255, 0, 0), 2);

    //打印面积
    std::cout << "轮廓 " << i << " 面积: " << area << std::endl;
}

imshow("Result", result);
waitKey(0);

//在原图副本上绘制圆、矩形和文字
Mat drawImg = img.clone();

//画圆（红色，圆心在左上角区域，半径 100）
circle(drawImg, Point(200, 200), 100, Scalar(0, 0, 255), 3);

//画矩形（绿色，左上角坐标 (400, 150)，宽 300 高 400）
rectangle(drawImg, Point(400, 150), Point(700, 550), Scalar(0, 255, 0), 3);

//写文字（蓝色，位置 (150, 600)，字号 1.5，线宽 3）
putText(drawImg, "flower", Point(150, 600),
        FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 0, 0), 3);

imshow("Draw", drawImg);
waitKey(0);

//绕图像中心旋转35°
Point2f center(img.cols / 2.0, img.rows / 2.0);   // 图像中心
Mat rotMat = getRotationMatrix2D(center, 35, 1.0); // 35 度，缩放 1.0
Mat rotated;
warpAffine(img, rotated, rotMat, img.size());      // 输出尺寸与原图一致

imshow("Rotated 35", rotated);
waitKey(0);

//裁剪原图左上角1/4
int w = img.cols / 2;
int h = img.rows / 2;
Mat cropped = img(Rect(0, 0, w, h));   // 从 (0,0) 开始，宽 w 高 h

imshow("Cropped 1/4", cropped);
waitKey(0);





return 0;
}