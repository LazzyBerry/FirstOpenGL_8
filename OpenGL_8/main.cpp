#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <gl/glut.h>


//画像ファイル
const char* texturefile = "sampleImage.png";//256*256の画像
//テクスチャID
GLuint texId[4];
int filter = 0;//フィルターモード

//各種コールバック関数

//ディスプレイ
void display()
{
	//バッファの初期化
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	glBindTexture(GL_TEXTURE_2D, texId[filter]);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex3f(-0.5f, -0.5f, 0.f);
	glTexCoord2f(1.f, 0.f); glVertex3f(0.5f, -0.5f, 0.f);
	glTexCoord2f(1.f, 1.f); glVertex3f(0.5f, 0.5f, 0.f);
	glTexCoord2f(0.f, 1.f); glVertex3f(-0.5f, 0.5f, 0.f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

//ウィンドウサイズを変更した際
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);//ビューポートの設定

	glMatrixMode(GL_PROJECTION);//射影行列の設定
	glLoadIdentity();

	gluPerspective(30, (double)w / h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW); //モデルビュー行列の設定
	glLoadIdentity();
}

//キーボードイベント
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		filter = ++filter % 4;//0~の間をとる
		printf("now filter is %d\n", filter);
		break;

	case '1':
		filter = 0;//GL_LINEAR
		break;

	case 'n':
		filter = 1;//GL_NEAREST
		break;

	case 'q':
	case 'Q':
	case '\033'://ESCキー
		glDeleteTextures(2, texId);//テクスチャの破棄
		exit(0);
		break;

	default:
		break;
	}

	glutPostRedisplay();//再描画
}

//その他各種設定
void otherInit()
{
	//カラーバッファの初期化
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glEnable(GL_DEPTH_TEST);
}

//テクスチャ読み込み
void LoadTexture()
{
	int loop;

	//画像読み込み
	IplImage* imgA = cvLoadImage(texturefile, 1);
	if (imgA == NULL) {
		puts("Can`t load image");
		exit(0);
	}

	////上下反転と色データの入れ替え
	//cvFlip(imgA, imgA, 0);
	//cvCvtColor(imgA, imgA, CV_BGR2RGB);

	

	cvConvertImage(imgA, imgA, CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB);

	//ミップマップの縮小フィルタ
	GLenum minFilter[4] = {
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR
	};

	glGenTextures(4, texId);//テクスチャオブジェクト4つ分

	for (loop = 0; loop < 4; ++loop)
	{
		glBindTexture(GL_TEXTURE_2D, texId[loop]);//バインド
		//拡大縮小フィルタ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter[loop]);
		//ミップマップ作製
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imgA->width, imgA->height,
			GL_RGB, GL_UNSIGNED_BYTE, imgA->imageData);
	}

	////1つ目のテクスチャ
	//glBindTexture(GL_TEXTURE_2D, texId[0]);//バインド
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgA->width, imgA->height,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, imgA->imageData);

	////2つ目のテクスチャ
	//glBindTexture(GL_TEXTURE_2D, texId[1]);//バインド
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgA->width, imgA->height,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, imgA->imageData);

	//メモリの開放
	cvReleaseImage(&imgA);
}

//GLEWのセットアップと拡張機能の確認
void setUpGLEW()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(0);
	}
	//拡張機能の確認
	if (glewIsSupported("GL_ARB_texture_non_power_of_two"))
	{
		puts("[GL_ARB_texture_non_power_of_two] : OK");
	}
	else
	{
		puts("[GL_ARB_texture_non_power_of_two] : BAD");
	}
}

//メイン関数
int main(int argc, char* argv[])
{
	glutInit(&argc, argv); //GLUTの初期化

	//Windowの作成
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); //ディスプレイモード
	glutCreateWindow("Texture with MipMap");

	setUpGLEW();//GLEWのセットアップ

	//コールバック関数登録
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	otherInit();
	LoadTexture();

	glutMainLoop(); //メインループへ

	return 0;
}