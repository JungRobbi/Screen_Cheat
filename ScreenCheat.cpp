#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <time.h>
#include <stdlib.h>
#include <cmath>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <random>

#define WINDOWX 800
#define WINDOWY 800
#define pie 3.141592

using namespace std;

random_device rd;
default_random_engine dre(rd());

uniform_real_distribution<float> uidmaxy(0.6, 0.8);
uniform_real_distribution<float> uidminy(0.1, 0.4);
uniform_real_distribution<float> uidspeed(0.007, 0.013);

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid InitBuffer();
void InitShader();
GLchar* filetobuf(const char* file);

GLuint shaderID;
GLint width, height;

GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO[3];

class Plane {
public:
	GLfloat p[9];
	GLfloat n[9];
	GLfloat color[9];

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

	}


	void Draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};

BOOL Time, start = TRUE, TimeLoop;

float BackGround[] = { 1.0, 0.9, 0.9 };
Plane* Fvertex[1];

glm::vec4* vertex;
glm::vec4* face;
glm::vec3* outColor;


glm::mat4 TR = glm::mat4(1.0f);


FILE* FL;
int faceNum = 0;
int Click = 0;
bool key[256];

float mousex = 0;
float mousey = 0;

float mx = 0.0;
float my = -0.465;
float mz = 0.7;

float fpsy = 0;
float fpsy2 = 0;
float fpsup = 0;
float fpsup2 = 0;

float rx = 0;
float ry = 0;

float turn = 0;
int dir = 0;

bool jump = false;

float dev = 0;
bool movey = false;

bool fps = false;
bool mode = false;
bool mousemode = false;

int rturn = 0;
int rturn2 = 0;


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

BOOL CrossCheckfor4p(float x1, float  y1, float x2, float  y2, float x3, float  y3, float x4, float  y4);

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
	glUseProgram(ShaderProgramID);
	int lightPosLocation = glGetUniformLocation(ShaderProgramID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, 0.0, 4.0, 0.0);
	int lightColorLocation = glGetUniformLocation(ShaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
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
	glGenBuffers(3, VBO);
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
	InitBuffer();

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(Motion2);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard2);
	glutTimerFunc(10, TimerFunction, 1);



	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	if (start) {

		cout << "x축을 몇개로 나눌까요? ( 2 ~ 20 )" << endl;
		cin >> xnum;
		cout << "y축을 몇개로 나눌까요? ( 2 ~ 20 )" << endl;
		cin >> ynum;
		if (xnum > 20 || ynum > 20 || xnum < 2 || ynum < 2) {
			glutLeaveMainLoop();
		}

		if (xnum % 2 == 0) {
			xnum2 = xnum - 1;
		}
		else {
			xnum2 = xnum;
		}
		if (ynum % 2 == 0) {
			ynum2 = ynum - 1;
		}
		else {
			ynum2 = ynum;
		}

		cout << endl << "-- 육면체 관련 --" << endl;
		cout << "   m : 육면체들이 위 아래로 움직임" << endl;
		cout << "   +/- : 육면체 속도 증가/감소" << endl;
		cout << "   v : 육면체가 낮아진다" << endl;
		cout << "   r : 미로 다시 제작" << endl;

		cout << endl << "-- 카메라 관련 --" << endl;
		cout << "   p : 원근 투영 / 직각 투영" << endl;
		cout << "   x/X : 카메라가 x축을 기준으로 양/음 방향 회전" << endl;
		cout << "   y/Y : 카메라가 y축을 기준으로 양/음 방향 회전" << endl;
		cout << "   1 : 1인칭 모드" << endl;
		cout << "   3 : 3인칭 모드" << endl;
		cout << "   b : 1인칭 모드 시 카메라 수동 이동/ 자동 이동" << endl;

		cout << endl << "-- 플레이어 관련 --" << endl;
		cout << "   w/a/s/d : 플레이어 움직임" << endl;
		cout << "   c : 플레이어 점프" << endl;

		cout << endl << "-- 제어 관련 --" << endl;
		cout << "   . : 리셋" << endl;
		cout << "   / : 미로 크기 다시 입력받기" << endl;
		cout << "   q : 프로그램 종료" << endl;


		start = FALSE;
		Time = TRUE; TimeLoop = TRUE;
		for (int i = 0; i < 400; ++i) {
			FL = fopen("cube.obj", "rt");
			ReadObj(FL);
			fclose(FL);
			Fvertex[i] = (Plane*)malloc(sizeof(Plane) * faceNum);
			vectoplane(Fvertex[i]);
			planecolorset(Fvertex[i], 0);

			speed[i / 20][i % 20] = uidspeed(dre);
			maxy[i / 20][i % 20] = uidmaxy(dre);
			miny[i / 20][i % 20] = uidminy(dre);
			by[i / 20][i % 20] = 0.5;
			mage[i / 20][i % 20] = 1;

			movecheck[i / 20][i % 20] = false;

		}
		FL = fopen("cube.obj", "rt");
		ReadObj(FL);
		fclose(FL);
		Fvertex[400] = (Plane*)malloc(sizeof(Plane) * faceNum);
		vectoplane(Fvertex[400]);
		planecolorset(Fvertex[400], 0);



		glEnable(GL_DEPTH_TEST);

	} // 초기화할 데이터


	if (fps == false) {
		glViewport(-50, 50, 700, 700);
	}
	else {
		glViewport(0, 0, 800, 800);
	}
	glClearColor(1.0, 0.498, 0.314, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	//배경

	glUseProgram(shaderID);
	glBindVertexArray(VAO);// 쉐이더 , 버퍼 배열 사용

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	unsigned int color = glGetUniformLocation(shaderID, "outColor");
	unsigned int modelLocation = glGetUniformLocation(shaderID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderID, "view");
	unsigned int projLocation = glGetUniformLocation(shaderID, "projection");

	glm::mat4 Vw = glm::mat4(1.0f);
	glm::mat4 Cp = glm::mat4(1.0f);

	glm::vec3 cameraPos = glm::vec4(0.0f, 2.0, 2.0, 1.0f) * Cp;
	glm::vec3 cameraDirection = glm::vec4(0.0f, -1.0, -1.0, 1.0f) * Cp;
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

	Vw = glm::rotate(Vw, (float)glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
	Vw = glm::rotate(Vw, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

	glm::mat4 Pj = glm::mat4(1.0f);

	if (mode == false) {
		Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.1f, 100.0f);
	}
	else {
		Pj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.00f);
	}

	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

	if (fps == true) {																											// 1인칭 모드
		glm::mat4 Vw = glm::mat4(1.0f);
		glm::mat4 Cp = glm::mat4(1.0f);

		Cp = glm::rotate(Cp, (float)glm::radians(fpsy + fpsy2), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 cameraPos = glm::vec4(mx, my, mz, 0.0f);
		glm::vec3 cameraDirection = glm::vec4(0.0, fpsup + fpsup2, -2.0, 0.0f) * Cp;
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

		glm::mat4 Pj = glm::mat4(1.0f);

		if (mode == false) {
			Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.0005f, 10.0f);
		}
		else {
			Pj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.00f);
		}
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);
	}

	// 그리기 코드

	for (int i = 0; i < 400; ++i) {
		if (((i % 20) < xnum) && ((i / 20) < ynum) && (mage[i / 20][i % 20] != 0)) {

			TR = glm::mat4(1.0f);
			modelLocation = glGetUniformLocation(shaderID, "model");
			TR = glm::translate(TR, glm::vec3(bx[i / 20][i % 20], -0.5 + (by[i / ynum][i % xnum] * 0.5), bz[i / 20][i % 20]));
			TR = glm::scale(TR, glm::vec3((float)1 / (float)(xnum), by[i / ynum][i % xnum], (float)1 / (float)(ynum)));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

			for (int j = 0; j < 12; ++j) {
				Fvertex[i][j].Bind();
				Fvertex[i][j].Draw();
			}

		}
	}																				// 벽 그리기

	if (fps == false) {
		TR = glm::mat4(1.0f);
		modelLocation = glGetUniformLocation(shaderID, "model");
		TR = glm::translate(TR, glm::vec3(mx, my, mz));
		TR = glm::rotate(TR, (float)glm::radians(turn), glm::vec3(0.0f, 1.0f, 0.0f));
		TR = glm::scale(TR, glm::vec3(0.03, 0.03, 0.03));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		for (int j = 0; j < 12; ++j) {
			Fvertex[400][j].Bind();
			Fvertex[400][j].Draw();
		}																				// 플레이어 그리기
	}



	glViewport(550, 550, 250, 250);

	Vw = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

	Vw = glm::mat4(1.0f);
	Cp = glm::mat4(1.0f);

	Cp = glm::rotate(Cp, (float)glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));

	cameraPos = glm::vec4(0.0, 1.0, 0.0, 1.0) * Cp;
	cameraDirection = glm::vec4(0.0, -1.0, 0.0, 1.0f) * Cp;
	cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

	Vw = glm::lookAt(cameraPos, cameraDirection, cameraUp);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

	Pj = glm::mat4(1.0f);
	Pj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.00f);

	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);

	// 그리기 코드
	if (fps == false) {
		for (int i = 0; i < 400; ++i) {
			if (((i % 20) < xnum) && ((i / 20) < ynum) && (mage[i / 20][i % 20] != 0)) {

				TR = glm::mat4(1.0f);
				modelLocation = glGetUniformLocation(shaderID, "model");
				TR = glm::translate(TR, glm::vec3(bx[i / 20][i % 20], -0.5 + (by[i / ynum][i % xnum] * 0.5), bz[i / 20][i % 20]));
				TR = glm::scale(TR, glm::vec3((float)1 / (float)(xnum), by[i / ynum][i % xnum], (float)1 / (float)(ynum)));
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

				for (int j = 0; j < 12; ++j) {
					Fvertex[i][j].Bind();
					Fvertex[i][j].Draw();
				}

			}
		}																				// 벽 그리기

		if (fps == false) {
			TR = glm::mat4(1.0f);
			modelLocation = glGetUniformLocation(shaderID, "model");
			TR = glm::translate(TR, glm::vec3(mx, my, mz));
			TR = glm::rotate(TR, (float)glm::radians(turn), glm::vec3(0.0f, 1.0f, 0.0f));
			TR = glm::scale(TR, glm::vec3(0.03, 0.03, 0.03));
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

			for (int j = 0; j < 12; ++j) {
				Fvertex[400][j].Bind();
				Fvertex[400][j].Draw();
			}																				// 플레이어 그리기
		}
	}

	glutSwapBuffers();
	glutPostRedisplay();
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{

}

void Mouse(int button, int state, int x, int y)
{
	if (mousemode == false) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			if (fps == false) {
				ry = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 180;
			}
			else {
				mousex = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 90;
				mousey = -((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2) * 1.5;
			}
			Click = 1;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			fpsy += fpsy2;
			fpsup += fpsup2;
			fpsy2 = 0;
			fpsup2 = 0;
			Click = 0;
		}
	}

}

void Motion(int x, int y)
{
	if (mousemode == false) {
		if (Click == 1) {
			if (fps == false) {
				ry = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 180;
			}
			else {
				fpsy2 = -mousex + ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 90;
				fpsup2 = -mousey - ((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2) * 1.5;
			}
		}
	}

}

void Motion2(int x, int y)
{
	if (mousemode == true) {

		if (fps == false) {
			ry = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 180;
		}
		else {
			fpsy = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2) * 180;
			fpsup = -((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2) * 1.5;
		}

	}

}

void keyboard2(unsigned char key2, int x, int y) {
	key[key2] = false;
	glutPostRedisplay();
}

void keyboard(unsigned char key2, int x, int y) {
	key[key2] = true;
	switch (key2) {
	case 'p':
		if (mode == false) {
			mode = true;
		}
		else {
			mode = false;
		}
		break;
	case 'm':
		if (movey == false) {
			movey = true;
		}
		else {
			movey = false;
		}
		break;
	case 'b':
		if (mousemode == false) {
			mousemode = true;
		}
		else {
			mousemode = false;
		}
		break;
		/*case 't':
			if (red == false) {
				red = true;
			}
			else {
				red = false;
			}
			break;*/
	case '1':
		fps = true;
		break;
	case '3':
		fpsy = 0;
		fpsy2 = 0;
		fps = false;
		break;
	case 'v':
		movey = false;
		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				by[i][j] = 0.1;
			}
		}
		break;
	case '+':
		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				speed[i][j] += 0.01;
			}
		}
		break;
	case '-':
		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				speed[i][j] -= 0.01;
				if (speed[i][j] <= 0) {
					speed[i][j] = 0.001;
				}
			}
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	case 'y':
		if (rturn == 1) {
			rturn = 0;
		}
		else {
			rturn = 1;
		}
		break;
	case 'Y':
		if (rturn == 2) {
			rturn = 0;
		}
		else {
			rturn = 2;
		}
		break;
	case 'x':
		if (rturn2 == 1) {
			rturn2 = 0;
		}
		else {
			rturn2 = 1;
		}
		break;
	case 'X':
		if (rturn2 == 2) {
			rturn2 = 0;
		}
		else {
			rturn2 = 2;
		}
		break;

	case '.':

		mousex = 0;
		mousey = 0;

		mx = 0.0;
		my = -0.465;
		mz = 0.7;

		fpsy = 0;
		fpsy2 = 0;
		fpsup = 0;
		fpsup2 = 0;

		rx = 0;
		ry = 0;

		turn = 0;
		dir = 0;

		movey = false;

		fps = false;
		mode = false;
		mousemode = false;

		rturn = 0;
		rturn2 = 0;
		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				movecheck[i][j] = false;
				mage[i][j] = 1;
			}
		}
		break;
	case '/':
		mousex = 0;
		mousey = 0;

		mx = 0.0;
		my = -0.465;
		mz = 0.7;

		fpsy = 0;
		fpsy2 = 0;
		fpsup = 0;
		fpsup2 = 0;

		rx = 0;
		ry = 0;

		turn = 0;
		dir = 0;

		movey = false;

		fps = false;
		mode = false;
		mousemode = false;

		rturn = 0;
		rturn2 = 0;
		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				movecheck[i][j] = false;
				mage[i][j] = 1;
				by[i][j] = 0.5;
			}
		}
		cout << "x축을 몇개로 나눌까요? ( 2 ~ 20 )" << endl;
		cin >> xnum;
		cout << "y축을 몇개로 나눌까요? ( 2 ~ 20 )" << endl;
		cin >> ynum;
		if (xnum % 2 == 0) {
			xnum2 = xnum - 1;
		}
		else {
			xnum2 = xnum;
		}
		if (ynum % 2 == 0) {
			ynum2 = ynum - 1;
		}
		else {
			ynum2 = ynum;
		}
		break;
	case 'r':

		// 길을 다 막아버리는 작업
		for (int i = 0; i < ynum2; i++)
		{
			for (int j = 0; j < xnum2; j++)
			{
				if (j % 2 == 0 || i % 2 == 0)
					mage[i][j] = 1;
				else
					mage[i][j] = 0;
			}
		}

		// 길을 반반 확률로 뚫는 작업
		for (int i = 0; i < ynum2; i++)
		{
			int count = 1;
			for (int j = 0; j < xnum2; j++)
			{
				if (j % 2 == 0 || i % 2 == 0)
					continue;

				if (j == xnum2 - 2 && i == ynum2 - 2)
					continue;

				if (i == ynum2 - 2)
				{
					mage[i][j + 1] = 0;
					continue;
				}

				if (j == xnum2 - 2)
				{
					mage[i + 1][j] = 0;
					continue;
				}

				if (rand() % 2 == 0)
				{
					mage[i][j + 1] = 0;
					count++;
				}
				else
				{
					mage[i + 1][j - (rand() % count) * 2] = 0;
					count = 1;
				}
			}
		}

		for (int i = 0; i < ynum2; ++i) {
			mage[i][0] = 1;
			mage[i][xnum2 - 1] = 1;
		}
		for (int j = 0; j < xnum2; ++j) {
			mage[0][j] = 1;
			mage[ynum2 - 1][j] = 1;
		}

		mage[0][1] = 0;
		mage[ynum - 1][xnum2 - 2] = 0;
		mage[ynum2 - 1][xnum2 - 2] = 0;



		break;
	}


	glutPostRedisplay();
}

void TimerFunction(int value) {
	if (Time) {

		if (key['w'] == true) {

			mx += sin((float)glm::radians(fpsy + fpsy2)) * 0.008;
			mz -= cos((float)glm::radians(fpsy + fpsy2)) * 0.008;

			dir = 3;
		}

		if (key['s'] == true) {

			mx -= sin((float)glm::radians(fpsy + fpsy2)) * 0.008;
			mz += cos((float)glm::radians(fpsy + fpsy2)) * 0.008;

			dir = 0;
		}

		if (key['a'] == true) {

			mx -= cos((float)glm::radians(fpsy + fpsy2)) * 0.008;
			mz -= sin((float)glm::radians(fpsy + fpsy2)) * 0.008;

			dir = 2;
		}

		if (key['d'] == true) {

			mx += cos((float)glm::radians(fpsy + fpsy2)) * 0.008;
			mz += sin((float)glm::radians(fpsy + fpsy2)) * 0.008;

			dir = 1;
		}

		if (key['c'] == true) {
			jump = true;
		}


		for (int i = 0; i < 400; ++i) {
			if (((i % 20) < xnum) && ((i / 20) < ynum)) {

				if ((mx - 0.015 < bx[i / 20][i % 20] - ((float)0.5 / (float)(xnum))) && (mz - 0.013 < (float)bz[i / 20][i % 20] + ((float)0.5 / (float)(ynum))) &&
					(mx + 0.015 > (float)bx[i / 20][i % 20] - ((float)0.5 / (float)(xnum))) && (mz + 0.015 > (float)bz[i / 20][i % 20] - ((float)0.5 / (float)(ynum))))
				{
					mx = bx[i / 20][i % 20] - ((float)0.5 / (float)(xnum)) - 0.024;						// 왼쪽
				}

				if ((mx - 0.015 < bx[i / 20][i % 20] + ((float)0.5 / (float)(xnum))) && (mz - 0.015 < (float)bz[i / 20][i % 20] + ((float)0.5 / (float)(ynum))) &&
					(mx + 0.015 > (float)bx[i / 20][i % 20] + ((float)0.5 / (float)(xnum))) && (mz + 0.015 > (float)bz[i / 20][i % 20] - ((float)0.5 / (float)(ynum))))
				{
					mx = bx[i / 20][i % 20] + ((float)0.5 / (float)(xnum)) + 0.024;						// 오른쪽
				}

				if ((mx - 0.015 < bx[i / 20][i % 20] + ((float)0.5 / (float)(xnum))) && (mz - 0.018 < (float)bz[i / 20][i % 20] + ((float)0.5 / (float)(ynum))) &&
					(mx + 0.015 > (float)bx[i / 20][i % 20] - ((float)0.5 / (float)(xnum))) && (mz + 0.015 > (float)bz[i / 20][i % 20] + ((float)0.5 / (float)(ynum))))
				{
					mz = bz[i / 20][i % 20] + ((float)0.5 / (float)(ynum)) + 0.027;						// 위
				}

				if ((mx - 0.015 < bx[i / 20][i % 20] + ((float)0.5 / (float)(xnum))) && (mz - 0.018 < (float)bz[i / 20][i % 20] - ((float)0.5 / (float)(ynum))) &&
					(mx + 0.015 > (float)bx[i / 20][i % 20] - ((float)0.5 / (float)(xnum))) && (mz + 0.015 > (float)bz[i / 20][i % 20] - ((float)0.5 / (float)(ynum))))
				{
					mz = bz[i / 20][i % 20] - ((float)0.5 / (float)(ynum)) - 0.024;						// 아래
				}

			}
		}

		if (movey == true) {
			for (int i = 0; i < 20; ++i) {
				for (int j = 0; j < 20; ++j) {

					if (movecheck[i][j] == false) {
						by[i][j] += speed[i][j];
						if (by[i][j] >= maxy[i][j]) {
							movecheck[i][j] = true;
						}
					}

					else if (movecheck[i][j] == true) {
						by[i][j] -= speed[i][j];
						if (by[i][j] <= miny[i][j]) {
							movecheck[i][j] = false;
						}
					}

				}
			}
		}

		if (rturn == 1) {
			ry += 1;
		}
		else if (rturn == 2) {
			ry -= 1;
		}

		if (rturn2 == 1) {
			rx += 1;
		}
		else if (rturn2 == 2) {
			rx -= 1;
		}

		if (jump == true) {
			my += 0.005;

			if (my > -0.35) {
				jump = false;
			}
		}

		else {
			my -= 0.005;
			if (my < -0.485) {
				my = -0.485;
			}
		}

		dev = -0.5;

		for (int i = 0; i < 20; ++i) {
			dev += (float)1 / (float)(xnum);
			for (int j = 0; j < 20; ++j) {
				bx[j][i] = dev;
			}
		}

		dev = -0.5;

		for (int i = 0; i < 20; ++i) {
			dev += (float)1 / (float)(ynum);
			for (int j = 0; j < 20; ++j) {
				bz[i][j] = dev;
			}
		}

		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				if (mage[i][j] == 0) {
					bx[i][j] = 10;
					bz[i][j] = 10;
				}

			}
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
	if (a == 0) {
		for (int i = 0; i < faceNum; i += 2) {
			float R = 0.7 + ((rand() % 4) * 0.1);
			float G = (1 + ((rand() % 10) * 0.01)) * R;
			float B = (rand() % 11) * 0.1;
			for (int j = 0; j < 3; ++j) {
				p[i].color[j * 3] = R;
				p[i].color[j * 3 + 1] = G;
				p[i].color[j * 3 + 2] = B;

				p[i + 1].color[j * 3] = R;
				p[i + 1].color[j * 3 + 1] = G;
				p[i + 1].color[j * 3 + 2] = B;
			}
		}
	}
}






