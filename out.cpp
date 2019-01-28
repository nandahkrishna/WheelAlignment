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
    
    iw = ip.size().height;
    
    Point2f vtx[4];
    Point2f A;
    Point2f B;
    
    imgOriginal = ip;
    pyrUp(imgOriginal, imgOriginal, Size(imgOriginal.cols * 2, imgOriginal.rows * 2));
    
    Canny(imgOriginal, imgEdges, 10, 20);
    
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
    //cout<<"Center x, y: "<<selEllipses[0].center.x<<", "<<selEllipses[0].center.y<<"\n"<<"";
    xret = selEllipses[0].center.x;
    yret = selEllipses[0].center.y;
    double k = selEllipses[0].size.height/selEllipses[0].size.width;
    double w = selEllipses[0].angle*3.14/180;
    double al, be;
    //al = sqrt(-((cos(w)*cos(w)*(k*k-1))/(pow(sin(w),2)*(1-k*k)-1)));
    //be = sqrt(-(sin(w)*sin(w)*(1-k*k)));
    //cout<<"a, b: "<<asin(al)*180/3.14<<", "<<asin(be)*180/3.14<<"\n";
    
    img = imgOriginal.clone();
    pyrDown(imgOriginal, ip, Size(imgOriginal.cols / 2, imgOriginal.rows / 2)); //2
    resize(img, img, Size(img.cols/4, img.rows/4)); //try
    namedWindow("rims"+imgpath, CV_WINDOW_AUTOSIZE);
    imshow("rims"+imgpath, img);
    
    return;
}

int main()
{
    int imgno;
    float xl, xr, yl, yr, iw;
    float X, Y, Z;
    cout<<"Image number: ";
    cin>>imgno;
    String imgcam1 = "Images/Cam1/CAM1_"+to_string(imgno)+".jpg";
    String imgcam2 = "Images/Cam2/CAM1_"+to_string(imgno)+".jpg";
    //cout<<"\nCam 1:\n";
    detect(imgcam1, xl, yl, iw);
    //cout<<"\nCam 2:\n";
    detect(imgcam2, xr, yr, iw);
    float f, b, d;
    // Given vertical viewing angle 28 degrees = 0.4887 rad
    f = iw/(2*tan(0.4887/2));
    // Assuming baseline width of 5 cm = 0.05 m
    b = 0.05;
    d = fabs(xr-xl);
    Z = f*b/d;
    X = xl*Z/f;
    Y = yl*Z/f;
    cout<<"\nX, Y: "<<X<<", "<<Y<<endl;
    cout<<"Z: "<<Z<<endl;
    cout<<"D: "<<sqrt(X*X+Y*Y+Z*Z)<<endl<<endl;
    waitKey(0);
}
