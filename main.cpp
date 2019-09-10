#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;



int main()
{
	VideoCapture video(0);
	if (!video.isOpened())
	{
		cout << "Webcam Acilamadi";
		system("pause");
		return -1;
	}
	
	//kirmizi
	/*int HMinDeg = 170;
	int HMaxDeg = 179;
	int SMinDeg = 150;
	int SMaxDeg = 255;
	int VMinDeg = 60;
	int VMaxDeg = 255;*/

	//siyah
	int HMinDeg = 0;
	int HMaxDeg = 180;
	int SMinDeg = 0;
	int SMaxDeg = 255;
	int VMinDeg = 0;
	int VMaxDeg = 30;

	int eskiX = -1;
	int eskiY = -1;

	
	Mat arkaPlan;
	video.read(arkaPlan);

	Mat cizgiResim = Mat::zeros(arkaPlan.size(), CV_8UC3);

	while (true)
	{
		Mat yeniFrame;
		bool bSuccess = video.read(yeniFrame);
		if (!bSuccess)
		{
			cout << "Yeni Frame Yuklenemedi" << endl;
			system("pause");
			break;
		}

		Mat gray;
		cvtColor(yeniFrame, gray, COLOR_BGR2GRAY);
		Mat edges;
		Canny(gray, edges, 50, 200);

		vector<Vec4i> lines;
		
		int centerx=0;
		int centery=0;

		Mat HSVres;
		cvtColor(yeniFrame, HSVres, COLOR_BGR2HSV);


		Mat isres;
		inRange(HSVres, Scalar(HMinDeg, SMinDeg, VMinDeg), Scalar(HMaxDeg, SMaxDeg, VMaxDeg), isres);

#pragma region resimde olusan gurultu engelleniyor

		//beyaz gurultulari temizleme
		erode(isres, isres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(isres, isres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//beyaz olan nesnelerin arasindaki kucuk siyah holler temizleniyor
		dilate(isres, isres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(isres, isres, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

#pragma endregion
		Moments konum = moments(isres);
		double yEksen = konum.m01;	
		double xEksen = konum.m10;
		double alan = konum.m00;

		if (alan > 100000)
		{
			int yeniX = xEksen / alan;
			int yeniY = yEksen / alan;
			if (eskiX >= 0 && eskiY >= 0 && yeniX >= 0 && yeniY >= 0)
			{
			
				line(cizgiResim, Point(yeniX, yeniY), Point(eskiX, eskiY), Scalar(0, 0, 255), 2);
			}

			
			eskiX = yeniX;
			eskiY = yeniY;
			cout << eskiX << "-" << eskiY << endl;
		}

		imshow("Siyah Beyaz Resim", isres);

		yeniFrame = yeniFrame + cizgiResim;

		imshow("Orjinal", yeniFrame);

		if (waitKey(100) == 27)
		{
			cout << "Kullanici esc ile cikti" << endl;
			break;
		}
	}
	cvDestroyAllWindows();
	system("pause");
	return 0;
}

