#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include<iostream>
using namespace cv;
using namespace std;
int main(){
    Mat depth = imread("../datapack/1_dpt.tiff",CV::IMREAD_ANYDEPTH);
    Mat img = imread("../datapack/1.jpg");
    FileStrorage reader("../datapack/data.yml",FileStorage::READ);
    Mat c_mat,p_mat;
    Eigen::Matrix<double,3,3> C;
    Eigen::Matrix<double,4,4> P;
    reader["C"] >> c_mat;
    reader["D"] >> p_mat;

    cv2eigen(c_mat, C); //内参矩阵
    cv2eigen(p_mat, P); //透视变换矩阵
    //p106 SlAM
    double fx = C(0,0);
    double fy = C(1,1);
    double cx = C(0,2);
    double cy = C(1,2);
    int pt_col = depth.cols;
    int pt_row = depth.rows;
    Mat result = Mat::zeros(Size(pt_col,pt_row), CV_8UC3);
    for(int u = 0;u < pt_row;u++){
        for(int v = 0;v < pt_col;v++){
            // 世界坐标系
            Vector4d WorldPoint;
            WorldPoint[2] = (double)depth.at<float>(u,v);
            WorldPoint[0] = (u -cx)*WorldPoint[2]/fx;
            WorldPoint[1] = (v - cy)*WorldPoint[2]/fy;
            WorldPoint[3] = 1;
            //透射变换
            Vector4d WorldPoint_t = P*WorldPoint;
            //转换为像素坐标系
            double x = WorldPoint_t[0];
            double y = WorldPoint_t[0];
            double z = WorldPoint_t[0];
            double u_t = fx*x/z + cx;
            double v_t = fy*y/z + cy;
            if(u_t>=0&&v_t>=0&&u_t<pt_col&&v_t<pt_row){
                result.at<uchar>((int)v_t, (int)u_t) = img.at<uchar>(v, u);
            }
        }
    }
    imshow("result",result);
    waitKey(0);
    return 0;
}