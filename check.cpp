#define  _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define WINDOWX 800
#define WINDOWY 800
#define pie 3.14159265358979324846 // 난 파이

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
uniform_real_distribution<float> uid(0.0, 0.1);
uniform_real_distribution<float> uid2(-1.0, 1.0);

GLuint g_window_w = 800;
GLuint g_window_h = 800;

GLuint VAO[100];
GLuint VBO_position[100];
GLuint VBO_normal[100];
GLuint VBO_uv[100];

int polygon_mode = 2;

int num_vertices = 3;
int num_triangles = 1;
int num_sphere = 0;
int num_cube = 0;

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


// obj 읽기 변수

int loadObj(const char* filename);
int loadObj_normalize_center(const char* filename);
float* sphere_object;
int num_Triangle;
float sunSize;
int shape = 1;					// 불러올 모양 (1. 육면체, 2. 구)

// 텍스쳐 변수

int img = 4;
GLuint texture[4];
int Imagenum = 0;
int widthImage, heightImage, numberOfChannel = 0;

// 게임 변수
bool start = false;

int game = 1;					// 게임 state
int dir = 0;					// 1p 방향
bool jump = false;				// 1p 점프
float mx = 0, my = 0, mz = 0;	// 1p 위치
float fpsy = 0;					// 1p 좌 우 시야
float fpsup = 0;				// 1p 위 아래 시야
float walkmove = 0;				// 1p 걷는 흔들림
bool walkmove2 = false;

// 응가 변수

glm::mat4 TR = glm::mat4(1.0f);
int Click = 0;
bool key[256];
float msx, msy = 0;


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
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(Motion2);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard2);
	glutTimerFunc(10, TimerFunction, 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// freeglut 윈도우 이벤트 처리 시작. 윈도우가 닫힐때까지 후한루프 실행.
	glutMainLoop();

	return 0;
}


void InitBuffer()
{

	//// 5.1. VAO 객체 생성 및 바인딩
	glGenVertexArrays(10, VAO);
	glGenBuffers(10, VBO_position);
	glGenBuffers(10, VBO_normal);
	glGenBuffers(10, VBO_uv);

	InitBuffer_bind(0); // 0 : 정육면체, 1 : 구
	InitBuffer_bind(1);

	glEnable(GL_DEPTH_TEST);
}

void InitBuffer_bind(const int street) {
	if (street == 0) {
		num_Triangle = loadObj_normalize_center("cube.obj");
	}
	else if (street == 1) {
		num_sphere = loadObj_normalize_center("Table.obj");
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

	outvertex = std::vector< glm::vec3 >(0.0f);  // 다음 obj 불러오기 위한 초기화
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
	string map[4] = { "A.png","B.png","C.png","D.png" };
	glGenTextures(4, texture); //--- 텍스처 생성

	for (int i = 0; i < 4; ++i) {
		glBindTexture(GL_TEXTURE_2D, texture[i]); //--- 텍스처 바인딩
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); //--- 현재 바인딩된 텍스처의 파라미터 설정하기
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(map[i].c_str(), &widthImage, &heightImage, &numberOfChannel, 0);//--- 텍스처로 사용할 비트맵 이미지 로드하기
		glTexImage2D(GL_TEXTURE_2D, 0, 3, widthImage, heightImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //---텍스처 이미지 정의
		stbi_image_free(data);
	}

	glUseProgram(s_program[0]);
	int tLocation = glGetUniformLocation(s_program[0], "outTex"); //--- outTexture 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation, 0); //--- 샘플러를 0번 유닛으로 설정
}

void Display()
{

	//*************************************************************************
	// 출력 설정

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	//*************************************************************************
	// 카메라 설정

	unsigned int modelLocation = glGetUniformLocation(s_program[0], "model");
	unsigned int viewLocation = glGetUniformLocation(s_program[0], "view");
	unsigned int projLocation = glGetUniformLocation(s_program[0], "projection");

	glm::mat4 Vw = glm::mat4(1.0f);
	glm::mat4 Cp = glm::mat4(1.0f);

	Cp = glm::rotate(Cp, (float)glm::radians(fpsy), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 cameraPos = glm::vec4(mx, my, mz, 0.0f);
	glm::vec3 cameraDirection = glm::vec4(0.0, fpsup + walkmove, -2.0, 0.0f) * Cp;
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

	glm::mat4 Pj = glm::mat4(1.0f);

	Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.0005f, 40.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

	//*************************************************************************
	// 조명 설정


	int lightPosLocation = glGetUniformLocation(s_program[0], "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 5.0, 3.0);
	int lightColorLocation = glGetUniformLocation(s_program[0], "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);

	//*************************************************************************
	// 그리기 부분

	glUseProgram(s_program[0]);


	if (game == 0) {
		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);																		// 맵
		TR = glm::translate(TR, glm::vec3(0.0f, 1.5f, 0.0f));
		TR = glm::scale(TR, glm::vec3(10.0, 4.0, 10.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		Imagenum = 1;

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

		glBindVertexArray(VAO[1]);
		TR = glm::mat4(1.0f);																		// 나무상자
		TR = glm::translate(TR, glm::vec3(1.0f, -0.1f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.2, 0.2, 0.2));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_sphere);




		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 블렌딩

		//Imagenum = 2;																				// 철창
		//glBindVertexArray(VAO[0]);
		//TR = glm::mat4(1.0f);
		//TR = glm::translate(TR, glm::vec3(1.5f, -0.1f, 0.0f));
		//TR = glm::scale(TR, glm::vec3(1.0, 1.0, 0.01));
		//glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		//glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		//glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

		glDisable(GL_BLEND); // 블렌딩 해제
	}

	else if (game == 1) {																			// 메인 화면

		lightPosLocation = glGetUniformLocation(s_program[0], "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
		glUniform3f(lightPosLocation, 1.0, 0.0, 0.0);
		lightColorLocation = glGetUniformLocation(s_program[0], "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
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

		// 그리기 코드
		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);
		TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.5, 1.0, 1.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

	}

	else if (game == 2) {																			// 맵 만들기

		glm::mat4 Vw = glm::mat4(1.0f);
		glm::mat4 Cp = glm::mat4(1.0f);

		glm::vec3 cameraPos = glm::vec4(1.0, 0.0, 0.0, 0.0f);
		glm::vec3 cameraDirection = glm::vec4(-1.0, 0.0, 0.0, 0.0f) * Cp;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

		glm::mat4 Pj = glm::mat4(1.0f);

		Pj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.00f);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

		// 그리기 코드
		glBindVertexArray(VAO[0]);
		TR = glm::mat4(1.0f);
		TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(1.0, 2.0, 2.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);
		glDrawArrays(GL_TRIANGLES, 0, num_Triangle);

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
			printf("%f %f", msx, msy);

		}


	}
}
void Motion(int x, int y)
{


}

void Motion2(int x, int y)
{


}

void keyboard(unsigned char key2, int x, int y) {
	key[key2] = true;
	switch (key2) {


	}
	glutPostRedisplay();
}
void keyboard2(unsigned char key2, int x, int y) {
	key[key2] = false;
	glutPostRedisplay();
}

void TimerFunction(int value) {
	if (key['v'] == true || key['b'] == true) {
		if (walkmove2 == false) {
			walkmove += 0.002;
			if (walkmove > 0.03) {
				walkmove2 = true;
			}
		}
		else {
			walkmove -= 0.004;
			if (walkmove < 0) {
				walkmove2 = false;
			}
		}
	}

	if (key['v'] == true) {					// 1p 전진

		mx += sin((float)glm::radians(fpsy)) * 0.02;
		mz -= cos((float)glm::radians(fpsy)) * 0.02;
		dir = 3;
	}

	if (key['b'] == true) {					// 1p 후진

		mx -= sin((float)glm::radians(fpsy)) * 0.02;
		mz += cos((float)glm::radians(fpsy)) * 0.02;

		dir = 0;
	}

	if (key['w'] == true) {					// 1p 위
		fpsup += 0.01;
	}

	if (key['s'] == true) {					// 1p 아래
		fpsup -= 0.01;
	}
	if (key['a'] == true) {					// 1p 왼쪽
		fpsy -= 1.5;
	}

	if (key['d'] == true) {					// 1p 오른쪽
		fpsy += 1.5;
	}

	if (key['c'] == true) {
		jump = true;
	}

	if (key['e'] == true) {
		shape = 1;
	}
	if (key['r'] == true) {
		shape = 2;
	}
	if (key['t'] == true) {
		shape = 3;
	}

	glutPostRedisplay();

	glutTimerFunc(10, TimerFunction, 1);

}






