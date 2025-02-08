/**************************
 CTIS164 - HOMEWORK 2
-----------------------------------
NAME SURNAME : AHMET CANPOLAT
STUDENT ID :  22102246
SECTION : 2
HOMEWORK : 2
-----------------------------------
ADDITIONAL FEATURES :
	-
 1- I added the number of birds shot to the code
 2- I added the number of birds that crossed the screen in 20 seconds to the code.
 3- I added last hit point to the code and adjusted the scoreboard accordingly.
*************************/
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
#define TIMER_PERIOD   20 // Zaman deðiþkeni için
#define TIMER_ON        1 // 0:devre dýþý, 1:uygun
#define D2R 0.01745329252
#define PI  3.14159265358

#define MAX_PEBBLE 1 // maximum çakýl taþý
#define TARGET_RADIUS 21
#define MAX_BIRDS 5

#define WEAPON_SIZE 150
#define PEBBLE_SIZE 8

int winWidth, winHeight; // en ve boy
bool spacebar = false;
bool F1 = false;
bool gameOver = false;
bool paused = false;
int score = 0; // skore sayacý
int lastPoint = 0; // son vuruþ sayacý
int timer = 20 * 1000; // 20 saniye
int noOfbbirds = 0; // ekrandaki kuþ sayýsý
int total_score = 0;
int birds_on_screen = 0; // Counter for birds currently on the screen

// Common structures
typedef struct {
	float x, y;
} point_t;

typedef struct {
	point_t pos;
	float speed;
} bird_t;

typedef struct {
	point_t pos;
	float angle;
	float size;
	bool active;
} pebble_t;

typedef struct {
	point_t pos;
	float speed;
	float size;
	float r, g, b;
} weapon_t;

bird_t birds[MAX_BIRDS];
pebble_t pebble[MAX_PEBBLE];
weapon_t weapon = { {-275, 0}, 5, WEAPON_SIZE, 1, 0, 0 }; // silahýn ilk konumu, hýzý, büyüklüðü, rengi

// genel daire çizimi için
void circle(int x, int y, int r)
{
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

// yazdýrma konumu
void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, string[i]);
}

//çizgi ayarlama
void line(point_t p1, point_t p2)
{
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
}

// üçgen çizmek için
void triangle(point_t p1, point_t p2, point_t p3)
{
	glBegin(GL_TRIANGLES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glVertex2f(p3.x, p3.y);
	glEnd();
}


void drawBird(bird_t bird)
{
	glColor3f(1.0f, 1.0f, 1.0f); // kuþ rengi
	point_t head = { bird.pos.x, bird.pos.y };
	point_t body_top = { bird.pos.x - 15, bird.pos.y - 15 };
	point_t body_bottom = { bird.pos.x + 15, bird.pos.y - 15 };
	point_t left_wing_top = { bird.pos.x - 15, bird.pos.y - 7 };
	point_t left_wing_bottom = { bird.pos.x - 30, bird.pos.y - 28 };
	point_t right_wing_top = { bird.pos.x + 15, bird.pos.y - 7 };
	point_t right_wing_bottom = { bird.pos.x + 30, bird.pos.y - 28 };
	point_t tail_top = { bird.pos.x + 15, bird.pos.y };
	point_t tail_bottom = { bird.pos.x + 30, bird.pos.y + 7 };

	// kuþ kafasý
	circle(head.x, head.y, 10);

	// kuþun body
	line(body_top, body_bottom);

	// wings
	triangle(left_wing_top, left_wing_bottom, body_top);
	triangle(right_wing_top, right_wing_bottom, body_top);

	// tail of bird
	line(tail_top, tail_bottom);
}

void drawGradient(int x1, int y1, int w, int h, float r, float g, float b)
{
	glBegin(GL_QUADS);
	glColor3f(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glColor3f(r + 0.4, g + 0.4, b + 0.4);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();

	glColor3f(0.1, 0.1, 0.1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x1 + w, y1);
	glVertex2f(x1 + w, y1 - h);
	glVertex2f(x1, y1 - h);
	glEnd();
}

void drawCloud(int x, int y)
{
	glColor3f(0.8, 0.8, 0.8); // Yaðmur bulutu
	circle(0 + x, 0 + y, 30);
	circle(-25 + x, y, 20);
	circle(25 + x, -2 + y, 20);
	circle(21 + x, -19 + y, 10);
}

void drawBackground()
{
	drawGradient(-400, 300, 800, 600, 0.2, 0.2, 0.2); // Fýrtýnalý hava
	drawGradient(-400, -200, 800, 100, 0.5, 0.7, 1);  // Lake

	// Info for users to understand the rules of mini game
	glColor3f(0.97, 0.88, 0.75); // krem
	glRectf(-WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2 + 20); // 10 units thick
	glColor3f(0.5, 0, 0.5); // MOr
	print(-297, -290, "SpaceBar: Fire  Up/Down: To move weapon  F1: Pause/Restart", GLUT_BITMAP_HELVETICA_12);
	// Draw 3 clouds
	drawCloud(-250, 180);
	drawCloud(250, 100);
	drawCloud(0, 200);
}

void drawPebble(pebble_t pebble) {
	// Taþ oluþturmak için daire 
	glColor3f(1, 1, 0); //
	circle(pebble.pos.x, pebble.pos.y, 5);

	//  ateþ sembolü 
	glColor3f(1, 0, 0); // Kýrmýzý
	glBegin(GL_TRIANGLES);
	// Üçgenin tepe noktasý
	glVertex2f(pebble.pos.x, pebble.pos.y + 10);
	// Diðer iki köþe
	glVertex2f(pebble.pos.x - 5, pebble.pos.y);
	glVertex2f(pebble.pos.x + 5, pebble.pos.y);
	glEnd();
}

void drawFish(float x, float y)
{
	// Balýðýn gövdesi 
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.8f, 0.8f); // Açýk pembe renk
	for (int i = 0; i < 360; i += 10) {
		float angle = i * PI / 180;
		float fishX = x + 25 * cos(angle);
		float fishY = y + 15 * sin(angle);
		glVertex2f(fishX, fishY);
	}
	glEnd();

	// Balýðýn kuyruðu gövdesi ve kuyruðu
	glBegin(GL_POLYGON);
	glColor3f(0.6f, 0.8f, 0.8f); // Açýk pembe renk
	for (int i = 90; i < 270; i += 10) {
		float angle = i * PI / 180;
		float fishX = x + 30 * cos(angle);
		float fishY = y + 15 * sin(angle);
		glVertex2f(fishX, fishY);
	}
	glEnd();

	// Balýðýn baþý 
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.8f, 0.8f); // Açýk pembe renk
	for (int i = 270; i < 450; i += 10) {
		float angle = i * PI / 180;
		float fishX = x + 20 * cos(angle);
		float fishY = y + 15 * sin(angle);
		glVertex2f(fishX, fishY);
	}
	glEnd();

	// Gözler 
	glColor3f(0.0f, 0.0f, 0.0f); // Siyah 
	glPointSize(5.0f); // Göz çapý
	glBegin(GL_POINTS);
	glVertex2f(x + 10, y + 5); // Sað göz
	glVertex2f(x + 20, y + 5); // Sol göz
	glEnd();
}

void drawWeapon(weapon_t weapon)
{
	// sapanýn sapaný dikdörtgen
	glColor3f(0.4, 0.2, 0.0); // dark brown 
	glRectf(weapon.pos.x - WEAPON_SIZE / 20, weapon.pos.y - WEAPON_SIZE / 12,
		weapon.pos.x + WEAPON_SIZE / 12, weapon.pos.y + WEAPON_SIZE / 12);

	// sapanýn diðer ögeleri
	glColor3f(0.9, 0.9, 0.9); // Gri renk
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 12, weapon.pos.y - WEAPON_SIZE / 12);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 4, weapon.pos.y);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 12, weapon.pos.y + WEAPON_SIZE / 12);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 4, weapon.pos.y);
	glEnd();

	// Silahýn namlusunu çizelim üçgen
	glColor3f(0.6, 0.4, 0.2); // Açýk ceviz
	glBegin(GL_TRIANGLES);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 4, weapon.pos.y);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 2, weapon.pos.y + WEAPON_SIZE / 24);
	glVertex2f(weapon.pos.x + WEAPON_SIZE / 2, weapon.pos.y - WEAPON_SIZE / 24);
	glEnd();
}




void drawScore(int score)
{
	// Score display area
	glColor3f(0.98, 0.88, 0.75); // krem
	glRectf(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2, (WINDOW_HEIGHT / 2) - 33); // Rectangle at the top of the window
	glColor3f(0, 1, 0); // Fosfor yeþili
	char arrayofscore[500];
	sprintf(arrayofscore, "Score: %d", total_score);
	glRasterPos2f(-WINDOW_WIDTH / 2 + 120, WINDOW_HEIGHT / 2 - 20);
	int len = strlen(arrayofscore);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, arrayofscore[i]);
	}
}

void drawLastHitPoint(int last_hit_point)
{
	glColor3f(1, 0, 0); // red
	char arrayofLastPoint[500];
	sprintf(arrayofLastPoint, "Last Point: %d", last_hit_point);
	glRasterPos2f(-WINDOW_WIDTH / 2 + 220, WINDOW_HEIGHT / 2 - 20);
	int len = strlen(arrayofLastPoint);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, arrayofLastPoint[i]);
	}
}

void resetBird(bird_t *bird)
{
	bird->pos.x = rand() % 281; // random x position between 0 and 280
	bird->pos.y = WINDOW_HEIGHT / 2 + rand() % (WINDOW_HEIGHT / 2); // random y position between 
	bird->speed = 2.5 + (rand() % 6) * 0.5; // random speed between 2.5 and 6
}

// Initialization
void init()
{
	srand(time(NULL));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < MAX_BIRDS; i++) {
		resetBird(&birds[i]);
	}
}

// Timer function
void onTimer(int v)
{
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	if (F1)
	{
		if (gameOver)
		{
			gameOver = false;
			score = 0;
			timer = 20 * 1000;
			noOfbbirds = MAX_BIRDS;
			total_score = 0;
			init();
		}
		else
		{
			paused = !paused;
		}
		F1 = false;
	}

	if (!paused && !gameOver)
	{
		timer -= TIMER_PERIOD;
		if (timer <= 0)
		{
			gameOver = true;
			timer = 0;
		}

		if (spacebar)
		{
			int nextPebble = -1;
			for (int i = 0; i < MAX_PEBBLE; i++)
			{
				if (!pebble[i].active)
				{
					nextPebble = i;
					break;
				}
			}
			if (nextPebble != -1)
			{
				pebble[nextPebble].pos.x = weapon.pos.x; // Weapon's x position
				pebble[nextPebble].pos.y = weapon.pos.y; // Weapon's y position
				pebble[nextPebble].active = true;
			}
		}

		for (int i = 0; i < MAX_PEBBLE; i++)
		{
			if (pebble[i].active)
			{
				pebble[i].pos.x += 10; // PEBBLE'S hareket yönü
				if (pebble[i].pos.x > WINDOW_WIDTH / 2)
				{
					pebble[i].active = false;
				}
			}
		}

		for (int i = 0; i < MAX_BIRDS; i++)
		{
			birds[i].pos.y -= birds[i].speed; // kuþun yukardan aþaðý gitmesi
			if (birds[i].pos.y < -WINDOW_HEIGHT / 2)
			{ // kuþ ekrandan çýktýðýnda
				birds[i].pos.y = WINDOW_HEIGHT / 2; // Reset its y position
				birds[i].pos.x = rand() % 251; // Reset its x position
				birds_on_screen--; // Decrement counter when bird exits the screen
			}
			else if (birds[i].pos.y > -600 / 2 && birds[i].pos.y < 600 / 2 && birds_on_screen < MAX_BIRDS)
			{
				birds_on_screen++; //ekrana girdiðinde kuþ sayýsýný arttýr.
				noOfbbirds++;
			}
		}

		// Puan hesaplama
		for (int i = 0; i < MAX_PEBBLE; i++)
		{
			if (pebble[i].active)
			{
				for (int j = 0; j < MAX_BIRDS; j++)
				{
					float distance = sqrt(pow(pebble[i].pos.x - birds[j].pos.x, 2) + pow(pebble[i].pos.y - birds[j].pos.y, 2));
					if (distance < TARGET_RADIUS)
					{
						// Kuþ Vurulduysa 
						birds[j].pos.x = -600; // Kuþu ekrandan çýkar
						birds[j].pos.y = 300;
						// Vurulma noktasýný  hesaplamak to find lastPoint
						if (distance < 13) {
							lastPoint = 5;
						}
						else if (distance < 15) {
							lastPoint = 4;
						}
						else if (distance < 17) {
							lastPoint = 3;
						}
						else if (distance < 20) {
							lastPoint = 2;
						}
						else {
							lastPoint = 1;
						}
						total_score += lastPoint; // Vurluþ yerine göre puan verilir
					}
				}
			}
		}

		score = total_score; // Toplam puaný son puan ile güncelle
	}

	glutPostRedisplay();
}


// Display function
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawBackground();

	// Draw birds
	for (int i = 0; i < MAX_BIRDS; i++)
	{
		drawBird(birds[i]);
	}

	// Draw pebbles
	for (int i = 0; i < MAX_PEBBLE; i++)
	{
		if (pebble[i].active)
			drawPebble(pebble[i]);
	}

	// Draw weapon
	drawWeapon(weapon);

	// Draw score
	drawScore(score);

	// Draw last point
	drawLastHitPoint(lastPoint);

	// Balýklar
	drawFish(5, -260);
	drawFish(100, -260);
	drawFish(210, -260);

	// Draw total birds passed
	glColor3f(0, 0, 0.75); // blue
	char arrayOfBirdNumber[50];
	sprintf(arrayOfBirdNumber, "Total Birds Passed: %d", noOfbbirds);
	glRasterPos2f(-WINDOW_WIDTH / 2 + 350, WINDOW_HEIGHT / 2 - 20);
	int lenTotalBirds = strlen(arrayOfBirdNumber);
	for (int i = 0; i < lenTotalBirds; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, arrayOfBirdNumber[i]);
	}

	// Draw timer
	char arrayOfTimer[50];
	sprintf(arrayOfTimer, "Time: %d", timer / 1000);
	glColor3f(1, 1, 1); // White
	glRasterPos2f(-WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 20);
	int len = strlen(arrayOfTimer);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, arrayOfTimer[i]);
	}

	// Draw game over message
	if (gameOver)
	{
		glColor3f(1, 1, 1); // White
		char gameOverStr[] = "Game Over! Press F1 to restart";
		glRasterPos2f(-90, 0);
		int len = strlen(gameOverStr);
		for (int i = 0; i < len; i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, gameOverStr[i]);
		}

		char scoreStr[20];
		sprintf(scoreStr, "Score: %d\n", total_score);
		glRasterPos2f(-55, -42);
		len = strlen(scoreStr);
		for (int i = 0; i < len; i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreStr[i]);
		}
	}

	glutSwapBuffers();
}



// Keyboard down function
void onKeyDown(unsigned char key, int x, int y)
{
	if (key == 32) // Spacebar
		spacebar = true;
}

// Keyboard up 
void onKeyUp(unsigned char key, int x, int y)
{
	if (key == 32) // Spacebar
		spacebar = false;
}


void onSpecialKeyDown(int key, int x, int y)
{
	if (key == GLUT_KEY_F1) // F1 key
		F1 = true;
	if (key == GLUT_KEY_UP && weapon.pos.y < WINDOW_HEIGHT / 2) // Up arrow 
		weapon.pos.y += weapon.speed;
	if (key == GLUT_KEY_DOWN && weapon.pos.y > -WINDOW_HEIGHT / 2) // Down arrow 
		weapon.pos.y -= weapon.speed;
}

// Resize 
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Hunting of the Pelican who went fishing");

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialFunc(onSpecialKeyDown);
	glutReshapeFunc(onResize);

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	glutMainLoop();
	return 0;
}
