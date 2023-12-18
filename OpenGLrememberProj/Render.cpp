#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


//void setNormal(double*firstPoint, double*secondPoint, double*middle)
//{
//	double vectorA[] = { middle[0] - firstPoint[0], middle[1] - firstPoint[1], middle[2] - firstPoint[2] };
//	double vectorB[] = { middle[0] - secondPoint[0], middle[1] - secondPoint[1], middle[2] - secondPoint[2] };
//	double N, Nx, Ny, Nz;
//	Nx=
//	Ny=
//	Nz=
//	N = sqrt(Nx * Nx + Ny * Ny + Nz * Nz);
//
//
//	glNormal3d(Nx, Ny, Nz);
//}

void normal(double* point1, double* point2, double* point3, int k) {
	double a[3] = { point2[0] - point1[0],point2[1] - point1[1],point2[2] - point1[2] };
	double b[3] = { point3[0] - point1[0],point3[1] - point1[1],point3[2] - point1[2] };

	double normal[3] = { k*(a[1] * b[2] - b[1] * a[2]),k*(- a[0] * b[2] + b[0] * a[2]), k*(a[0] * b[1] - b[0] * a[1])};

	
	glNormal3d(normal[0], normal[1], normal[2]);
}

void DrawPrism()
{

		double a = 0;
		double b = 0.01;	// b'��� ��� ������ ����� �� ����, �.�. �������� �������� ����������, ����� �� ��� ���������, ��� ������ - ��� ������ ��������� => ���� �������� ����������
		double A[] = { 0, 0, 0 };
		double B[] = { -2, 0, 0 };
		double C[] = { -2, 7, 0 };
		double D[] = { 0,1,0 };
		double E[] = { 7, 3,0 };
		double F[] = { 5,-1,0 };
		double G[] = { 0,-1, 0 };
		double H[] = { -2, -7, 0 };
		double Z[] = { -6,-2, 0 };
		glPushMatrix();
		glTranslated(-3, -4, 0);
		
		glPopMatrix();
		glNormal3d(0, 0, -1);
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(H);
		glVertex3dv(Z);
		glVertex3dv(B);
		glEnd();



		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(B);
		glVertex3dv(C);
		glVertex3dv(D);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(D);
		glVertex3dv(E);
		glVertex3dv(F);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(F);
		glVertex3dv(G);
		glVertex3dv(D);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(D);
		glVertex3dv(B);
		glVertex3dv(G);
		glEnd();

		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(G);
		glVertex3dv(H);
		glVertex3dv(B);
		glEnd();

		double O[] = { 0,0,0 };
		glPushMatrix();
		glNormal3d(0, 0, -1);
		int numSegments = 100;
		double radius = 2.2;
		double centerX = 6.0;
		double centerY = 1.0;
		double startAngle = M_PI + (M_PI / 0.78); // 150 �������� (180 - 30)
		double endAngle = 2.0 * M_PI + (M_PI / 0.78); // ���� ��������� (360 ��������)
		double R = 1.145;
		double X, Y;
		glBegin(GL_TRIANGLE_FAN);
		double middle[] = { (E[0] + F[0]) / 2, (E[1] + F[1]) / 2, 0 };
		for (double t = -3.14159265358979323846 + R; t < 0 + R; t = t + 0.1) {

			X = middle[0] + radius * cos(t);
			Y = middle[1] + radius * sin(t);
			glVertex3d(X, Y, 0);
		}
		glEnd();

		glNormal3d(0, 0, 1);
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.7, 0.2, 0.2);;
		double X1, Y1;
		for (double t = -3.14159265358979323846 + R; t < 0 + R; t = t + 0.1) {

			X1 = middle[0] + radius * cos(t);
			Y1 = middle[1] + radius * sin(t);
			glVertex3d(X1, Y1, 10);
		}
		glEnd();
		glBegin(GL_QUAD_STRIP);
		glColor3d(0.7, 0.2, 0.2);
		double X2, Y2, Z2;
		int k = 0;
		double X22[] = { 0, 0, 0 };
		double Y22[] = { 0, 0, 10 };
		for (double t = -3.14159265358979323846 + R; t < 0 + R; t = t + 0.1) {
			k = 1;
			X2 = middle[0] + radius * cos(t);
			Y2 = middle[1] + radius * sin(t);
			glVertex3d(X2, Y2, 0);
			glVertex3d(X2, Y2, 10);

			
				double X22[] = { X2, Y2, 0 };
				double Y22[] = { X2, Y2, 10 };
			
			
			if(k % 2 == 0)
			{
				double Z22[] = { X2, Y2, 10 };
				normal(X22, Y22, Z22, -1);
			}
		}
		
		glEnd();
		glPopMatrix();
		double A1[] = { 0, 0, 10 };
		double B1[] = { -2, 0, 10 };
		double C1[] = { -2, 7, 10 };
		double D1[] = { 0,1, 10 };
		double E1[] = { 7, 3, 10 };
		double F1[] = { 5,-1, 10 };
		double G1[] = { 0,-1, 10 };
		double H1[] = { -2, -7, 10 };
		double Z1[] = { -6,-2,10 };
		glNormal3d(0, 0, 1);
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(H1);
		glVertex3dv(Z1);
		glVertex3dv(B1);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(B1);
		glVertex3dv(C1);
		glVertex3dv(D1);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(D1);
		glVertex3dv(E1);
		glVertex3dv(F1);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(F1);
		glVertex3dv(G1);
		glVertex3dv(D1);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(D1);
		glVertex3dv(B1);
		glVertex3dv(G1);
		glEnd();

		glBegin(GL_POLYGON);
		glColor3d(0.949, 0.941, 0.62);
		glVertex3dv(G1);
		glVertex3dv(H1);
		glVertex3dv(B1);
		glEnd();

		glBegin(GL_POLYGON);
		normal(B,C,C1, -1);
		glColor3d(0.949, 0.341, 0.62);
		glTexCoord2d(0, 0);
		glVertex3dv(B);
		glTexCoord2d(0, 1);
		glVertex3dv(C);
		glTexCoord2d(1, 1);
		glVertex3dv(C1);
		glTexCoord2d(1, 0);
		glVertex3dv(B1);
		glEnd();


		glBegin(GL_POLYGON);
		normal(C, D, D1, -1);
		glColor3d(0.349, 0.941, 0.62);
		glVertex3dv(C);
		glVertex3dv(D);
		glVertex3dv(D1);
		glVertex3dv(C1);
		glEnd();

		glBegin(GL_POLYGON);
		normal(D, E, E1, -1);
		glColor3d(0.349, 0.941, 0.62);
		glVertex3dv(D);
		glVertex3dv(E);
		glVertex3dv(E1);
		glVertex3dv(D1);
		glEnd();

		glBegin(GL_POLYGON);
		normal(E, F, F1, -1);
		glColor3d(0.949, 0.941, 0.32);
		glVertex3dv(E);
		glVertex3dv(F);
		glVertex3dv(F1);
		glVertex3dv(E1);
		glEnd();

		glBegin(GL_POLYGON);
		normal(F, G, G1, -1);
		glColor3d(0.349, 0.341, 0.62);
		glVertex3dv(F);
		glVertex3dv(G);
		glVertex3dv(G1);
		glVertex3dv(F1);
		glEnd();

		glBegin(GL_POLYGON);
		normal(G, H, H1, -1);
		glColor3d(0.349, 0.941, 0.32);
		glVertex3dv(G);
		glVertex3dv(H);
		glVertex3dv(H1);
		glVertex3dv(G1);
		glEnd();


		glBegin(GL_POLYGON);
		normal(H, Z, Z1, -1);
		glColor3d(0.3, 0.6, 0.3);
		glVertex3dv(H);
		glVertex3dv(Z);
		glVertex3dv(Z1);
		glVertex3dv(H1);
		glEnd();


		glBegin(GL_POLYGON);
		normal(Z, B, B1, -1);
		glColor3d(0.3, 0.6, 0.3);
		glVertex3dv(Z);
		glVertex3dv(B);
		glVertex3dv(B1);
		glVertex3dv(Z1);
		glEnd();
}




void Render(OpenGL *ogl)
{


	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������

	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);


	/*glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	glNormal3d(0, 0, 1);
	DrawPrism();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}