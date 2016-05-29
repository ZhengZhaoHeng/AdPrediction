#include "AdImage.h"

using namespace cv;
using namespace std;

#define EPS 1e-4

Mat mascara;
SIFT sift1(0, 8, 0.1), sift2(0, 8, 0.1);
std::vector<cv::KeyPoint> key_points1, key_points2;
cv::Mat descriptors1, descriptors2;
std::vector<int> matches;
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

AdImage::AdImage(const char * file_name, const char* logo)
{
	_file_name = file_name;
	_cascade_face = "haarcascade_frontalface_alt2.xml";
	_scale = 2;
	_num_faces = 0;
	_num_unionpay = 0;
	_fullscreen = false;
	union_logo = imread(logo);
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
	double dis = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	if (abs(dis) < EPS) dis = 100;
	return dis;
}

void AdImage::detectFace()
{
	int threshold = 30;
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
	int div_col = 5;
	int div_row = 5;
	int threshold = 5;
	Rect ROI(0, 0, 3 * img.cols / div_col, 3 * img.rows / div_row);
	for (int dcol = 0; dcol < div_col - 2; dcol++)
	{
		for (int drow = 0; drow < div_row - 2; drow++)
		{
			ROI.x = img.cols * dcol / div_col;
			ROI.y = img.rows * drow / div_row;
			sift2(img(ROI), mascara, key_points2, descriptors2);
			//imshow("dfdf", img(ROI));
			//cvWaitKey(0);
			//cvDestroyAllWindows();
			//matches.resize(0);
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

				if ((max_1 / max_2) <= 0.6)
				{
					//matches.push_back(DMatch(i, idx_1, 0, sqrt(max_1)));
					matches.push_back(idx_1);
					//circle(img, key_points2[idx_1].pt + Point2f(ROI.x, ROI.y), 2, cvScalar(255, 0, 0), CV_FILLED);
				}
			}

			std::sort(matches.begin(), matches.end());
			auto it = std::unique(matches.begin(), matches.end());
			matches.resize(std::distance(matches.begin(), it));
			for (int it1 = 0; it1 < matches.size(); it1++)
			{
				int count = 0;
				for (int it2 = 0; it2 < matches.size(); it2++)
				{
					if (pointDistance(key_points2[matches[it1]].pt, key_points2[matches[it2]].pt) < 50)
					{
					//	cout << key_points2[matches[it1]].pt << ' ' << key_points2[matches[it2]].pt << endl;
						count++;
					}
					if (count >= threshold) break;
				}

				//cout << count << endl;
				if (count >= threshold)
				{
					_num_unionpay++;
					circle(img, key_points2[matches[it1]].pt + Point2f(ROI.x, ROI.y), 80, cvScalar(0, 255, 0), CV_FILLED);
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
