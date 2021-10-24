#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
using namespace cv;
using namespace std;
void onclick(int event, int x, int y, int flags, void* srcQuad){
    {
        vector< Point2f >* pointSet = (vector< Point2f >*) srcQuad;
        pointSet->push_back(Point2f(x, y));
        cout << "get a point" << endl;
    }
}
void doPerspecticveTransform(Mat input,Mat &output){
    vector<Point2f> src(4),dst(4);
    dst[0].y = 0;    dst[0].x = 0;
    dst[1].y = 60;    dst[1].x = 0;
    dst[2].y = 60;    dst[2].x = 180;
    dst[3].y = 0;    dst[3].x = 180;

    onclick();
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