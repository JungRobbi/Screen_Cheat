#define  _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define WINDOWX 800
#define WINDOWY 800
#define pie 3.14159265358979324846 // �� ����

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <time.h>
#include <stdlib.h>
#include <cmath>
#include <cctype>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "stb_image.h"
#include "shader.h"
#include "objRead.cpp"


using namespace std;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> uid(-3.5, 3.5);

GLuint g_window_w = 800;
GLuint g_window_h = 800;

GLuint VAO[100];
GLuint VBO_position[100];
GLuint VBO_normal[100];
GLuint VBO_uv[100];

int polygon_mode = 2;



void keyboard(unsigned char, int, int);
void keyboard2(unsigned char key2, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Motion2(int x, int y);
void TimerFunction(int value);
void Display();
void Reshape(int w, int h);
void InitBuffer();
void InitTexture();
void TimerFunction(int value);

void InitBuffer_bind(const int);


// obj �б� ����

int loadObj(const char* filename);
int loadObj_normalize_center_3f(const char* filename);
int loadObj_normalize_center_4f(const char* filename);
float* sphere_object;
int num_Triangle;
int num_vertices = 3;
int num_triangles = 1;
int num_Sphere = 0;
int num_cube = 0;
int num_Gun;
int num_Tank;

float sunSize;
int shape = 1;					// �ҷ��� ��� (1. ����ü, 2. ��)

// �ؽ��� ����

int img = 7;
GLuint texture[8];
int Imagenum = 0;
int widthImage, heightImage, numberOfChannel = 0;

// ���� ����

int game = 2;					// ���� state

int intmpx = 0;
int intmpy = 0;

float fpsy = 0;					// 1p �� �� �þ�
float fpsup = 0;				// 1p �� �Ʒ� �þ�
float fpsy2 = 0;					// 1p �� �� �þ�
float fpsup2 = 0;				// 1p �� �Ʒ� �þ�
float walkmove = 0;				// 1p �ȴ� ��鸲
bool walkmove2 = false;
float shootmove = 0;			// 1p �� ��鸲
bool shootmove2 = false;
bool shoot = false;

float cx = 0;					// ī�޶� x
float cz = 0;					// ī�޶� z
float ry = 0;					// ȸ��
float boxx[4];
float boxy[4];

int clone[5];					// ����
float mx[6];					// ĳ���͵� x
float my[6];					// ĳ���͵� y
float mz[6];					// ĳ���͵� z
float leg[6];					// ĳ���͵� �ٸ� ������
float turn[6];					// ĳ���͵� ����
bool walk[6];					// ĳ���͵� �ȱ� üũ1
bool walk2[6];					// ĳ���͵� �ȱ� üũ2
bool jump[6];					// ĳ���͵� ���� üũ
float savey[6];					// ĳ���͵� ���� �� y����
int dir[6];						// ĳ���͵� ����

float mousex = 0;				// ���콺 x
float mousey = 0;				// ���콺 y

float makelegX[5];				// ���� �ٸ������ x
float makelegY[5];				// ���� �ٸ������ y
float makehead[5];				// ���� �Ӹ������ x
float makearmX[5];				// ���� �ȸ���� x
float makearmY[5];				// ���� �ȸ���� y
float makenose[5];				// ���� �ڸ����

float boom[6];					// ĳ���͵� ����
bool fireball = false;			// ����

int dir2;						// ���ΰ��� ������ ������
float clonespeed[5];			// ���� �̵��ӵ�

float turnY2 = 0;
float boxturn = 0;
int num = 0;
float zsize = 1;

int now;
float color2[3];
float fb[4];

float dieing = 0;				// �״� �ִϸ��̼�
bool die[6];					// ĳ���� ���� üũ

glm::vec3 cameraPos = glm::vec4(mx[0], my[0], mz[0], 0.0f);
glm::vec3 cameraDirection = glm::vec4(0.0, fpsup + walkmove, -2.0, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 gunPos = glm::vec3(0.0f, 1.0f, 0.0f);

// �� ����� ����

int itemnum = 0;
int itemkind = 0;

class Item {
private:

public:
	int state = 0;											// 0 = ���콺 ��		1 = ��ġ��
	int kind = 0;
	float left = 0;
	float right = 0;
	float top = 0;
	float bottom = 0;
	float front = 0;
	float back = 0;
	float x = 0;
	float z = 0;
	float plus = 0;

	glm::vec3 s;
	glm::vec3 r;
	glm::vec3 t;

	void Update() {
		if (this->state == 0) {
			this->kind = itemkind;
		}

		if (this->kind == 0) {								// ���� ����
			this->s = glm::vec3(0.1f + this -> plus, 0.1f + this->plus, 0.1f + this->plus);
		}
		else if (this->kind == 1) {							// ���� öâ
			this->s = glm::vec3(1.0f + this->plus, 1.0f + this->plus, 0.001f);
		}
		else if (this->kind == 2) {							// ���� öâ
			this->s = glm::vec3(0.001f, 1.0f + this->plus, 1.0f + this->plus);
		}
		else if (this->kind == 3) {							// ��ũ
			this->s = glm::vec3(0.7f + this->plus, 0.4f + this->plus, 0.6f + this->plus);
		}

		this->t = glm::vec3(this -> x, 0.0f, this-> z);
	}

	void Draw() {
		if (this->kind == 0) {								// ��������
			glBindVertexArray(VAO[0]);
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
		}
		else if (this->kind == 1) {							// ���� öâ
			glBindVertexArray(VAO[0]);
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
		}
		else if (this->kind == 2) {							// ���� öâ
			glBindVertexArray(VAO[0]);
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
		}
		else if (this->kind == 3) {							// ��ũ
			glBindVertexArray(VAO[1]);
			glBindTexture(GL_TEXTURE_2D, texture[4]);
			glDrawArrays(GL_TRIANGLES, 0, num_Tank);
		}
	}
};

Item item[100];



// ���� ����

glm::mat4 TR = glm::mat4(1.0f);
int Click = 0;
bool key[256];
float msx, msy = 0;
bool start = false;


int main(int argc, char** argv)
{
	// create window using freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(g_window_w, g_window_h);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Computer Graphics");

	//////////////////////////////////////////////////////////////////////////////////////
	//// initialize GLEW
	//////////////////////////////////////////////////////////////////////////////////////
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW OK\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//// Create shader program an register the shader
	//////////////////////////////////////////////////////////////////////////////////////

	GLuint vShader[4];
	GLuint fShader[4];

	vShader[0] = MakeVertexShader("vertex.glvs", 0);
	fShader[0] = MakeFragmentShader("fragment.glfs", 0);

	// shader Program
	s_program[0] = glCreateProgram();
	glAttachShader(s_program[0], vShader[0]);
	glAttachShader(s_program[0], fShader[0]);
	glLinkProgram(s_program[0]);
	checkCompileErrors(s_program[0], "PROGRAM");

	InitBuffer();
	InitTexture();

	// callback functions
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);

	glutPassiveMotionFunc(Motion2);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard2);
	glutTimerFunc(10, TimerFunction, 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// freeglut ������ �̺�Ʈ ó�� ����. �����찡 ���������� ���ѷ��� ����.
	glutMainLoop();

	return 0;
}


void InitBuffer()
{

	//// 5.1. VAO ��ü ���� �� ���ε�
	glGenVertexArrays(10, VAO);
	glGenBuffers(10, VBO_position);
	glGenBuffers(10, VBO_normal);
	glGenBuffers(10, VBO_uv);

}

void InitBuffer_bind(const int street) {
	if (street == 0) {
		num_Triangle = loadObj_normalize_center_3f("cube.obj");
	}
	else if (street == 1) {
		num_Tank = loadObj_normalize_center_4f("tank.obj");
	}
	else if (street == 2) {
		num_Gun = loadObj_normalize_center_3f("gun.obj");
	}
	else if (street == 3) {
		num_Sphere = loadObj_normalize_center_3f("sphere.obj");
	}


	glUseProgram(s_program[0]);
	glBindVertexArray(VAO[street]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position[street]);
	glBufferData(GL_ARRAY_BUFFER, outvertex.size() * sizeof(glm::vec3), &outvertex[0], GL_STATIC_DRAW);
	GLint pAttribute = glGetAttribLocation(s_program[0], "aPos");
	glVertexAttribPointer(pAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pAttribute);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_normal[street]);
	glBufferData(GL_ARRAY_BUFFER, outnormal.size() * sizeof(glm::vec3), &outnormal[0], GL_STATIC_DRAW);
	GLint nAttribute = glGetAttribLocation(s_program[0], "aNormal");
	glVertexAttribPointer(nAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(nAttribute);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_uv[street]);
	glBufferData(GL_ARRAY_BUFFER, outuv.size() * sizeof(glm::vec3), &outuv[0], GL_STATIC_DRAW);
	GLint tAttribute = glGetAttribLocation(s_program[0], "aTex");
	glVertexAttribPointer(tAttribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(tAttribute);

	outvertex = std::vector< glm::vec3 >(0.0f);  // ���� obj �ҷ����� ���� �ʱ�ȭ
	outnormal = std::vector< glm::vec3 >(0.0f);
	outuv = std::vector< glm::vec2 >(0.0f);

	vertexIndices = std::vector< unsigned int >(0.0f);
	uvIndices = std::vector< unsigned int >(0.0f);
	normalIndices = std::vector< unsigned int >(0.0f);
	temp_vertices = std::vector< glm::vec3 >(0.0f);
	temp_uvs = std::vector< glm::vec2 >(0.0f);
	temp_normals = std::vector< glm::vec3 >(0.0f);
}


void InitTexture()
{
	BITMAPINFO* bmp;
	string map[8] = { "A.png","B.png","C.png","D.png","E.png","body.png","face.png","gun_tex.png" };
	glGenTextures(8, texture); //--- �ؽ�ó ����

	for (int i = 0; i < 8; ++i) {
		glBindTexture(GL_TEXTURE_2D, texture[i]); //--- �ؽ�ó ���ε�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); //--- ���� ���ε��� �ؽ�ó�� �Ķ���� �����ϱ�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(map[i].c_str(), &widthImage, &heightImage, &numberOfChannel, 0);//--- �ؽ�ó�� ����� ��Ʈ�� �̹��� �ε��ϱ�
		glTexImage2D(GL_TEXTURE_2D, 0, 3, widthImage, heightImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //---�ؽ�ó �̹��� ����
		stbi_image_free(data);
	}

	glUseProgram(s_program[0]);
	int tLocation = glGetUniformLocation(s_program[0], "outTex"); //--- outTexture ������ ���÷��� ��ġ�� ������
	glUniform1i(tLocation, 0); //--- ���÷��� 0�� �������� ����
}

void Display()
{
	//*************************************************************************
	// ���� ����(�ѹ� �� ���� �ؾ���)
	if (!start) {
		// obj�� ���� ����� �߰��� bind �ϰ�, ��ȣ�� �´� VAO[n]�� �����
		InitBuffer_bind(0); // 0 : ������ü, 1 : ��ũ, 2 : ��
		InitBuffer_bind(1);
		InitBuffer_bind(2);
		InitBuffer_bind(3);

		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < 4; ++i) {
			boxx[i] = uid(dre);
			boxy[i] = uid(dre);

		}

		for (int i = 0; i < 12; ++i) {										// (0,1) (2,3) (4,5) (6,7) (8,9) (10,11) = ĳ����
			if (i < 6) {
				walk[i] = false;
				walk2[i] = false;
				jump[i] = false;
				savey[i] = 0.0;
				dir[i] = 1;
				mx[i] = uid(dre);
				my[i] = 0.5;
				mz[i] = uid(dre);
				leg[i] = 0;
				turn[i] = 0;
				if (i < 5) {
					clone[i] = 0;
					makelegX[i] = 0;
					makelegY[i] = 0;
					makehead[i] = 0;
					makearmX[i] = 0;
					makearmY[i] = 0;
					makenose[i] = 0;

				}
				boom[i] = 0;
				die[i] = false;
			}
		}


		for (int i = 0; i < 3; ++i) {
			color2[i] = 1.0;
		}

		clonespeed[0] = 0.007;
		clonespeed[1] = 0.009;
		clonespeed[2] = 0.011;
		clonespeed[3] = 0.013;
		clonespeed[4] = 0.015;

		start = true;
	}


	//*************************************************************************
	// ��� ����

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (game == 0) {
		ShowCursor(false);
	}
	else {
		ShowCursor(true);
	}


	//*************************************************************************
	// ī�޶� ����
	unsigned int color = glGetUniformLocation(s_program[0], "outColor");
	unsigned int modelLocation = glGetUniformLocation(s_program[0], "model");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "view");
	unsigned int projLocation = glGetUniformLocation(s_program[0], "projection");

	glm::mat4 Vw = glm::mat4(1.0f);
	glm::mat4 Cp = glm::mat4(1.0f);

	Cp = glm::rotate(Cp, (float)glm::radians(fpsy), glm::vec3(0.0f, 1.0f, 0.0f));

	cameraPos = glm::vec4(mx[0], my[0], mz[0], 0.0f);
	//cameraDirection = glm::vec4(0.0, fpsup + walkmove, -2.0, 0.0f) * Cp;
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



	Vw = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

	glm::mat4 Pj = glm::mat4(1.0f);

	Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.0005f, 40.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

	//*************************************************************************
	// ���� ����


	int lightPosLocation = glGetUniformLocation(s_program[0], "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 5.0, 3.0);
	int lightColorLocation = glGetUniformLocation(s_program[0], "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);

	//*************************************************************************
	// �׸��� �κ�

	glUseProgram(s_program[0]);



	if (game == 0) {
		glViewport(0, 0, WINDOWX, WINDOWY);

		glm::mat4 Vw = glm::mat4(1.0f);
		glm::mat4 Cp = glm::mat4(1.0f);

		Cp = glm::rotate(Cp, (float)glm::radians(fpsy), glm::vec3(0.0f, 1.0f, 0.0f));

		cameraPos = glm::vec4(mx[0], my[0], mz[0], 0.0f);
		//cameraDirection = glm::vec4(0.0, 0.0, -4.0, 0.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



		Vw = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

		glm::mat4 Pj = glm::mat4(1.0f);

		Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.0005f, 40.0f);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

		Imagenum = 1;

		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);																		// ��
		TR = glm::translate(TR, glm::vec3(0.0f, 4.5f, 0.0f));
		TR = glm::scale(TR, glm::vec3(7.0, 10.0, 7.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

		Imagenum = 3;

		glBindVertexArray(VAO[1]);
		TR = glm::mat4(1.0f);																		// ��������
		TR = glm::translate(TR, glm::vec3(0.0f, -0.1f, -2.0f));
		TR = glm::scale(TR, glm::vec3(0.7, 0.4, 0.6));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Tank);

		Imagenum = 2;

		glBindVertexArray(VAO[2]);
		TR = glm::mat4(1.0f);																		// ��
		TR = glm::translate(TR, glm::vec3(1.0f, 0.0f, 1.0f));
		TR = glm::rotate(TR, (float)glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.005, 0.005, 0.005));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Gun);

		glBindVertexArray(VAO[0]);
		// Ŭ�� ��
		for (int i = 1; i < 6; ++i) {
			Imagenum = 0;

			if (clone[i - 1] == 0) {
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], -0.4, mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(boxturn), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.1, 0.1, 0.1));

				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
			}
		}

		glBindVertexArray(VAO[0]);
		// ��ֹ�
		for (int i = 0; i < 4; ++i) {
			Imagenum = 2;
			TR = glm::mat4(1.0f);
			TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
			TR = glm::translate(TR, glm::vec3(boxx[i], -0.4, boxy[i]));
			TR = glm::rotate(TR, (float)glm::radians(boxturn), glm::vec3(0.0f, 1.0f, 0.0f));
			TR = glm::rotate(TR, 90.0f, glm::vec3(1.0f, 0.0f, 1.0f));
			TR = glm::scale(TR, glm::vec3(0.1, 0.1, 0.1));

			modelLocation = glGetUniformLocation(s_program[0], "model");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
			glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
			glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
		}

		glBindVertexArray(VAO[0]);
		// Ŭ�� ���������
		for (int i = 1; i < 6; ++i) {
			if (clone[i - 1] == 1) {
				Imagenum = 5;
				glBindVertexArray(VAO[0]);
				// �� �ٸ� �׸���
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i], mz[i]));

				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(-makelegX[i - 1], makelegY[i - 1], 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));

				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// ���� �ٸ� �׸���
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(makelegX[i - 1], makelegY[i - 1], 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// �� �� �׸���
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(-makearmX[i - 1], makearmY[i - 1], 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// ���� �� �׸���
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(makearmX[i - 1], makearmY[i - 1], 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));

				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// �� �׸���
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i] + 0.075, mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.05, 0.05, 0.03));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// �Ӹ� �׸���
				Imagenum = 6;
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i] + makehead[i - 1], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.04, 0.04, 0.04));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
				Imagenum = 5;
			}
		}
		// Ŭ�� �׸�
		glBindVertexArray(VAO[0]);

		for (int i = 1; i < 6; ++i) {																			// ĳ����
			Imagenum = 5;
			if (i == 0 || (i != 0 && clone[i - 1] == 2)) {
				// �� �ٸ� �׸���
				TR = glm::mat4(1.0f);
				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i] - boom[i], my[i] - boom[i], mz[i]));

				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(-0.01, 0.03, 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));

				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// ���� �ٸ� �׸���
				TR = glm::mat4(1.0f);

				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i] + boom[i], my[i] - boom[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.01, 0.03, 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// �� �� �׸���
				TR = glm::mat4(1.0f);

				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i] - boom[i], my[i] + boom[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(-0.03, 0.08, 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// ���� �� �׸���
				TR = glm::mat4(1.0f);

				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i] + boom[i], my[i] + boom[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.03, 0.08, 0.0));

				TR = glm::translate(TR, glm::vec3(0.0, 0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(leg[i]), glm::vec3(1.0f, 0.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(0.0, -0.025, 0.0));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.01));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// �� �׸���
				TR = glm::mat4(1.0f);

				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i] + 0.08, mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.03, 0.04, 0.03));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

				// �Ӹ� �׸���
				Imagenum = 6;
				TR = glm::mat4(1.0f);

				TR = glm::rotate(TR, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::translate(TR, glm::vec3(mx[i], my[i] + 0.15 + boom[i], mz[i]));
				TR = glm::rotate(TR, (float)glm::radians(turn[i]), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0f, 1.0f, 0.0f));
				TR = glm::scale(TR, glm::vec3(0.04, 0.04, 0.04));
				modelLocation = glGetUniformLocation(s_program[0], "model");
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
				glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
				glDrawArrays(GL_TRIANGLES, 0, num_Triangle);
				Imagenum = 5;
			}
		}


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ����

		Imagenum = 2;																				// öâ
		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);
		TR = glm::translate(TR, glm::vec3(1.5f, -0.1f, 0.0f));
		TR = glm::scale(TR, glm::vec3(1.0, 1.0, 0.01));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

		glBindVertexArray(VAO[3]);

		if (fireball == true) {
			TR = glm::mat4(1.0f);
			TR = glm::translate(TR, glm::vec3(fb[0], fb[1], fb[2]));
			TR = glm::rotate(TR, (float)glm::radians(boxturn), glm::vec3(0.0f, 1.0f, 0.0f));
			TR = glm::scale(TR, glm::vec3(0.03, 0.03, 0.03));

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
			glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
			glDrawArrays(GL_TRIANGLES, 0, num_Sphere);
		}


		glEnable(GL_CULL_FACE);

		Imagenum = 7;

		glBindVertexArray(VAO[0]);

		gunPos.x = cos(glm::radians(fpsup)) * cos(glm::radians(fpsy)) * 0.2 + cameraPos.x;
		gunPos.y = sin(glm::radians(fpsup)) * 0.2 + cameraPos.y + walkmove * 0.4 - 0.04;
		gunPos.z = cos(glm::radians(fpsup)) * sin(glm::radians(fpsy)) * 0.2 + cameraPos.z;

		TR = glm::mat4(1.0f);

		TR = glm::translate(TR, glm::vec3(gunPos.x, gunPos.y, gunPos.z));

		TR = glm::rotate(TR, (float)glm::radians(-fpsy), glm::vec3(0.0, 1.0, 0.0));
		TR = glm::rotate(TR, (float)glm::radians(fpsup), glm::vec3(0.0, 0.0, 1.0));
		TR = glm::rotate(TR, (float)glm::radians(shootmove), glm::vec3(1.0, 0.0, 0.0));
		TR = glm::scale(TR, glm::vec3(0.01, 0.05, 0.05));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);


		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND); // ���� ����






	}

	else if (game == 1) {																			// ���� ȭ��

		lightPosLocation = glGetUniformLocation(s_program[0], "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
		glUniform3f(lightPosLocation, 1.0, 0.0, 0.0);
		lightColorLocation = glGetUniformLocation(s_program[0], "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
		glUniform3f(lightColorLocation, 0.6, 0.6, 0.6);


		glm::mat4 Vw = glm::mat4(1.0f);
		glm::mat4 Cp = glm::mat4(1.0f);

		glm::vec3 cameraPos = glm::vec4(1.0, 0.0, 0.0, 0.0f);
		glm::vec3 cameraDirection = glm::vec4(-1.0, 0.0, 0.0, 0.0f) * Cp;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

		glm::mat4 Pj = glm::mat4(1.0f);

		Pj = Pj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.00f);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

		// �׸��� �ڵ�
		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);
		TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.5, 1.0, 1.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

	}

	else if (game == 2) {																			// �� �����

		glm::vec3 cameraPos = glm::vec4(0.0, 14.0, 0.0, 0.0f);
		glm::vec3 cameraDirection = glm::vec4(0.0, -1.0, 0.0, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

		Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

		glm::mat4 Pj = glm::mat4(1.0f);

		Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.0005f, 40.0f);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);


		// �׸��� �ڵ�



		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ����

		Imagenum = 1;
		for (int i = 0; i < itemnum+1; ++i) {
			item[i].Update();
			TR = glm::mat4(1.0f);																		// ������Ʈ �׸���
			TR = glm::translate(TR, glm::vec3(item[i].t));
			TR = glm::scale(TR, glm::vec3(item[i].s));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));
			item[i].Draw();
		}

		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);																		// ��
		TR = glm::translate(TR, glm::vec3(0.0f, 4.5f, 0.0f));
		TR = glm::scale(TR, glm::vec3(7.0, 10.0, 7.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

		Imagenum = 2;																				// öâ
		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);
		TR = glm::translate(TR, glm::vec3(1.5f, -0.1f, 0.0f));
		TR = glm::scale(TR, glm::vec3(1.0, 1.0, 0.01));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

		glDisable(GL_BLEND); // ���� ����

	}

	glutSwapBuffers();

}


void Reshape(int w, int h)
{
	g_window_w = w;
	g_window_h = h;
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y)
{
	msx = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2);
	msy = -((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2);

	if (game == 1) {

		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

			if (msx > -0.8 && msx<-0.2 && msy>-0.8 && msy < -0.2) {
				game = 2;
			}
			else if (msx < 0.8 && msx > 0.2 && msy > -0.8 && msy < -0.2) {
				game = 0;
			}


		}


	}
	else if (game == 0) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			mousex = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 90;
			mousey = -((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2) * 1.5;
			Click = 1;
			fireball = true;
			fb[0] = mx[0];
			fb[1] = my[0];
			fb[2] = mz[0];
			fb[3] = 1;
			shoot = true;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			fpsy += fpsy2;
			fpsup += fpsup2;
			fpsy2 = 0;
			fpsup2 = 0;
			Click = 0;
		}
	}
	else if (game == 2) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			item[itemnum].x = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 5;
			item[itemnum].z = ((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2) * 5;
			
			item[itemnum].state = 1;
			itemnum++;

			Click = 1;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			Click = 0;
		}
	}
}
void Motion(int x, int y)
{

}

void Motion2(int x, int y)
{
	if (game == 0) {
		if (x > WINDOWX - 100 || x < 100 || y > WINDOWY - 100 || y < 100) {
			SetCursorPos(WINDOWX / 2, WINDOWY / 2);
		}

		float xoffset = x - intmpx;
		float yoffset = intmpy - y;
		intmpx = x;
		intmpy = y;

		if (abs(xoffset) < WINDOWX / 4 && abs(yoffset) < WINDOWX / 4) {

			xoffset *= 0.2;
			yoffset *= 0.2;

			fpsy += xoffset;
			fpsup += yoffset;


			if (fpsup > 89.0f)
				fpsup = 89.0f;
			if (fpsup < -89.0f)
				fpsup = -89.0f;

		}

	}
	else if (game == 2) {
		item[itemnum].x = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 5;
		item[itemnum].z = ((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2) * 5;
	}
}

void keyboard(unsigned char key2, int x, int y) {
	key[key2] = true;

	if (game == 2) {
		if (key2 == 27) {
			exit(0);
		}
		switch (key2) {
		case '1':
			itemkind = 0;
			break;

		case '2':
			itemkind = 1;
			break;
		case '3':
			itemkind = 2;
			break;
		case '4':
			itemkind = 3;
			break;
		case '=':
			item[itemnum].plus += 0.01;
			break;
		case '-':
			item[itemnum].plus -= 0.01;
			break;
		}

	}
	else {
		if (key2 == 27) {
			exit(0);
		}
		switch (key2) {
		case ',':
			game = 0;
			break;

		case '.':
			game = 1;
			Imagenum = 0;
			break;
		case '/':
			game = 2;
			break;
		}
	}
	glutPostRedisplay();
}
void keyboard2(unsigned char key2, int x, int y) {
	key[key2] = false;
	glutPostRedisplay();
}

void TimerFunction(int value) {
	if (fireball == true) {
		if (fb[3] == 1) {
			fb[0] += cos((float)glm::radians(fpsy + fpsy2)) * 0.05;
			fb[2] += sin((float)glm::radians(fpsy + fpsy2)) * 0.05;
		}
		else if (fb[3] == 2) {
			fb[0] -= cos((float)glm::radians(fpsy + fpsy2)) * 0.05;
			fb[2] -= sin((float)glm::radians(fpsy + fpsy2)) * 0.05;
		}
		else if (fb[3] == 3) {
			fb[0] += sin((float)glm::radians(fpsy + fpsy2)) * 0.05;
			fb[2] -= cos((float)glm::radians(fpsy + fpsy2)) * 0.05;
		}
		else if (fb[3] == 4) {
			fb[0] -= sin((float)glm::radians(fpsy + fpsy2)) * 0.05;
			fb[2] += cos((float)glm::radians(fpsy + fpsy2)) * 0.05;
		}
		else {
			fb[0] -= sin((float)glm::radians(fpsy + fpsy2)) * 0.05;
			fb[2] += cos((float)glm::radians(fpsy + fpsy2)) * 0.05;
		}
		if ((fb[0] < -5.0) || (fb[0] > 5.0) || (fb[2] < -5.0) || (fb[2] > 5.0)) {
			fireball = false;
		}
	}

	for (int i = 0; i < 6; ++i) {											// ������ ����
		if (die[i] == true) {
			boom[i] += 0.05;
		}
		if ((i != 0) && boom[i] >= 1) {
			mx[i] = uid(dre);
			my[i] = 0;
			mz[i] = uid(dre);
			clone[i - 1] = 0;
			die[i] = false;
			boom[i] = 0;
			makelegX[i - 1] = 0;
			makelegY[i - 1] = 0;
			makehead[i - 1] = 0;
			makearmX[i - 1] = 0;
			makearmY[i - 1] = 0;
			makenose[i - 1] = 0;
		}
	}

	if (boom[0] >= 1) {
		boom[0] = 0;
		for (int i = 0; i < 4; ++i) {
			boxx[i] = uid(dre);
			boxy[i] = uid(dre);
		}

		for (int i = 0; i < 12; ++i) {										// (0,1) (2,3) (4,5) (6,7) (8,9) (10,11) = ĳ����
			if (i < 6) {
				walk[i] = false;
				die[i] = false;
				walk2[i] = false;
				jump[i] = false;
				savey[i] = -0.5;
				dir[i] = 1;
				mx[i] = uid(dre);
				my[i] = 0;
				mz[i] = uid(dre);;
				leg[i] = 0;
				turn[i] = 0;
				if (i < 5) {
					clone[i] = 0;
					makelegX[i] = 0;
					makelegY[i] = 0;
					makehead[i] = 0;
					makearmX[i] = 0;
					makearmY[i] = 0;
					makenose[i] = 0;

				}
			}
		}
		cx = 0;
		cz = 0;
		ry = 0;

		turnY2 = 0;
		boxturn = 0;
		num = 0;
		zsize = 1;
		dieing = 0;

	}

	for (int i = 1; i < 6; ++i) {					// �κ� ���󰡱�
		if (clone[i - 1] == 2) {

			walk[i] = true;

			if ((mx[i] <= mx[0])) {
				mx[i] += clonespeed[i - 1];
				dir[i] = 1;
			}

			if ((mz[i] <= mz[0])) {
				mz[i] += clonespeed[i - 1];
				dir[i] = 4;

			}

			if ((mx[i] >= mx[0])) {
				mx[i] -= clonespeed[i - 1];
				dir[i] = 2;

			}

			if ((mz[i] >= mz[0])) {
				mz[i] -= clonespeed[i - 1];
				dir[i] = 3;
			}

			if (abs(mz[i] - mz[0]) > abs(mx[i] - mx[0])) {
				if (mz[i] >= mz[0]) {
					dir[i] = 3;
				}
				else {
					dir[i] = 4;
				}
			}
			else {
				if (mx[i] >= mx[0]) {
					dir[i] = 2;
				}
				else {
					dir[i] = 1;
				}
			}

		}
	}

	for (int i = 0; i < 6; ++i) {
		if (i == 0 || clone[i - 1] == 2) {
			if (walk[i] == true) {
				if (walk2[i] == false) {
					leg[i] += 6;
					if (leg[i] > 60) {
						walk2[i] = true;
					}
				}
				else {
					leg[i] -= 6;
					if (leg[i] < -60) {
						walk2[i] = false;
					}
				}
			}

		}

		for (int j = 0; j < 4; ++j) {																			// �÷��̾�� ��ֹ�
			if ((mx[0] - 0.025 < boxx[j] + 0.05) && (mz[0] - 0.015 < boxy[j] + 0.05) &&
				(mx[0] + 0.025 > boxx[j] - 0.05) && (mz[0] + 0.015 > boxy[j] - 0.05)) {
				die[0] = true;
			}
		}

		for (int j = 1; j < 6; ++j) {																			// �÷��̾�� ����
			if ((mx[0] - 0.025 < mx[j] + 0.025) && (mz[0] - 0.015 < mz[j] + 0.015) &&
				(mx[0] + 0.025 > mx[j] - 0.025) && (mz[0] + 0.015 > mz[j] - 0.015)) {
				if (clone[j - 1] == 2) {
					die[0] = true;
				}

			}
		}

		if (i != 0 && clone[i - 1] == 0) {																		// �÷��̾�� ��Ȱ�� ����
			if ((mx[0] - 0.025 < mx[i] + 0.05) && (mz[0] - 0.015 < mz[i] + 0.05) &&
				(mx[0] + 0.025 > mx[i] - 0.05) && (mz[0] + 0.015 > mz[i] - 0.05)) {
				clone[i - 1] = 1;
			}

		}

		if (i != 0 && clone[i - 1] == 2) {																		// ����� ���̾
			if ((mx[i] - 0.025 < fb[0] + 0.05) && (mz[i] - 0.015 < fb[2] + 0.05) &&
				(mx[i] + 0.025 > fb[0] - 0.05) && (mz[i] + 0.015 > fb[2] - 0.05) && fireball == true) {
				die[i] = true;
				fireball = false;
			}

		}

		if (jump[i] == true) {
			my[i] += 0.01;

			if (my[i] > 0.2) {
				jump[i] = false;
			}
		}

		else {
			my[i] -= 0.015;
			if (my[i] < savey[i]) {
				my[i] = savey[i];
			}
		}

		walk[i] = false;
	}

	for (int i = 0; i < 5; ++i) {					// �κ� ��������� ����
		if (clone[i] == 1) {
			makelegX[i] += 0.0001;
			makelegY[i] += 0.0003;

			makearmX[i] += 0.0001;
			makearmY[i] += 0.0008;

			makehead[i] += 0.0012;

			if (makelegX[i] > 0.01) {
				clone[i] = 2;
			}
		}
	}


	if (shoot == true) {
		if (shootmove2 == false && shootmove <= 5) {
			shootmove += 1;
			if (shootmove > 5) {
				shootmove2 = true;
			}
		}
		else {
			shootmove -= 2;
			if (shootmove < 0) {
				shootmove2 = false;
				shoot = false;
			}
		}
	}


	if (key['w'] == true || key['s'] == true || key['a'] == true || key['d'] == true) {
		if (walkmove2 == false) {
			walkmove += 0.001;
			if (walkmove > 0.02) {
				walkmove2 = true;
			}
		}
		else {
			walkmove -= 0.002;
			if (walkmove < 0) {
				walkmove2 = false;
			}
		}
	}

	else {
		walkmove = 0;
	}


	if (key['a'] == true) {						// ���� �̵�
		walk[0] = true;
		mx[0] += sin((float)glm::radians(fpsy + fpsy2)) * 0.015;
		mz[0] -= cos((float)glm::radians(fpsy + fpsy2)) * 0.015;
		dir[0] = 3;
	}
	if (key['d'] == true) {						// �Ʒ��� �̵�
		walk[0] = true;
		mx[0] -= sin((float)glm::radians(fpsy + fpsy2)) * 0.015;
		mz[0] += cos((float)glm::radians(fpsy + fpsy2)) * 0.015;
		dir[0] = 4;
	}
	if (key['s'] == true) {						// �������� �̵�
		walk[0] = true;
		mx[0] -= cos((float)glm::radians(fpsy + fpsy2)) * 0.015;
		mz[0] -= sin((float)glm::radians(fpsy + fpsy2)) * 0.015;
		dir[0] = 2;
	}
	if (key['w'] == true) {						// ���������� �̵�
		walk[0] = true;
		mx[0] += cos((float)glm::radians(fpsy + fpsy2)) * 0.015;
		mz[0] += sin((float)glm::radians(fpsy + fpsy2)) * 0.015;
		dir[0] = 1;
	}

	if (key['f'] == true) {
		jump[0] = true;
	}

	cameraDirection.x = cos(glm::radians(fpsup)) * cos(glm::radians(fpsy)) + cameraPos.x;
	cameraDirection.y = sin(glm::radians(fpsup)) + cameraPos.y + walkmove;
	cameraDirection.z = cos(glm::radians(fpsup)) * sin(glm::radians(fpsy)) + cameraPos.z;



	glutPostRedisplay();

	glutTimerFunc(10, TimerFunction, 1);

}






