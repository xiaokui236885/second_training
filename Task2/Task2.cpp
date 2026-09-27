#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace cv;
using namespace std;

int main(){
    VideoCapture cap("resources/task_2.mp4");
    if(!cap.isOpened()){
        cout << "无法打开视频！" << endl;
        return -1;
    }

    //视频参数
    double fps = 60.0;
    int width = 960;
    int height = 720;
    int total = 1440;
    double cx =480.0;
    double cy = 360.0;
    double radius = 220.0;

    Mat frame;
    int frame_count = 0;

    // 创建视频写入对象
    VideoWriter writer("result/task2_fit/tracking_overlay.mp4",
                        VideoWriter::fourcc('m', 'p', '4', 'v'),
                        fps, Size(width, height));
    if (!writer.isOpened()) {
        cout << "无法创建输出视频！" << endl;
        return -1;
    }

    vector<double> angles;
    vector<double> frame_ids;
    double prev_angle = 0.0;

    while (cap.read(frame)) {
        // ========== 青色小球识别 ==========
        // 1. 转 HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // 2. 青色掩膜（范围先按这个试）
        Mat mask;
        inRange(hsv, Scalar(85, 100, 100), Scalar(115, 255, 255), mask);

        // 3. 形态学去噪
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(mask, mask, MORPH_OPEN, kernel);
        morphologyEx(mask, mask, MORPH_CLOSE, kernel);

        // 4. 找轮廓
        vector<vector<Point>> contours;
        findContours(mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // 5. 取面积最大的轮廓（青色小球）
        if (!contours.empty()) {
            int max_idx = 0;
            double max_area = 0;
            for (size_t i = 0; i < contours.size(); i++) {
                double area = contourArea(contours[i]);
                if (area > max_area) {
                max_area = area;
                max_idx = (int)i;
                }
            }

            // 6. 用矩算重心
            Moments m = moments(contours[max_idx]);
            if (m.m00 > 0) {
                double x = m.m10 / m.m00;
                double y = m.m01 / m.m00;

                // 7. 画标记
                circle(frame, Point((int)x, (int)y), 5, Scalar(0, 0, 255), -1);      // 小球
                circle(frame, Point((int)cx, (int)cy), 4, Scalar(255, 255, 255), -1); // 旋转中心
                circle(frame, Point((int)cx, (int)cy), (int)radius, Scalar(255, 255, 255), 1); // 轨迹圆
                line(frame, Point((int)cx, (int)cy), Point((int)x, (int)y), Scalar(0, 255, 0), 2);

                // 8. 终端打印坐标
                cout << "Frame " << frame_count << " 中心: (" << x << ", " << y << ")" << endl;
                
                // 9. 计算角度
                double angle = atan2(cy - y, x - cx);
                
                // 10.角度展开
                if (frame_count > 0) {
                    double diff = angle - prev_angle;
                    if (diff > M_PI) {
                        angle -= 2 * M_PI;
                    }
                    if (diff < -M_PI) {
                        angle += 2 * M_PI;
                    }
                    prev_angle = angle;
                }
                // 11.保存
                angles.push_back(angle);
                frame_ids.push_back(frame_count);

                // 12. 在画面上显示角度
                putText(frame, "Angle: " + to_string(angle), Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 255), 2);
                
            }
        }


        // ========== 显示 ==========
        imshow("Mask", mask);
        imshow("Tracking", frame);
        writer.write(frame);
        char key = waitKey(1);
        if(key == 27){
            break;
        }
        frame_count++;
    }
    
    // 保存角度到 CSV
    ofstream out("result/angles.csv");
    out << "frame,angle\n";
    for (size_t i = 0; i < angles.size(); i++) {
        out << frame_ids[i] << "," << angles[i] << "\n";
    }
    out.close();
    writer.release();
    cout << "角度已保存到 result/angles.csv" << endl;







    return 0;
}