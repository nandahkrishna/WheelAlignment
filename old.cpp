#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<stdlib.h>

using namespace std;
using namespace cv;
int main() {

	Mat imgOriginal;
	Mat ip;
	Mat img;
	Mat imgGrayscale;
	Mat imgEdges;
	int a[10000];
	int c, check;

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	ip = imread("image3.jpg");
	if (ip.empty()) {
		cout << "Error: Image not loaded...";
		_getch();
		return 0;
	}
	Point2f vtx[4];
	Point2f A;
	Point2f B;

	//resize(ip, imgOriginal, Size(ip.cols * 1/2, ip.rows * 1/2));
	imgOriginal = ip;
	pyrUp(imgOriginal, imgOriginal, Size(imgOriginal.cols * 2, imgOriginal.rows * 2));
	//line(imgOriginal, Point(0, 555), Point(2000, 555), CV_RGB(0, 255, 255), 5);
	//cvtColor(imgOriginal, imgGrayscale, CV_BGR2GRAY);

	Canny(imgOriginal, imgEdges, 10, 20);

	findContours(imgEdges, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	vector<RotatedRect> minEllipses(contours.size());
	c = 0;
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > 500) {
			minEllipses[i] = fitEllipse(Mat(contours[i]));
			a[c] = i;
			c++;
		}
	}
	vector<RotatedRect> Ellipses(c);
	for (int i = 0; i < c - 1; i++) {
		Point2f diff = minEllipses[a[i]].center - minEllipses[a[i + 1]].center;
		//if (abs(minEllipses[a[i+1]].boundingRect().width - minEllipses[a[i+1]].boundingRect().height) < 100 && diff.x < 1.0 && diff.y < 1.0) {
			//
			Ellipses[i] = minEllipses[a[i+1]];
			Ellipses[i].points(vtx);
			cout << "\n" << Ellipses[i].center << i;
			cout << "\nmajor=" << max(norm(Mat(vtx[0]), Mat(vtx[1])), norm(Mat(vtx[1]), Mat(vtx[2]))) << " minor=" << min(norm(Mat(vtx[0]), Mat(vtx[1])), norm(Mat(vtx[1]), Mat(vtx[2])));
			check = i;
			for (int j = 0; j < 4; j++)
			{
				line(imgOriginal, vtx[j], vtx[(j + 1) % 4], Scalar(0, 255, 0), 5, LINE_4);
				cout << "\nPoint " << j << " : " << vtx[j].x << " " << vtx[j].y;
			}
			cout << "\nCheck value is =" << check;
			cout << "\nContour Size: " << contours[a[i+1]].size();
			A.x = (vtx[0].x + vtx[1].x) / 2;
			A.y = (vtx[0].y + vtx[1].y) / 2;
			B.x = (vtx[2].x + vtx[3].x) / 2;
			B.y = (vtx[2].y + vtx[3].y) / 2;
			line(imgOriginal, A, B, CV_RGB(255, 255, 255), 10);
			//double angle1 = atan2(A.y - B.y, A.x - B.x) * 180.0 / CV_PI;
			//cout << "\n Angle is : " << angle1;
			double val = ((B.y - A.y) / (B.x - A.x));
			double angle_major = atan(val) * 180.0 / CV_PI;
			cout << "\n Angle of inclination of Major Axis : " << abs(angle_major);
			double angle_minor = 90.0000 - abs(angle_major);
			cout << "\n Angle of inclination of Minor Axis : " << abs(angle_minor);
		//}

	}
	img = imgOriginal.clone();

	//for (int i = 0; i < c; i++) {
		//Scalar color = Scalar(0, 0, 255);
		//ellipse(img, Ellipses[1], color, 2);
	//}
	pyrDown(imgOriginal, ip, Size(imgOriginal.cols / 2, imgOriginal.rows / 2));
	namedWindow("rims", WINDOW_NORMAL);
	imshow("rims", img);
	waitKey(0);
}
