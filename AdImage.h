#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp> 
#include <opencv2/nonfree/nonfree.hpp> 
#include <opencv2/legacy/legacy.hpp> 
#include <string>


class AdImage
{
public:
	AdImage(const char* file_name, const char* logo);
	~AdImage();
	void rectScale(cv::Rect & rect, double scale);
	double pointDistance(cv::Point2f & a, cv::Point2f & b);
	int get_num_face() { return _num_faces;	}
	int get_num_unionpay() { return _num_unionpay; }
	int is_fullscreen() { return _fullscreen; }

private:
	void detectFace();
	void detectUnionpay();
	void detectFullScreen();
	int _num_faces, _num_unionpay;
	int _scale;
	bool _fullscreen;
	std::string _file_name;
	std::string _cascade_face;
};

