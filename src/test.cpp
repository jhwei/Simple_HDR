#include<iostream>
#include<vector>
#include<fstream>
#include<string.h>
#include<math.h>
#include"opencv2/core.hpp"
#include"opencv2/highgui.hpp"
#include"opencv2/photo.hpp"

using namespace std;
using namespace cv;

void loadImage(char* filename,vector<Mat>&images, vector<Mat>&origin_images,
		vector<float>& g_trans, vector<float>& times);
void preprocessimg(vector<Mat>& images, int height, int width,
		vector<float>& g_trans, vector<float>& a);
Mat get_hdr_large_expo(vector<Mat>&images, int height, int width);
Mat get_hdr_average(vector<Mat>&images, int height, int width);
Mat get_hdr_weighted(vector<Mat>&images, int height, int width,
		vector<float>& a);
Mat get_tonemapping(Mat hdr);

int main(int argc, char* argv[])
{
	  if (argc < 2) {
	    cout << "Usage: hdr list.txt (may use other file)\n";
	    exit(-1);
	  }
	vector<Mat> images;
	vector<Mat> origin_images;
	vector<float> g_trans;
	vector<float> a;
	vector<float> times;

	loadImage(argv[1],images, origin_images, g_trans, times);
	a.push_back(times[0] / times[2]);
	a.push_back(times[0] / times[2]);
	a.push_back(1);

	int height = images[0].rows;
	int width = images[0].cols;

	preprocessimg(images, height, width, g_trans, a);

	Mat hdr_large_expo, hdr_average, hdr_weighted;

	hdr_large_expo = get_hdr_large_expo(images, height, width);
	hdr_average = get_hdr_average(images, height, width);
	hdr_weighted = get_hdr_weighted(images, height, width, a);

	Mat tonemapped;

	tonemapped = get_tonemapping(hdr_large_expo);
	namedWindow("large exposure time", CV_WINDOW_NORMAL);
	resizeWindow("large exposure time", 1024, 768);
	imshow("large exposure time", tonemapped);
	//imwrite("let.jpg",tonemapped*255);

	tonemapped = get_tonemapping(hdr_average);
	namedWindow("average", CV_WINDOW_NORMAL);
	resizeWindow("average", 1024, 768);
	imshow("average", tonemapped);
//	imwrite("ave.jpg",tonemapped*255);

	tonemapped = get_tonemapping(hdr_weighted);
	namedWindow("weighted", CV_WINDOW_NORMAL);
	resizeWindow("weighted", 1024, 768);
	imshow("weighted", tonemapped);
	//imwrite("weight.jpg",tonemapped*255);

	Mat response;
	Ptr<CalibrateDebevec> calibrate = createCalibrateDebevec();
	calibrate->process(origin_images, response, times);
	Mat hdr;
	Ptr<MergeDebevec> merge_debevec = createMergeDebevec();
	merge_debevec->process(origin_images, hdr, times, response);

	tonemapped = get_tonemapping(hdr);
	namedWindow("hdr_by_opencv", CV_WINDOW_NORMAL);
	resizeWindow("hdr_by_opencv", 1024, 768);
	imshow("hdr_by_opencv", tonemapped);
//	imwrite("opencv.jpg",tonemapped*255);

	waitKey(0);
	return 0;
}

void loadImage(char* filename,vector<Mat>& images, vector<Mat>& origin_images,
		vector<float>& g_trans, vector<float> &times)
{
	String path = "./images/";
	ifstream list_file((path + filename).c_str());
	string name;
	float val;
	for (int i = 0; i < 3; i++)
	{
		list_file >> val;
		g_trans.push_back(1);
	}
	while (list_file >> name >> val)
	{
		Mat img = imread((path + name).c_str(), IMREAD_COLOR);
		times.push_back(1 / val);
		origin_images.push_back(img);
		Mat float_img;
		img.convertTo(float_img, CV_32FC3);
		images.push_back(float_img);
	}
	list_file.close();
}

void preprocessimg(vector<Mat>& images, int height, int width,
		vector<float>& g_trans, vector<float>& a)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			for (unsigned int num = 0; num < images.size(); num++)
				for (int channel = 0; channel < 3; channel++)
				{
					if (images[num].at<Vec3f>(i, j)[channel] < 255)
						images[num].at<Vec3f>(i, j)[channel] = pow(
								images[num].at<Vec3f>(i, j)[channel],
								g_trans[channel]) / a[num];
					else
					{
						images[num].at<Vec3f>(i, j)[0] = -1;
						images[num].at<Vec3f>(i, j)[1] = -1;
						images[num].at<Vec3f>(i, j)[2] = -1;
						break;
					}
				}
	for (uint num = 0; num < images.size(); num++)
	{
		imwrite("linear_" + to_string(num) + ".jpg",
				get_tonemapping(images[num]) * 255);
	}
}

Mat get_hdr_large_expo(vector<Mat>& images, int height, int width)
{
	Mat hdr = Mat::zeros(height, width, CV_32FC3);
	int flag = 1;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			for (unsigned int num = 0; num < images.size(); num++)
			{
				flag = 1;
				for (int channel = 0; channel < 3; channel++)
				{
					if (images[num].at<Vec3f>(i, j)[channel] < 0)
					{
						flag = 0;
						break;
					}
				}
				if (flag == 0)
					continue;
				else
				{
					hdr.at<Vec3f>(i, j)[0] = images[num].at<Vec3f>(i, j)[0];
					hdr.at<Vec3f>(i, j)[1] = images[num].at<Vec3f>(i, j)[1];
					hdr.at<Vec3f>(i, j)[2] = images[num].at<Vec3f>(i, j)[2];
					break;
				}
			}
	return hdr;
}

Mat get_hdr_average(vector<Mat>&images, int height, int width)
{
	Mat hdr = Mat::zeros(height, width, CV_32FC3);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			int flag[3] =
			{ 1, 1, 1 };
			for (unsigned int num = 0; num < images.size(); num++)
			{
				for (int channel = 0; channel < 3; channel++)
				{
					if (images[num].at<Vec3f>(i, j)[channel] < 0)
					{
						flag[num] = 0;
						break;
					}
				}
			}
			for (int channel = 0; channel < 3; channel++)
			{
				for (int channel = 0; channel < 3; channel++)
				{
					float count = 0;
					float sum = 0;
					for (unsigned int num = 0; num < images.size(); num++)
					{
						if (flag[num] == 0)
							continue;
						else
						{
							sum += images[num].at<Vec3f>(i, j)[channel];
							count++;
						}
					}
					hdr.at<Vec3f>(i, j)[channel] = sum / count;
				}
			}
		}
	return hdr;
}
Mat get_hdr_weighted(vector<Mat>&images, int height, int width,
		vector<float>& a)
{
	Mat hdr = Mat::zeros(height, width, CV_32FC3);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			int flag[3] =
			{ 1, 1, 1 };
			for (unsigned int num = 0; num < images.size(); num++)
			{
				for (int channel = 0; channel < 3; channel++)
				{
					if (images[num].at<Vec3f>(i, j)[channel] < 0)
					{
						flag[num] = 0;
						break;
					}
				}
			}
			float weight[3] =
			{ 0, 0, 0 };

			if (flag[0] == 1)
			{
				weight[0] = 1 / pow(a[2], 2)
						/ (1 / pow(a[2], 2) + 1 / pow(a[1], 2)
								+ 1 / pow(a[0], 2));
				weight[1] = 1 / pow(a[1], 2)
						/ (1 / pow(a[2], 2) + 1 / pow(a[1], 2)
								+ 1 / pow(a[0], 2));
				weight[2] = 1 / pow(a[0], 2)
						/ (1 / pow(a[2], 2) + 1 / pow(a[1], 2)
								+ 1 / pow(a[0], 2));
			}
			else
			{
				if (flag[1] == 1)
				{
					weight[1] = 1 / pow(a[2], 2)
							/ (1 / pow(a[1], 2) + 1 / pow(a[2], 2));
					weight[2] = 1 / pow(a[1], 2)
							/ (1 / pow(a[1], 2) + 1 / pow(a[2], 2));
				}
				else
				{
					if (flag[2] == 1)
					{
						weight[2] = 1;
					}
					else
						weight[2] = 0;
				}
			}
			for (int channel = 0; channel < 3; channel++)
				for (unsigned int num = 0; num < images.size(); num++)
					hdr.at<Vec3f>(i, j)[channel] +=
							images[num].at<Vec3f>(i, j)[channel] * weight[num];
		}
	return hdr;
}

Mat get_tonemapping(Mat hdr)
{
	Mat tonemapped;
	Ptr<TonemapDurand> tonemap = createTonemapDurand(2.2f);
	tonemap->process(hdr, tonemapped);

	return tonemapped;
}
