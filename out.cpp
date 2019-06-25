#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<math.h>
#include<stdlib.h>

using namespace std;
using namespace cv;

void detect(String imgpath, float &xret, float &yret, float &iw)
{
    Mat imgOriginal;
    Mat ip;
    Mat img;
    Mat imgGrayscale;
    Mat imgEdges;
    int a[10000];
    int c, check;
    
    vector<vector<Point> > contours;
    vector<vector<Point> > selcontours;
    vector<Vec4i> hierarchy;
    
    ip = imread(imgpath);
    if (ip.empty()) {
        cout << "Error: Image not loaded\n";
        return;
    }

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(2, 2));
    morphologyEx(ip, ip, MORPH_CLOSE, kernel, Point(-1,-1), 4);
    
    iw = ip.size().height;
    
    Point2f vtx[4];
    Point2f A;
    Point2f B;
    
    imgOriginal = ip;
    
    Canny(imgOriginal, imgEdges, 120, 240);
    
    findContours(imgEdges, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    
    vector<RotatedRect> minEllipses(contours.size());
    vector<RotatedRect> selEllipses(contours.size());
    
    int ci = 0, cj = 0;
    
    for (ci = 0; ci < contours.size(); ci++) {
        if (contours[ci].size() > 500) {
            minEllipses[ci] = fitEllipse(Mat(contours[ci]));
            if(minEllipses[ci].center.x>=imgOriginal.cols/2-400 && minEllipses[ci].center.x<=imgOriginal.cols/2+400 && minEllipses[ci].center.y>=imgOriginal.rows/2-350 && minEllipses[ci].center.y<=imgOriginal.cols/2+350) {
                selEllipses[cj] = minEllipses[ci];
                selcontours.push_back(contours[ci]);
                cj++;
            }
        }
    }
    
    for(ci = 1; ci < cj; ci++) {
        if(contourArea(selcontours[ci])>contourArea(selcontours[0])) {
            selEllipses[0] = selEllipses[ci];
            selcontours[0] = selcontours[ci];
        }
    }
    
    ellipse(imgOriginal,selEllipses[0],(255,255,255),10);
    xret = selEllipses[0].center.x;
    yret = selEllipses[0].center.y;
    
    img = imgOriginal.clone();
    resize(img, img, Size(img.cols/4, img.rows/4)); //try
    namedWindow("rims"+imgpath, WINDOW_AUTOSIZE);
    imshow("rims"+imgpath, img);
    
    return;
}

int main()
{
    int imgno;
    float xl, xr, yl, yr, iw;
    float X, Y, Z;
    String imgcam1 = "TouchLess2019/U3TL0001/Negative x/U3TL0001_021_1d.jpg";
    String imgcam2 = "TouchLess2019/U3TL0002/Negative x/U3TL0002_021_1d.jpg";
    detect(imgcam1, xl, yl, iw);
    detect(imgcam2, xr, yr, iw);
    float f, b, d;
    f = 0; // get from camera matrix
    b = 0.2;
    d = fabs(xr - xl);
    Z = f * b / d;
    X = xl * Z / f;
    Y = yl * Z / f;
    cout<<"\nX, Y: "<<X<<", "<<Y<<endl;
    cout<<"Z: "<<Z<<endl;
    cout<<"D: "<<sqrt(X * X + Y * Y + Z * Z)<<endl<<endl;
    waitKey(0);
}
