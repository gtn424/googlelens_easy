#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include<stdlib.h>
int mouse_x, mouse_y, mouse_x2, mouse_y2,counter=0,temp=0,count1=0;  // マウスの左クリックのx, y座標格納用変数
int m_x[2], m_y[2];
bool mode = false;     // マウスの左クリックの有無の記録用変数
using namespace std;
using namespace cv;

//GoogleLensの実装
//カメラに映し出されたものが何なのか、どこで売られているのか、似ているものは何なのか、取得した画像をChromeに送り、それに類似した情報を出力するというもの
//今回は、撮影したものと類似度が一番高いものの写真を探すという形で行うことにした

void on_mouse(int event, int x, int y, int flags, void* param) {  // コールバック関数

	if (event == CV_EVENT_LBUTTONDOWN) {  // 左クリック時の処理
		mouse_x = x; //四角形の座標を決定(ｘ)
		mouse_y = y; //四角形の座標を決定(ｙ)
		counter++;
	}
	if (event == CV_EVENT_RBUTTONDOWN) {
		mouse_x2 = x; //四角形の座標を決定(x)
		mouse_y2 = y; //四角形の座標を決定(y)
		counter++;
	}

	if (counter == 2) { //ボタンを二回クリックした
		std::cout << counter << std::endl;
		mode = true; //２つの座標の取得に完了したという合図
		counter = 0;
	}
}
int main(void) {
	int border = 0;
	double comp_result[8];


	for (int i = 0; i < 8; i++) {
		comp_result[i] = 0;
	}
	Mat img, r_image, temp_image, much_result, rect_image;
	Mat comp_image[8], comp_much_result[8];
	Point p1[8], p2[8]; //小さいほう、大きいほう
	Point p11, p22;
	Point forrect;


	comp_image[0] = imread("./Images/Book.jpg");
	comp_image[1] = imread("./Images/Clock.jpg");
	comp_image[2] = imread("./Images/Guitar.jpg");
	comp_image[3] = imread("./Images/Magcup.jpg");
	comp_image[4] = imread("./Images/Pen.jpg");
	comp_image[5] = imread("./Images/Sharppencil.jpg");
	comp_image[6] = imread("./Images/Speaker.jpg");
	comp_image[7] = imread("./Images/Tumbler.jpg");

	namedWindow("image");// "image"名のウィンドウを生成
	setMouseCallback("image", on_mouse, NULL); // "image"ウィンドウ内でのマウスイベントイベント発生時にon_mouse関数を呼ぶように設定
	VideoCapture camera; //カメラ
	camera.open(0); //カメラ起動(ivcamを使用)
	double max1[8], min1[8];
	double max2, min2;
	Scalar color(255, 0, 0); //色を設定
	Scalar comp_color(0, 0, 255);

	while (1) {
		camera >> img;
		if (mode) { //二つ目の座標を取得したら
			rect_image = img(Rect(min(mouse_x, mouse_x2), min(mouse_y, mouse_y2), abs(mouse_x - mouse_x2), abs(mouse_y - mouse_y2))); //画像上に四角形を生成
			rect_image.copyTo(temp_image); //四角形でくりぬいた部分をtemp_imageに格納
			mode = false; //２つの座標をリセットする
			count1++;
		}

		if (!temp_image.empty()) {
			imshow("rect_image", temp_image); //四角形でくりぬいた画像を表示

			/*-------実験その1で使用(テンプレートマッチングによる類似度算出)--------*/
			for (int i = 0; i < 8; i++) {
				matchTemplate(comp_image[i], temp_image, comp_much_result[i], CV_TM_SQDIFF); //入力画像の各画素位置とテンプレート画像との照合の良さを算出
				minMaxLoc(comp_much_result[i], &min1[i], &max1[i], &p1[i], &p2[i]); //入力画像中で画素値が最小，最大となる位置などを求める
				std::cout << "(" << i << "," << p1[i] << "," << p2[i] << "), score=" << min1[i] << "," << max1[i] << std::endl;
				Point forrect(p1[i].x + temp_image.cols, p1[i].y + temp_image.rows); 
				rectangle(img, p1[i], forrect, comp_color, 1, 8, 0); //画像に長方形を描画する．原点は画素値が最小の座標、そこからテンプレート画像の長さ分描画している
			}
			/*matchTemplate(img, temp_image, much_result, CV_TM_SQDIFF); //入力画像の各画素位置とテンプレート画像との照合の良さを算出
			minMaxLoc(much_result, &min2, &max2, &p11, &p22); //入力画像中で画素値が最小，最大となる位置などを求める
			std::cout << "(" << p11 << "," << p22 << "), score=" << min2 << "," << max2 << std::endl;
			Point forrect(p11.x + temp_image.cols, p11.y + temp_image.rows); //画像に長方形を描画する．原点は画素値が最小の座標、そこからテンプレート画像の長さ分描画している
			rectangle(img, p11,forrect, color, 1, 8, 0);*/ //画像に長方形を描画する．


			/*-------実験その２で使用(ヒストグラムを用いた類似度算出)--------*/
			/*int imageCount = 1; // 入力画像の枚数
			int channelsToUse[] = { 0 }; // 0番目のチャネルを使う
			int dimention = 1; // ヒストグラムの次元数
			int binCount = 256; // ヒストグラムのビンの数
			int binCounts[] = { binCount };
			float range[] = { 0, 256 }; // データの範囲は0～255
			const float* histRange[] = { range };

			Mat histogram1;
			calcHist(&temp_image, imageCount, channelsToUse, Mat(), histogram1, dimention, binCounts, histRange); //画像のヒストグラム生成
			Mat histogram2[8];
			temp = 0;
			for (int i = 0; i < 8; i++) {
				calcHist(&comp_image[i], imageCount, channelsToUse, Mat(), histogram2[i], dimention, binCounts, histRange);
				comp_result[i] = compareHist(histogram1, histogram2[i], CV_COMP_CORREL); //順番にテンプレート画像との類似度を格納する
				std::cout << i << "," << comp_result[i] << std::endl;
				if (i >= 1 && comp_result[i]>comp_result[i-1]) {
					temp = i;
				}
			}
			std::cout << temp << std::endl;*/
		}
		imshow("image", img);
		if (waitKey(1) == 'q') {
			break;
		}
	}
	if (count1 == 1) {
		
	}
}
