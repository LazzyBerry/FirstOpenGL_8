#include <stdlib.h>
#include <stdio.h>
#include <gl/glew.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <gl/glut.h>


//�摜�t�@�C��
const char* texturefile = "sampleImage.png";//256*256�̉摜
//�e�N�X�`��ID
GLuint texId[4];
int filter = 0;//�t�B���^�[���[�h

//�e��R�[���o�b�N�֐�

//�f�B�X�v���C
void display()
{
	//�o�b�t�@�̏�����
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

//�E�B���h�E�T�C�Y��ύX������
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);//�r���[�|�[�g�̐ݒ�

	glMatrixMode(GL_PROJECTION);//�ˉe�s��̐ݒ�
	glLoadIdentity();

	gluPerspective(30, (double)w / h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW); //���f���r���[�s��̐ݒ�
	glLoadIdentity();
}

//�L�[�{�[�h�C�x���g
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		filter = ++filter % 4;//0~�̊Ԃ��Ƃ�
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
	case '\033'://ESC�L�[
		glDeleteTextures(2, texId);//�e�N�X�`���̔j��
		exit(0);
		break;

	default:
		break;
	}

	glutPostRedisplay();//�ĕ`��
}

//���̑��e��ݒ�
void otherInit()
{
	//�J���[�o�b�t�@�̏�����
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glEnable(GL_DEPTH_TEST);
}

//�e�N�X�`���ǂݍ���
void LoadTexture()
{
	int loop;

	//�摜�ǂݍ���
	IplImage* imgA = cvLoadImage(texturefile, 1);
	if (imgA == NULL) {
		puts("Can`t load image");
		exit(0);
	}

	////�㉺���]�ƐF�f�[�^�̓���ւ�
	//cvFlip(imgA, imgA, 0);
	//cvCvtColor(imgA, imgA, CV_BGR2RGB);

	

	cvConvertImage(imgA, imgA, CV_CVTIMG_FLIP | CV_CVTIMG_SWAP_RB);

	//�~�b�v�}�b�v�̏k���t�B���^
	GLenum minFilter[4] = {
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR
	};

	glGenTextures(4, texId);//�e�N�X�`���I�u�W�F�N�g4��

	for (loop = 0; loop < 4; ++loop)
	{
		glBindTexture(GL_TEXTURE_2D, texId[loop]);//�o�C���h
		//�g��k���t�B���^
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter[loop]);
		//�~�b�v�}�b�v�쐻
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imgA->width, imgA->height,
			GL_RGB, GL_UNSIGNED_BYTE, imgA->imageData);
	}

	////1�ڂ̃e�N�X�`��
	//glBindTexture(GL_TEXTURE_2D, texId[0]);//�o�C���h
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgA->width, imgA->height,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, imgA->imageData);

	////2�ڂ̃e�N�X�`��
	//glBindTexture(GL_TEXTURE_2D, texId[1]);//�o�C���h
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgA->width, imgA->height,
	//	0, GL_RGB, GL_UNSIGNED_BYTE, imgA->imageData);

	//�������̊J��
	cvReleaseImage(&imgA);
}

//GLEW�̃Z�b�g�A�b�v�Ɗg���@�\�̊m�F
void setUpGLEW()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(0);
	}
	//�g���@�\�̊m�F
	if (glewIsSupported("GL_ARB_texture_non_power_of_two"))
	{
		puts("[GL_ARB_texture_non_power_of_two] : OK");
	}
	else
	{
		puts("[GL_ARB_texture_non_power_of_two] : BAD");
	}
}

//���C���֐�
int main(int argc, char* argv[])
{
	glutInit(&argc, argv); //GLUT�̏�����

	//Window�̍쐬
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); //�f�B�X�v���C���[�h
	glutCreateWindow("Texture with MipMap");

	setUpGLEW();//GLEW�̃Z�b�g�A�b�v

	//�R�[���o�b�N�֐��o�^
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	otherInit();
	LoadTexture();

	glutMainLoop(); //���C�����[�v��

	return 0;
}