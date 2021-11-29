#define STB_IMAGE_IMPLEMENTATION
#define WINDOWX 800
#define WINDOWY 800
#define pie 3.14159265358979324846

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#include <iostream>
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

using namespace std;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> uid(0.0, 0.1);
uniform_real_distribution<float> uid2(-1.0, 1.0);

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid InitBuffer();
GLchar* filetobuf(const char* file);
void InitShader();

GLuint shaderID;
GLint width, height;

GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO[4];
int img = 4;
GLuint texture[4];
int Imagenum = 0;
int widthImage,heightImage,numberOfChannel = 0;

class Plane {
public:
	GLfloat p[9];
	GLfloat n[9];
	GLfloat color[9];
	GLfloat tex[6];

	void Bind() {

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);


		glm::vec3 normal = glm::cross(glm::vec3(p[3] - p[0], p[4] - p[1], p[5] - p[2]), glm::vec3(p[6] - p[0], p[7] - p[1], p[8] - p[2]));
		for (int i = 0; i < 3; ++i) {
			n[(i * 3) + 0] = normal.x;
			n[(i * 3) + 1] = normal.y;
			n[(i * 3) + 2] = normal.z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(n), n, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tex), tex, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(3);

	}


	void Draw() {
		glUseProgram(shaderID);
		glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[Imagenum]);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};


BOOL Time, start = TRUE, TimeLoop;

float BackGround[] = { 0.0, 0.0, 0.0 };

Plane* Fvertex[100];

glm::vec4* vertex;
glm::vec4* face;
glm::vec3* outColor;


glm::mat4 TR = glm::mat4(1.0f);
glm::mat4 TR2 = glm::mat4(1.0f);


FILE* FL;
int faceNum = 0;
int Click = 0;
bool key[256];
int game = 1;

float msx, msy = 0;
int dir = 0;					// 1p 방향
bool jump = false;				// 1p 점프
float mx = 0, my = 0, mz = 0;	// 1p 위치
float fpsy = 0;					// 1p 좌 우 시야
float fpsup = 0;				// 1p 위 아래 시야
float walkmove = 0;				// 1p 걷는 흔들림
bool walkmove2 = false;

float color[3] = { 1.0,1.0,1.0 };

void ReadObj(FILE* objFile);
void keyboard(unsigned char, int, int);
void keyboard2(unsigned char key2, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Motion2(int x, int y);
void TimerFunction(int value);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void vectoplane(Plane* p);
void planecolorset(Plane* p, int a);
void InitTexture();

BOOL CrossCheckfor4p(float x1, float  y1, float x2, float  y2, float x3, float  y3, float x4, float  y4);

void InitTexture()
{
	BITMAPINFO* bmp;
	string map[4] = { "A.jpg","B.png","C.png","D.png" };
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

	glUseProgram(shaderID);
	int tLocation = glGetUniformLocation(shaderID, "outTex"); //--- outTexture 유니폼 샘플러의 위치를 가져옴
	glUniform1i(tLocation, 0); //--- 샘플러를 0번 유닛으로 설정
}

void make_vertexShaders()
{

	GLchar* vertexShaderSource;

	vertexShaderSource = filetobuf("vertex.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);



	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << endl;
		exit(-1);
	}
}
void make_fragmentShaders()
{
	GLchar* fragmentShaderSource = filetobuf("fragment.glsl");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);



	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
		exit(-1);
	}

}
GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512];
	GLuint ShaderProgramID;
	ShaderProgramID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(ShaderProgramID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(ShaderProgramID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(ShaderProgramID); //--- 세이더 프로그램 링크하기

	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program 연결 실패\n" << errorLog << endl;
		exit(-1);
	}
	glUseProgram(ShaderProgramID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	int lightPosLocation = glGetUniformLocation(ShaderProgramID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 1.0, 0.0);
	int lightColorLocation = glGetUniformLocation(ShaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, color[0], color[1], color[2]);
	return ShaderProgramID;
}
void InitShader()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	shaderID = make_shaderProgram(); //--- 세이더 프로그램 만들기
}
GLvoid InitBuffer() {
	//--- VAO 객체 생성 및 바인딩
	glGenVertexArrays(1, &VAO);
	//--- vertex data 저장을 위한 VBO 생성 및 바인딩.
	glGenBuffers(4, VBO);
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
{
	srand((unsigned int)time(NULL));
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOWX, WINDOWY); // 윈도우의 크기 지정
	glutCreateWindow("Example6");// 윈도우 생성	(윈도우 이름)
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitTexture();
	InitBuffer();

	glutMotionFunc(Motion);
	glutPassiveMotionFunc(Motion2);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard2);
	glutTimerFunc(10, TimerFunction, 1);



	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	printf("%f %f", msx, msy);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	if (start) {
		start = FALSE;
		Time = TRUE; TimeLoop = TRUE;

		FL = fopen("cube.obj", "rt");
		ReadObj(FL);
		fclose(FL);
		Fvertex[0] = (Plane*)malloc(sizeof(Plane) * faceNum);
		vectoplane(Fvertex[0]);
		planecolorset(Fvertex[0], 0);

		for (int i = 0; i < 3; ++i) {
			color[i] = 2.0;
		}


		glEnable(GL_DEPTH_TEST);

	} // 초기화할 데이터
	InitShader();

	glClearColor(BackGround[0], BackGround[1], BackGround[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	//배경

	glUseProgram(shaderID);
	glBindVertexArray(VAO);// 쉐이더 , 버퍼 배열 사용

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (game == 0) {
		unsigned int color = glGetUniformLocation(shaderID, "outColor");
		unsigned int modelLocation = glGetUniformLocation(shaderID, "model");
		unsigned int viewLocation = glGetUniformLocation(shaderID, "view");
		unsigned int projLocation = glGetUniformLocation(shaderID, "projection");

		glm::mat4 Vw = glm::mat4(1.0f);
		glm::mat4 Cp = glm::mat4(1.0f);

		Cp = glm::rotate(Cp, (float)glm::radians(fpsy), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 cameraPos = glm::vec4(mx, my, mz, 0.0f);
		glm::vec3 cameraDirection = glm::vec4(0.0, fpsup + walkmove, -2.0, 0.0f) * Cp;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

		glm::mat4 Pj = glm::mat4(1.0f);

		Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.0005f, 10.0f);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

		// 그리기 코드

		TR = glm::mat4(1.0f);
		modelLocation = glGetUniformLocation(shaderID, "model");
		TR = glm::translate(TR, glm::vec3(0.0f, 1.5f, 0.0f));
		TR = glm::scale(TR, glm::vec3(8.0, 4.0, 8.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		for (int j = 0; j < 12; ++j) {
			Fvertex[0][j].Bind();
			Fvertex[0][j].Draw();
		}

		TR = glm::mat4(1.0f);
		modelLocation = glGetUniformLocation(shaderID, "model");
		TR = glm::translate(TR, glm::vec3(1.0f, -0.1f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.5, 0.5, 0.5));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		for (int j = 0; j < 12; ++j) {
			Fvertex[0][j].Bind();
			Fvertex[0][j].Draw();
		}
	}


	else if (game == 1) {																			// 메인 화면
		unsigned int color = glGetUniformLocation(shaderID, "outColor");
		unsigned int modelLocation = glGetUniformLocation(shaderID, "model");
		unsigned int viewLocation = glGetUniformLocation(shaderID, "view");
		unsigned int projLocation = glGetUniformLocation(shaderID, "projection");

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

		TR = glm::mat4(1.0f);
		modelLocation = glGetUniformLocation(shaderID, "model");
		TR = glm::translate(TR, glm::vec3(0.0f, 0.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(1.0, 2.0, 2.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		for (int j = 0; j < 12; ++j) {
			Fvertex[0][j].Bind();
			Fvertex[0][j].Draw();
		}

	}

	glutSwapBuffers();
	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y)
{	
	if (game == 0) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			
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
	if (Time) {

		if (key['v'] == true || key['b'] == true) {
			if (walkmove2 == false) {
				walkmove += 0.003;
				if (walkmove > 0.05) {
					walkmove2 = true;
				}
			}
			else {
				walkmove -= 0.006;
				if (walkmove < 0) {
					walkmove2 = false;
				}
			}
		}

		if (key['v'] == true) {					// 1p 전진

			mx += sin((float)glm::radians(fpsy)) * 0.01;
			mz -= cos((float)glm::radians(fpsy)) * 0.01;
			dir = 3;
		}

		if (key['b'] == true) {					// 1p 후진

			mx -= sin((float)glm::radians(fpsy)) * 0.01;
			mz += cos((float)glm::radians(fpsy)) * 0.01;

			dir = 0;
		}

		if (key['w'] == true) {					// 1p 위
			fpsup += 0.01;
		}

		if (key['s'] == true) {					// 1p 아래
			fpsup -= 0.01;
		}
		if (key['a'] == true) {					// 1p 왼쪽
			fpsy--;
		}

		if (key['d'] == true) {					// 1p 오른쪽
			fpsy++;
		}

		if (key['c'] == true) {
			jump = true;
		}
		glutPostRedisplay();
	}

	glutTimerFunc(10, TimerFunction, 1);

}

BOOL CrossCheckfor4p(float x11, float y11, float x12, float y12, float x21, float y21, float x22, float y22) {
	BOOL b1, b2;

	if (((x12 - x11) * (y21 - y11) - (y12 - y11) * (x21 - x11)) * ((x12 - x11) * (y22 - y11) - (y12 - y11) * (x22 - x11)) < 0) {
		b1 = TRUE;
	}
	else {
		b1 = FALSE;
	}

	if (((x22 - x21) * (y11 - y21) - (y22 - y21) * (x11 - x21)) * ((x22 - x21) * (y12 - y21) - (y22 - y21) * (x12 - x21)) < 0) {
		b2 = TRUE;
	}
	else {
		b2 = FALSE;
	}

	if (b1 && b2) {
		return TRUE;
	}
	return FALSE;
}

GLchar* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

void ReadObj(FILE* objFile)
{
	faceNum = 0;
	//--- 1. 전체 버텍스 개수 및 삼각형 개수 세기
	char count[100];
	char bind[100];
	int vertexNum = 0;
	while (!feof(objFile)) {
		fscanf(objFile, "%s", count);
		if (count[0] == 'v' && count[1] == '\0')
			vertexNum += 1;
		else if (count[0] == 'f' && count[1] == '\0')
			faceNum += 1;
		memset(count, '\0', sizeof(count));
	}
	int vertIndex = 0;
	int faceIndex = 0;
	vertex = (glm::vec4*)malloc(sizeof(glm::vec4) * vertexNum);
	face = (glm::vec4*)malloc(sizeof(glm::vec4) * faceNum);

	fseek(objFile, 0, 0);
	while (!feof(objFile)) {
		fscanf(objFile, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(objFile, "%f %f %f",
				&vertex[vertIndex].x, &vertex[vertIndex].y, &vertex[vertIndex].z);
			vertIndex++;
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			fscanf(objFile, "%f %f %f",
				&face[faceIndex].x, &face[faceIndex].y, &face[faceIndex].z);
			int x = face[faceIndex].x - 1, y = face[faceIndex].y - 1, z = face[faceIndex].z - 1;
			faceIndex++;
		}
	}
}

void vectoplane(Plane* p) {
	for (int i = 0; i < faceNum; ++i) {
		int x = face[i].x - 1, y = face[i].y - 1, z = face[i].z - 1;
		p[i].p[0] = vertex[x].x;
		p[i].p[1] = vertex[x].y;
		p[i].p[2] = vertex[x].z;

		p[i].p[3] = vertex[y].x;
		p[i].p[4] = vertex[y].y;
		p[i].p[5] = vertex[y].z;

		p[i].p[6] = vertex[z].x;
		p[i].p[7] = vertex[z].y;
		p[i].p[8] = vertex[z].z;
	}
}

void planecolorset(Plane* p, int a) {
	if (a == 0) {										// 랜덤 색
		for (int i = 0; i < faceNum; i += 2) {
			for (int j = 0; j < 3; ++j) {
				p[i].color[j * 3] = 1.0;
				p[i].color[j * 3 + 1] = 1.0;
				p[i].color[j * 3 + 2] = 1.0;

				p[i + 1].color[j * 3] = 1.0;
				p[i + 1].color[j * 3 + 1] = 1.0;
				p[i + 1].color[j * 3 + 2] = 1.0;
			}
		}
		for (int i = 0; i < faceNum; ++i) {
			if (i % 2 == 0) {
				p[i].tex[0] = 0.0;
				p[i].tex[1] = 1.0;
				p[i].tex[2] = 0.0;
				p[i].tex[3] = 0.0;
				p[i].tex[4] = 1.0;
				p[i].tex[5] = 1.0;
			}
			else {
				p[i].tex[0] = 1.0;
				p[i].tex[1] = 0.0;
				p[i].tex[2] = 1.0;
				p[i].tex[3] = 1.0;
				p[i].tex[4] = 0.0;
				p[i].tex[5] = 0.0;
			}
		}
	}
	if (a == 1) {										// 하얀 색
		for (int i = 0; i < faceNum; i += 2) {
			for (int j = 0; j < 3; ++j) {
				p[i].color[j * 3] = 1.0;
				p[i].color[j * 3 + 1] = 1.0;
				p[i].color[j * 3 + 2] = 1.0;

				p[i + 1].color[j * 3] = 1.0;
				p[i + 1].color[j * 3 + 1] = 1.0;
				p[i + 1].color[j * 3 + 2] = 1.0;
			}
		}
		for (int i = 0; i < faceNum; ++i) {
			if (i % 2 == 0) {
				p[i].tex[0] = 0.0;
				p[i].tex[1] = 1.0;
				p[i].tex[2] = 0.0;
				p[i].tex[3] = 0.0;
				p[i].tex[4] = 1.0;
				p[i].tex[5] = 1.0;
			}
			else {
				p[i].tex[0] = 1.0;
				p[i].tex[1] = 0.0;
				p[i].tex[2] = 1.0;
				p[i].tex[3] = 1.0;
				p[i].tex[4] = 0.0;
				p[i].tex[5] = 0.0;
			}
		}
	}
	if (a == 2) {										// 하얀 색
		for (int i = 0; i < faceNum; i += 2) {
			for (int j = 0; j < 3; ++j) {
				p[i].color[j * 3] = 1.0;
				p[i].color[j * 3 + 1] = 0.5;
				p[i].color[j * 3 + 2] = 0.0;

				p[i + 1].color[j * 3] = 1.0;
				p[i + 1].color[j * 3 + 1] = 0.5;
				p[i + 1].color[j * 3 + 2] = 0.0;
			}
		}
	}
}



