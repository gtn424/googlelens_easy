#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include<stdlib.h>
int mouse_x, mouse_y, mouse_x2, mouse_y2,counter=0,temp=0,count1=0;  // �}�E�X�̍��N���b�N��x, y���W�i�[�p�ϐ�
int m_x[2], m_y[2];
bool mode = false;     // �}�E�X�̍��N���b�N�̗L���̋L�^�p�ϐ�
using namespace std;
using namespace cv;

//GoogleLens�̎���
//�J�����ɉf���o���ꂽ���̂����Ȃ̂��A�ǂ��Ŕ����Ă���̂��A���Ă�����͉̂��Ȃ̂��A�擾�����摜��Chrome�ɑ���A����ɗގ����������o�͂���Ƃ�������
//����́A�B�e�������̂Ɨގ��x����ԍ������̂̎ʐ^��T���Ƃ����`�ōs�����Ƃɂ���

void on_mouse(int event, int x, int y, int flags, void* param) {  // �R�[���o�b�N�֐�

	if (event == CV_EVENT_LBUTTONDOWN) {  // ���N���b�N���̏���
		mouse_x = x; //�l�p�`�̍��W������(��)
		mouse_y = y; //�l�p�`�̍��W������(��)
		counter++;
	}
	if (event == CV_EVENT_RBUTTONDOWN) {
		mouse_x2 = x; //�l�p�`�̍��W������(x)
		mouse_y2 = y; //�l�p�`�̍��W������(y)
		counter++;
	}

	if (counter == 2) { //�{�^������N���b�N����
		std::cout << counter << std::endl;
		mode = true; //�Q�̍��W�̎擾�Ɋ��������Ƃ������}
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
	Point p1[8], p2[8]; //�������ق��A�傫���ق�
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

	namedWindow("image");// "image"���̃E�B���h�E�𐶐�
	setMouseCallback("image", on_mouse, NULL); // "image"�E�B���h�E���ł̃}�E�X�C�x���g�C�x���g��������on_mouse�֐����ĂԂ悤�ɐݒ�
	VideoCapture camera; //�J����
	camera.open(0); //�J�����N��(ivcam���g�p)
	double max1[8], min1[8];
	double max2, min2;
	Scalar color(255, 0, 0); //�F��ݒ�
	Scalar comp_color(0, 0, 255);

	while (1) {
		camera >> img;
		if (mode) { //��ڂ̍��W���擾������
			rect_image = img(Rect(min(mouse_x, mouse_x2), min(mouse_y, mouse_y2), abs(mouse_x - mouse_x2), abs(mouse_y - mouse_y2))); //�摜��Ɏl�p�`�𐶐�
			rect_image.copyTo(temp_image); //�l�p�`�ł���ʂ���������temp_image�Ɋi�[
			mode = false; //�Q�̍��W�����Z�b�g����
			count1++;
		}

		if (!temp_image.empty()) {
			imshow("rect_image", temp_image); //�l�p�`�ł���ʂ����摜��\��

			/*-------��������1�Ŏg�p(�e���v���[�g�}�b�`���O�ɂ��ގ��x�Z�o)--------*/
			for (int i = 0; i < 8; i++) {
				matchTemplate(comp_image[i], temp_image, comp_much_result[i], CV_TM_SQDIFF); //���͉摜�̊e��f�ʒu�ƃe���v���[�g�摜�Ƃ̏ƍ��̗ǂ����Z�o
				minMaxLoc(comp_much_result[i], &min1[i], &max1[i], &p1[i], &p2[i]); //���͉摜���ŉ�f�l���ŏ��C�ő�ƂȂ�ʒu�Ȃǂ����߂�
				std::cout << "(" << i << "," << p1[i] << "," << p2[i] << "), score=" << min1[i] << "," << max1[i] << std::endl;
				Point forrect(p1[i].x + temp_image.cols, p1[i].y + temp_image.rows); 
				rectangle(img, p1[i], forrect, comp_color, 1, 8, 0); //�摜�ɒ����`��`�悷��D���_�͉�f�l���ŏ��̍��W�A��������e���v���[�g�摜�̒������`�悵�Ă���
			}
			/*matchTemplate(img, temp_image, much_result, CV_TM_SQDIFF); //���͉摜�̊e��f�ʒu�ƃe���v���[�g�摜�Ƃ̏ƍ��̗ǂ����Z�o
			minMaxLoc(much_result, &min2, &max2, &p11, &p22); //���͉摜���ŉ�f�l���ŏ��C�ő�ƂȂ�ʒu�Ȃǂ����߂�
			std::cout << "(" << p11 << "," << p22 << "), score=" << min2 << "," << max2 << std::endl;
			Point forrect(p11.x + temp_image.cols, p11.y + temp_image.rows); //�摜�ɒ����`��`�悷��D���_�͉�f�l���ŏ��̍��W�A��������e���v���[�g�摜�̒������`�悵�Ă���
			rectangle(img, p11,forrect, color, 1, 8, 0);*/ //�摜�ɒ����`��`�悷��D


			/*-------�������̂Q�Ŏg�p(�q�X�g�O������p�����ގ��x�Z�o)--------*/
			/*int imageCount = 1; // ���͉摜�̖���
			int channelsToUse[] = { 0 }; // 0�Ԗڂ̃`���l�����g��
			int dimention = 1; // �q�X�g�O�����̎�����
			int binCount = 256; // �q�X�g�O�����̃r���̐�
			int binCounts[] = { binCount };
			float range[] = { 0, 256 }; // �f�[�^�͈̔͂�0�`255
			const float* histRange[] = { range };

			Mat histogram1;
			calcHist(&temp_image, imageCount, channelsToUse, Mat(), histogram1, dimention, binCounts, histRange); //�摜�̃q�X�g�O��������
			Mat histogram2[8];
			temp = 0;
			for (int i = 0; i < 8; i++) {
				calcHist(&comp_image[i], imageCount, channelsToUse, Mat(), histogram2[i], dimention, binCounts, histRange);
				comp_result[i] = compareHist(histogram1, histogram2[i], CV_COMP_CORREL); //���ԂɃe���v���[�g�摜�Ƃ̗ގ��x���i�[����
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
