#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <time.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <cmath>

#define WINDOWX 800
#define WINDOWY 800
#define pie 3.141592

using namespace std;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<float> uid(-1.0, 1.0);
uniform_real_distribution<float> uidpos(0.01, 0.03);
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

GLuint VAO, VBO[3];

class Line {
public:
	GLfloat p[6];
	GLfloat color[6];

	void Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

	}
	void Draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}
};

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

float BackGround[] = { 0.0, 0.0, 0.0 };

Line point[6][1000];

glm::vec4* vertex;
glm::vec4* face;
glm::vec3* outColor;


glm::mat4 TR = glm::mat4(1.0f);
glm::mat4 TR2 = glm::mat4(1.0f);

GLUquadricObj* obj[7];
Plane* Figure[2];

FILE* FL;
int faceNum = 0;
int Click = 0;
float mx = 0, my = 0, mz = 0;
int sier_cnt = 0;

bool mode = true;
bool timeloop = false;
bool wire = true;
int turn = 0;
float turnY = 0;
int turn2 = 0;
float turnY2 = 0;

glm::vec3 lp = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 lc = glm::vec3(0.0, 0.0, 0.0);

glm::vec3 snow[100];
float snow_speed[100];

int now;
float theta[5];

float color[3];

void ReadObj(FILE* objFile);
void keyboard(unsigned char, int, int);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void vectoplane(Plane* p);
void planecolorset(Plane* p, int a);
void Sierpinski(glm::vec3, int, float);

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
		cerr << "ERROR: vertex shader ������ ����\n" << errorLog << endl;
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
		cerr << "ERROR: fragment shader ������ ����\n" << errorLog << endl;
		exit(-1);
	}

}
GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512];
	GLuint ShaderProgramID;
	ShaderProgramID = glCreateProgram(); //--- ���̴� ���α׷� �����
	glAttachShader(ShaderProgramID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(ShaderProgramID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(ShaderProgramID); //--- ���̴� ���α׷� ��ũ�ϱ�

	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		cerr << "ERROR: shader program ���� ����\n" << errorLog << endl;
		exit(-1);
	}
	glUseProgram(ShaderProgramID); //--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
	return ShaderProgramID;
}
void InitShader()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	shaderID = make_shaderProgram(); //--- ���̴� ���α׷� �����
}
GLvoid InitBuffer() {
	//--- VAO ��ü ���� �� ���ε�
	glGenVertexArrays(1, &VAO);
	//--- vertex data ������ ���� VBO ���� �� ���ε�.
	glGenBuffers(3, VBO);
}

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
{
	srand((unsigned int)time(NULL));
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(WINDOWX, WINDOWY); // �������� ũ�� ����
	glutCreateWindow("Example6");// ������ ����	(������ �̸�)
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(1, TimerFunction, 1);



	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	if (start) {
		start = FALSE;
		Time = TRUE; TimeLoop = TRUE;

		for (int i = 0; i < 3; ++i) {
			color[i] = 1.0;
		}
		for (int i = 0; i < 6; ++i) {
			for (int j = 0; j < 1000; ++j) {
				for (int k = 0; k < 6; ++k) {
					point[i][j].color[k] = 1.0;
				}
			}
		}

		for (int i = 0; i < 100; ++i) {
			snow[i] = glm::vec3(uid(dre), 1.0, uid(dre)); // ���� x, y,z
			snow_speed[i] = uidpos(dre);
		}


		for (int i = 0; i < 7; ++i) {
			obj[i] = gluNewQuadric(); // ��ü �����ϱ�
			gluQuadricNormals(obj[i], GLU_SMOOTH);
			gluQuadricOrientation(obj[i], GLU_OUTSIDE);
		}

		for (int i = 0; i < 3; ++i) {
			theta[i] = 0;
		}

		turn2 = 1;
		lp = glm::vec3(0.0, 0.4, 0.4);
		lc = glm::vec3(0.8, 0.8, 0.8);

		FL = fopen("cube.obj", "rt");
		ReadObj(FL);
		fclose(FL);
		Figure[0] = (Plane*)malloc(sizeof(Plane) * faceNum);
		vectoplane(Figure[0]);
		planecolorset(Figure[0], 0);

		FL = fopen("Pyramid.obj", "rt");
		ReadObj(FL);
		fclose(FL);
		Figure[1] = (Plane*)malloc(sizeof(Plane) * faceNum);
		vectoplane(Figure[1]);
		planecolorset(Figure[1], 1);



		glEnable(GL_DEPTH_TEST);

	} // �ʱ�ȭ�� ������
	InitShader();

	glClearColor(BackGround[0], BackGround[1], BackGround[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 	//���

	glUseProgram(shaderID);
	glBindVertexArray(VAO);// ���̴� , ���� �迭 ���

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wire == true) {
		for (int i = 0; i < 7; ++i) {
			gluQuadricDrawStyle(obj[i], GLU_FILL); // ���� ��Ÿ��
		}
	}
	else {
		for (int i = 0; i < 7; ++i) {
			gluQuadricDrawStyle(obj[i], GLU_FILL); // ���� ��Ÿ��
		}
	}

	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos"); //--- lightPos �� ����: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lp.x, lp.y, lp.z);
	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor"); //--- lightColor �� ����: (1.0, 1.0, 1.0) ���
	glUniform3f(lightColorLocation, lc.x, lc.y, lc.z);

	unsigned int color = glGetUniformLocation(shaderID, "outColor");
	unsigned int modelLocation = glGetUniformLocation(shaderID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderID, "view");
	unsigned int projLocation = glGetUniformLocation(shaderID, "projection");

	glm::vec3 C = glm::vec3(1.0f, 0.5f, 0.5f);

	C = glm::vec3(1.0f, 0.5f, 0.5f);
	glUniform3f(color, C[0], C[1], C[2]);

	glm::mat4 Vw = glm::mat4(1.0f);
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.5f, 1.5f);
	glm::vec3 cameraDirection = -cameraPos;
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	Vw = glm::translate(Vw, glm::vec3(0.0, 0.0, 2.0));

	Vw = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &Vw[0][0]);

	glm::mat4 Pj = glm::mat4(1.0f);

	Pj = glm::perspective(glm::radians(45.0f), (float)WINDOWX / (float)WINDOWY, 0.1f, 100.0f);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &Pj[0][0]);


	if (turn == 1) {
		turnY++;
	}
	else if (turn == 2) {
		turnY--;
	}
	if (turn2 == 1) {
		turnY2++;
	}
	else if (turn2 == 2) {
		turnY2--;
	}

	// �׸��� �ڵ�
	TR = glm::mat4(1.0f);
	TR = glm::rotate(TR, (float)glm::radians(turnY2), glm::vec3(0.0, 1.0, 0.0));
	TR2 = TR;
	TR2 = glm::translate(TR2, glm::vec3(-0.2, 0.0, 0.0));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
	C = glm::vec3(0.5f, 0.1f, 0.1f);
	glUniform3f(color, C[0], C[1], C[2]);
	gluSphere(obj[0], 0.03, 20, 20);

	TR = glm::mat4(1.0f);
	TR = glm::rotate(TR, (float)glm::radians(turnY2 * 2), glm::vec3(0.0, 0.0, 1.0));
	TR2 = TR;
	TR2 = glm::translate(TR2, glm::vec3(-0.4, 0.0, 0.0));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
	C = glm::vec3(0.1f, 0.4f, 0.1f);
	glUniform3f(color, C[0], C[1], C[2]);
	gluSphere(obj[1], 0.02, 20, 20);

	TR = glm::mat4(1.0f);
	TR = glm::rotate(TR, (float)glm::radians(turnY2 * 3), glm::vec3(1.0, 1.0, 0.0));
	TR2 = TR;
	TR2 = glm::translate(TR2, glm::vec3(0.0, 0.0, -0.3));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
	C = glm::vec3(0.1f, 0.1f, 0.4f);
	glUniform3f(color, C[0], C[1], C[2]);
	gluSphere(obj[2], 0.01, 20, 20);

	C = glm::vec3(0.7f, 0.7f, 0.7f);
	glUniform3f(color, C[0], C[1], C[2]);
	TR2 = glm::mat4(1.0f);
	TR2 = glm::translate(TR2, glm::vec3(0.0, -0.2, 0.0));
	TR2 = glm::scale(TR2, glm::vec3(1.0, 0.001, 1.0));
	for (int i = 0; i < 12; ++i) {
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
		Figure[0][i].Bind();
		Figure[0][i].Draw();
	}

	//����
	C = glm::vec3(1.0f, 1.0f, 1.0f);
	glUniform3f(color, C[0], C[1], C[2]);
	TR2 = glm::mat4(1.0f);
	TR2 = glm::translate(TR2, glm::vec3(lp.x, lp.y, lp.z));
	TR2 = glm::scale(TR2, glm::vec3(0.03, 0.03, 0.03));
	for (int i = 0; i < 12; ++i) {
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
		Figure[0][i].Bind();
		Figure[0][i].Draw();
	}


	//�þ���
	glm::vec3 pos = { 0.0, 0.0, 0.0 };
	C = glm::vec3(0.0f, 1.0f, 1.0f);
	glUniform3f(color, C[0], C[1], C[2]);
	Sierpinski(pos, sier_cnt, 0.3);

	C = glm::vec3(1.0f, 1.0f, 1.0f);
	glUniform3f(color, C[0], C[1], C[2]);
	if (theta[3] == 1) {
		for (int i = 0; i < 100; ++i) {
			C = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3f(color, C[0], C[1], C[2]);
			TR2 = glm::mat4(1.0f);
			TR2 = glm::translate(TR2, glm::vec3(snow[i].x, snow[i].y, snow[i].z));
			TR2 = glm::scale(TR2, glm::vec3(0.01, 0.01, 0.01));
			for (int i = 0; i < 12; ++i) {
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
				Figure[0][i].Bind();
				Figure[0][i].Draw();
			}
			snow[i].y -= snow_speed[i];

			if (snow[i].y < 0.0) {
				snow[i].y = 1.0;
			}
		}
	}


	glutSwapBuffers();
	glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mx = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2);
		my = -((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2);
		Click = 1;

	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		Click = 0;
	}
}

void Motion(int x, int y)
{
	if (Click == 1) {
		mx = ((float)x - ((float)WINDOWX / (float)2)) / ((float)WINDOWX / (float)2);
		my = -((float)y - ((float)WINDOWY / (float)2)) / ((float)WINDOWY / (float)2);

	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'i':
		lp.x *= 0.9; lp.y *= 0.9; lp.z *= 0.9;
		break;
	case 'o':
		lp.x *= 1.1; lp.y *= 1.1; lp.z *= 1.1;
		break;
	case 'r':
		theta[0] = 1;
		break;
	case 'R':
		theta[0] = 0;
		break;
	case '=':
		lc.x += 0.03; lc.y += 0.03; lc.z += 0.03;
		break;
	case '-':
		if (lc.x > 0.03) {
			lc.x -= 0.03; lc.y -= 0.03; lc.z -= 0.03;
		}
		break;
	case 'c':
		for (int i = 0; i < 3; ++i) {
			color[i] = uid(dre);
		}
		break;
	case 's':
		if (theta[3] == 0) {
			theta[3] = 1;
		}
		else {
			theta[3] = 0;
		}
		break;
	case '0':
		sier_cnt = 0;
		break;
	case '1':
		sier_cnt = 1;
		break;
	case '2':
		sier_cnt = 2;
		break;
	case '3':
		sier_cnt = 3;
		break;
	case '4':
		sier_cnt = 4;
		break;
	case '5':
		sier_cnt = 5;
		break;
	case '6':
		sier_cnt = 6;
		break;
	case '7':
		sier_cnt = 7;
		break;
	}

	glutPostRedisplay();
}

void TimerFunction(int value) {
	if (Time) {
		float r = sqrt(lp.x * lp.x + lp.z * lp.z);
		if (theta[0] == 1) {
			lp.x = r * sin(theta[1]);
			lp.z = r * cos(theta[1]);
			theta[1] += 0.03;
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
	//--- 1. ��ü ���ؽ� ���� �� �ﰢ�� ���� ����
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
			float R = (rand() % 11) * 0.1;
			float G = (rand() % 11) * 0.1;
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
	else if (a == 1) {
		float R = (rand() % 11) * 0.1;
		float G = (rand() % 11) * 0.1;
		float B = (rand() % 11) * 0.1;
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 3; ++j) {
				p[i].color[j * 3] = R;
				p[i].color[j * 3 + 1] = G;
				p[i].color[j * 3 + 2] = B;
			}
		}
		for (int i = 2; i < faceNum; ++i) {
			float R = (rand() % 11) * 0.1;
			float G = (rand() % 11) * 0.1;
			float B = (rand() % 11) * 0.1;
			for (int j = 0; j < 3; ++j) {
				p[i].color[j * 3] = R;
				p[i].color[j * 3 + 1] = G;
				p[i].color[j * 3 + 2] = B;
			}
		}
	}
}

void Sierpinski(glm::vec3 pos, int sier_cnt, float size)
{
	unsigned int modelLocation = glGetUniformLocation(shaderID, "model");
	float sier_size = size;



	if (sier_cnt == 0) {
		TR2 = glm::mat4(1.0f);
		TR2 = glm::translate(TR2, glm::vec3(pos.x, pos.y, pos.z));
		TR2 = glm::scale(TR2, glm::vec3(sier_size, sier_size, sier_size));
		for (int i = 0; i < 6; ++i) {
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2));
			Figure[1][i].Bind();
			Figure[1][i].Draw();
		}
	}
	else {
		int cnt = sier_cnt;
		--cnt;
		sier_size *= 0.5;
		glm::vec3 Pos[5];

		Pos[0].x = pos.x + sier_size * 0.5;
		Pos[0].y = pos.y - sier_size * 0.5;
		Pos[0].z = pos.z + sier_size * 0.5;

		Pos[1].x = pos.x - sier_size * 0.5;
		Pos[1].y = pos.y - sier_size * 0.5;
		Pos[1].z = pos.z + sier_size * 0.5;

		Pos[2].x = pos.x + sier_size * 0.5;
		Pos[2].y = pos.y - sier_size * 0.5;
		Pos[2].z = pos.z - sier_size * 0.5;

		Pos[3].x = pos.x - sier_size * 0.5;
		Pos[3].y = pos.y - sier_size * 0.5;
		Pos[3].z = pos.z - sier_size * 0.5;

		Pos[4].x = pos.x;
		Pos[4].y = pos.y + sier_size * 0.5;
		Pos[4].z = pos.z;


		for (int i = 0; i < 5; ++i) {
			Sierpinski(Pos[i], cnt, sier_size);
		}
	}
}