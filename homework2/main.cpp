#include "CameraApi.h" //相机SDK的API头文件

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <iostream>
using namespace cv;
unsigned char           * g_pRgbBuffer;     //处理后数据缓存区

int main() {

    int iCameraCounts = 1;
    int iStatus = -1;
    tSdkCameraDevInfo tCameraEnumList;
    int hCamera;
    tSdkCameraCapbility tCapability;      //设备描述信息
    tSdkFrameHead sFrameInfo;
    BYTE *pbyBuffer;
    int iDisplayFrames = 10000;
    int channel = 3;

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts);
    printf("state = %d\n", iStatus);

    printf("count = %d\n", iCameraCounts);
    //没有连接设备
    if (iCameraCounts == 0) {
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);

    //初始化失败
    printf("state = %d\n", iStatus);
    if (iStatus != CAMERA_STATUS_SUCCESS) {
        return -1;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera, &tCapability);

    //
    g_pRgbBuffer = (unsigned char *) malloc(
            tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3);
    //g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(hCamera);

    /*其他的相机参数设置
    例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
         CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
         CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
         更多的参数的设置方法，，清参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
    */

    if (tCapability.sIspCapacity.bMonoSensor) {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    } else {
        channel = 3;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
    }


    //循环显示1000帧图像
    std::vector <Mat> Photo;
    std::vector <std::vector<Point2f>> point_pix_pos;
    const int board_w = 11, board_h = 8;
    const int board_n = board_w * board_h;
    Size board_size(11, 8);
    int PhotoNum = 0;
    int successes = 0;
    int cnt = 0;
    Mat gray_img, drawn_img;
    std::vector <Point2f> point_pix_pos_buf;
    int found;
    Size img_size;
    while (true) {
        if (CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS) {
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);
            cv::Mat src(
                    Size(sFrameInfo.iWidth, sFrameInfo.iHeight),
                    sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
                    g_pRgbBuffer
            );
            imshow("Opencv Demo", src);
            int key = waitKey(5);
            if (key == 's') {
                //Photo.push_back(src);
                cv::Mat src0 = src;
                if (!cnt) {
                    img_size.width = src0.cols;
                    img_size.height = src0.rows;
                }
                found = findChessboardCorners(src0, board_size, point_pix_pos_buf);
                if (found && point_pix_pos_buf.size() == board_n) {
                    successes++;
                    cvtColor(src0, gray_img, COLOR_BGR2GRAY);
                    find4QuadCornerSubpix(gray_img, point_pix_pos_buf, Size(5, 5));
                    point_pix_pos.push_back(point_pix_pos_buf);
                    drawn_img = src0.clone();
                    drawChessboardCorners(drawn_img, board_size, point_pix_pos_buf, found);
                    imshow("corners", drawn_img);
                    waitKey(50);
                } else
                    std::cout << "\tbut failed to found all chess board corners in this image" << std::endl;
                point_pix_pos_buf.clear();
                PhotoNum++;
            } else if (key == 'm'&&successes>0) {
                int k = 0, n = 0;
                std::cout << successes << " useful chess boards" << std::endl;
                Size square_size(10, 10);
                std::vector <std::vector<Point3f>> point_grid_pos;
                std::vector <Point3f> point_grid_pos_buf;
                std::vector<int> point_count;
                Mat camera_matrix(3, 3, CV_32FC1, Scalar::all(0));
                Mat dist_coeffs(1, 5, CV_32FC1, Scalar::all(0));
                std::vector <Mat> rvecs;
                std::vector <Mat> tvecs;
                if (successes < 4) {
                    std::cout << successes << "too less effective photo" << std::endl;
                    continue;
                }
                for (int i = 0; i < successes; i++) {
                    for (int j = 0; j < board_h; j++) {
                        for (int k = 0; k < board_w; k++) {
                            Point3f pt;
                            pt.x = k * square_size.width;
                            pt.y = j * square_size.height;
                            pt.z = 0;
                            point_grid_pos_buf.push_back(pt);
                        }
                    }
                    point_grid_pos.push_back(point_grid_pos_buf);
                    point_grid_pos_buf.clear();
                    point_count.push_back(board_h * board_w);
                }
                std::cout << calibrateCamera(point_grid_pos, point_pix_pos, img_size, camera_matrix, dist_coeffs, rvecs,tvecs) << std::endl;
                std::cout << camera_matrix << std::endl << dist_coeffs << std::endl;
            } else if (key == 'c') {
                Photo.clear();
                PhotoNum = 0;
            } else if (key == 'q')
                break;
            //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
            //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
            CameraReleaseImageBuffer(hCamera, pbyBuffer);
        }
    }

    CameraUnInit(hCamera);
    //注意，现反初始化后再free
    free(g_pRgbBuffer);
    return 0;
}

