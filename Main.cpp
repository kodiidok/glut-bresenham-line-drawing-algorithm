
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <math.h>
#include <vector>

using namespace std;

int WINDOW_WIDTH = 720;
int WINDOW_HEIGHT = 480;

float sq_pos[] = { 0.0f, 0.0f };
float sq_rot = 0.0f;
float sq_scl = 1.0f;

const float pi = 3.14;

int X1, Y1, X2, Y2;

struct Point {
	float x, y;

	Point(float _x, float _y) : x(_x), y(_y) {}
};

vector<Point> pixels; // Vector to store pixel coordinates

void pixel(float u, float v) {
	// Add the pixel coordinates to the vector
	pixels.push_back(Point(u, v));
}

void calculateLineBresenham(int x1, int y1, int x2, int y2) {
	// Clear the pixel vector
	pixels.clear();

	// Calculate the differences
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	// Determine the sign of increments
	int signX = (x2 > x1) ? 1 : -1;
	int signY = (y2 > y1) ? 1 : -1;

	// Initialize decision parameters
	int decisionParameter = 2 * dy - dx;
	int x = x1, y = y1;

	// Add the starting point to the vector
	pixel(x, y);

	// Loop through the steps
	for (int i = 1; i <= dx; ++i) {
		if (decisionParameter > 0) {
			// Increment y and update decision parameter
			y += signY;
			decisionParameter += 2 * (dy - dx);
		}
		else {
			// Update decision parameter
			decisionParameter += 2 * dy;
		}

		// Increment x in each step
		x += signX;

		// Add the pixel coordinates to the vector
		pixel(x, y);
	}
}

void printPixels() {
	// Print the contents of the pixels vector
	cout << "Pixel Coordinates:\n";
	for (const auto& p : pixels) {
		cout << "(" << p.x << ", " << p.y << ")\n";
	}
}

void drawLineDDA() {
	glColor3f(1.0f, 1.0f, 1.0f);
	// Draw the pixels from the vector
	for (const auto& p : pixels) {
		glBegin(GL_QUADS);
		glVertex2f(p.x - 0.5f, p.y - 0.5f);
		glVertex2f(p.x + 0.5f, p.y - 0.5f);
		glVertex2f(p.x + 0.5f, p.y + 0.5f);
		glVertex2f(p.x - 0.5f, p.y + 0.5f);
		glEnd();
	}
}

void drawLine(int x1, int y1, int x2, int y2, float thickness) {
	// Set the line color
	glColor3f(0.0f, 1.0f, 0.0f);

	// Set the line thickness
	glLineWidth(thickness);

	// Draw the line
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();

	// Reset the line thickness to default
	glLineWidth(1.0f);
}

void square() {
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f); // green
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
}

void axes() {
	// x axis
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); // red
	glVertex2f(-10.0f, 0.0f);
	glVertex2f(10.0f, 0.0f);
	glEnd();

	// y axis
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); // blue
	glVertex2f(0.0f, 10.0f);
	glVertex2f(0.0f, -10.0f);
	glEnd();
}

void cell(int u, int v) {
	glBegin(GL_LINE_LOOP);
	glVertex2f(u - 0.5f, v - 0.5f);
	glVertex2f(u + 0.5f, v - 0.5f);
	glVertex2f(u + 0.5f, v + 0.5f);
	glVertex2f(u - 0.5f, v + 0.5f);
	glEnd();
}

void grid() {
	int u = 0, v = 0, m = 0, n = 0;

	glPushMatrix();
	glColor3f(0.3f, 0.3f, 0.3f);

	for (int i = 0; i < 10; i++) {

		u = 0, v = 0;

		for (int j = 0; j < 10; j++) {

			cell(u, m);

			if (v != u) {
				cell(v, m);
			}

			if (m != n) {
				cell(u, n);

				if (v != u) {
					cell(v, n);
				}
			}

			u++;
			v--;

		}

		m++;
		n--;

	}

	glPopMatrix();
}

void drawCartesianCoordinates() {
	// Set the line color
	glColor3f(0.3f, 0.3f, 0.3f);

	// Display tick marks on x-axis
	for (int i = -10; i <= 10; ++i) {
		glBegin(GL_LINES);
		glVertex2f(i, -0.2);
		glVertex2f(i, 0.2);
		glEnd();
	}

	// Display tick marks on y-axis
	for (int i = -10; i <= 10; ++i) {
		glBegin(GL_LINES);
		glVertex2f(-0.2, i);
		glVertex2f(0.2, i);
		glEnd();
	}

	// Display the axis labels
	glColor3f(0.5f, 0.5f, 0.5f);
	glRasterPos2f(10.2, 0.0);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'X');
	glRasterPos2f(0.0, 10.2);
	glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'Y');
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();

	axes();

	// LOCAL >> T -> R -> S
	// GLOABAL >> S -> R -> T

	grid();
	drawCartesianCoordinates();

	drawLineDDA();
	drawLine(X1, Y1, X2, Y2, 4);

	glFlush();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);

	GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;
	// w = aspect_ratio * h
	// h = w / aspect_ratio

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h) {
		gluOrtho2D(-11, 11, -11 / aspect_ratio, 11 / aspect_ratio);
	}
	else {
		gluOrtho2D(-11 * aspect_ratio, 11 * aspect_ratio, -11, 11);
	}
}

void keyboardSpecial(int key, int x, int y) {
	//printf("%d\n", key);

	if (key == GLUT_KEY_UP) {
		sq_scl += 0.5f;
	}
	if (key == GLUT_KEY_DOWN) {
		sq_scl -= 0.5f;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {

	// movement

	if (key == 'w') { // up
		sq_pos[1] += 0.5f;
	}
	if (key == 'a') { // left
		sq_pos[0] -= 0.5f;
	}
	if (key == 's') { // down
		sq_pos[1] -= 0.5f;
	}
	if (key == 'd') { // right
		sq_pos[0] += 0.5f;
	}

	glutPostRedisplay();

}

void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void getUserInput() {
	cout << "Enter Coordinates To Draw Line\nUsing Bresenham Line Drawing Algorithm:\n\nNOTE: (enter values from -10 to +10)\n\n";

	// Get user input for starting coordinates
	cout << "Enter x1: ";
	cin >> X1;
	cout << "Enter y1: ";
	cin >> Y1;

	// Get user input for ending coordinates
	cout << "Enter x2: ";
	cin >> X2;
	cout << "Enter y2: ";
	cin >> Y2;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(500, 300);
	glutCreateWindow("2D Graphics Template");

	getUserInput();

	calculateLineBresenham(X1, Y1, X2, Y2);
	printPixels();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutSpecialFunc(keyboardSpecial);
	glutKeyboardFunc(keyboard);

	init();
	glutMainLoop();

	return 0;
}