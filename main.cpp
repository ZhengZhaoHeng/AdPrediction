#include "AdImage.h"
#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	system("rd /s/q out_images_dip");
	system("mkdir out_images_dip");
	WIN32_FIND_DATA search_data;
	memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

	HANDLE handle = FindFirstFile("images_dip\\*.jpg", &search_data);

	std::vector<string> files;
	ofstream fout("unionPay_num.txt");

	while (handle != INVALID_HANDLE_VALUE)
	{
		string file_name = search_data.cFileName;
		file_name = "images_dip\\" + file_name;
		AdImage ad(file_name.c_str());
		fout << search_data.cFileName << ' ' << ad.get_num_unionpay() << endl;
		//if (ad.get_num_unionpay() > 0)
		//{
		//	//cout << search_data.cFileName << ' ' << ad.get_num_unionpay() << endl;
		//	//system("pause");
		//}
		if (FindNextFile(handle, &search_data) == FALSE)
		break;
	}

	//AdImage ad("credit_card_test.jpg");
	//cout << ad.get_num_unionpay() << endl;

	//Close the handle after use or memory/resource leak
	FindClose(handle);
}