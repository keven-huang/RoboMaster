#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
int main(){
     Mat src;
     src = imread(PROJECT_DIR"/apple.jpg");
     Mat hsv_img,hsv1,hsv2,hsv3,hsv4,result;
     cvtColor(src,hsv_img,COLOR_BGR2HSV);

     inRange(hsv_img,Scalar(0,156,53),Scalar(24,255,255),hsv1);
     inRange(hsv_img,Scalar(155,156,53),Scalar(180, 255, 255),hsv2);
     Mat ones_mat = cv::Mat::ones(cv::Size(src.cols, src.rows), CV_8UC1);
     Mat hsv_result = 255 * (ones_mat - (ones_mat - hsv1 / 255).mul(ones_mat - hsv2 / 255));
     Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));
     morphologyEx(hsv_result,hsv_result, MORPH_OPEN, element);  //open
     morphologyEx(hsv_result,hsv_result, MORPH_CLOSE, element); // close
    cv::imshow("result",hsv_result);
     vector<vector<Point>> contour;
     vector<Vec4i> hierarchy;
     cv::findContours(hsv_result,contour,hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
     vector<Rect> Rect(contour.size());
     for(int i=0;i<contour.size();i++){
         Rect[i] = boundingRect(Mat(contour[i]));
        if(Rect[i].width>50&&Rect[i].height>50)
            rectangle(src,Rect[i],Scalar(0, 0, 255),2,8);
     }
     cv::imshow("apple",src);
     waitKey(0);
    return 0;
}