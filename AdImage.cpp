#include "AdImage.h"

using namespace cv;
using namespace std;

Mat mascara;
SIFT sift1, sift2;
std::vector<cv::KeyPoint> key_points1, key_points2;
cv::Mat descriptors1, descriptors2;
std::vector<cv::DMatch> matches;
cv::Mat union_logo;
bool* grouped;

double SIFTDistance(float* a, float* b)
{
	double dis = 0;
	for (int i = 0; i < 128; i++)
	{
		double temp = a[i] - b[i];
		dis += temp * temp;
	}
	return dis;
}

AdImage::AdImage(const char * file_name)
{
	_file_name = file_name;
	_cascade_face = "haarcascade_frontalface_alt.xml";
	_scale = 2;
	_num_faces = 0;
	_num_unionpay = 0;
	_fullscreen = false;
	union_logo = imread("unionPay_logo.jpg");
	sift1(union_logo, mascara, key_points1, descriptors1);
	grouped = new bool[key_points1.size()];
	//detectFace();
	detectUnionpay();
}

AdImage::~AdImage()
{
}

void AdImage::rectScale(cv::Rect& rect, double scale)
{
	rect.x *= scale;
	rect.y *= scale;
	rect.width *= scale;
	rect.height *= scale;
}

double AdImage::pointDistance(cv::Point2f& a, cv::Point2f& b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void AdImage::detectFace()
{
	CascadeClassifier faceClassifier;
	vector<Rect> faces;
	Mat img = imread(_file_name);

	if (!faceClassifier.load(_cascade_face))
	{
		printf("Error: could not found face classifier\n");
	}

	Mat gray, smallImg(img.rows / _scale, img.cols / _scale, CV_8UC1);
	cvtColor(img, gray, CV_BGR2GRAY);
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);

	faceClassifier.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE);
	for (auto& face : faces)
	{
		rectScale(face, _scale);
		rectangle(img, face, cvScalar(255, 0, 0));
	}

	_num_faces = faces.size();
	string temp = "out_" + _file_name;
	imwrite(temp.c_str(), img);
}

void AdImage::detectUnionpay()
{
	Mat img = imread(_file_name);
	int div_col = 3;
	int div_row = 3;
	Rect ROI(0, 0, img.cols / div_col, img.rows / div_row);
	for (int dcol = 0; dcol < div_col; dcol++)
	{
		for (int drow = 0; drow < div_row; drow++)
		{
			ROI.x = img.cols * dcol / div_col;
			ROI.y = img.rows * drow / div_row;
			sift2(img(ROI), mascara, key_points2, descriptors2);
			matches.resize(0);
			for (int i = 0; i < key_points1.size(); i++)
			{
				grouped[i] = false;
				float* a = descriptors1.ptr<float>(i);
				double max_2 = -1;
				int idx_2, idx_1;
				idx_2 = idx_1 = 0;
				double max_1 = -1;
				for (int j = 0; j < key_points2.size(); j++)
				{
					float* b = descriptors2.ptr<float>(j);

					double dis = SIFTDistance(a, b);
					if (dis < max_1 || max_1 == -1)
					{
						max_2 = max_1;
						idx_2 = idx_1;
						max_1 = dis;
						idx_1 = j;
					}
					else if (dis < max_2 || max_2 == -1)
					{
						max_2 = dis;
						idx_2 = j;
					}
				}

				if ((max_1 / max_2) <= 0.5)
				{
					matches.push_back(DMatch(i, idx_1, 0, sqrt(max_1)));
					circle(img, key_points2[idx_1].pt + Point2f(ROI.x, ROI.y), 2, cvScalar(255, 0, 0), CV_FILLED);
				}
			}

			for (int it1 = 0; it1 < matches.size(); it1++)
			{
				int count = 0;
				for (int it2 = 0; it2 < matches.size(); it2++)
				{
					if (pointDistance(key_points2[matches[it1].trainIdx].pt, key_points2[matches[it2].trainIdx].pt) < 30)
					{
						count++;
					}
				}

				//cout << count << endl;
				if (count >= 10)
				{
					_num_unionpay++;
					circle(img, key_points2[matches[it1].trainIdx].pt + Point2f(ROI.x, ROI.y), 30, cvScalar(0, 0, 255));
					break;
				}
			}
		}
	}
	
	//cout << _num_unionpay << endl;
	string temp = "out_" + _file_name;
	imwrite(temp, img);
}

void AdImage::detectFullScreen()
{
}
