#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
using namespace cv;
using namespace std;
void doPerspecticveTransform(Mat input,Mat &output){
    vector<Point2f> src(4),dst(4);
    src[0].x =594;     src[0].y = 329;
    src[1].x =606;     src[1].y = 390;
    src[2].x = 773;     src[2].y = 392;
    src[3].x = 763;     src[3].y =  338;
    dst[0].y = 0;    dst[0].x = 0;
    dst[1].y = 100;    dst[1].x = 0;
    dst[2].y = 100;    dst[2].x = 300;
    dst[3].y = 0;    dst[3].x = 300;
    Mat warp_matrix = getPerspectiveTransform(src,dst);
    warpPerspective(input,output,warp_matrix,Size(500,500));

}
int main(){
    Mat img = imread(PROJECT_DIR"/car.jpg");
    Mat result;
    doPerspecticveTransform(img,result);
    imshow("input",img);
    imshow("output",result);
    waitKey(0);
    return 0;
}