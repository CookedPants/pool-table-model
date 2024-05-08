#include <GL/glut.h>
#include <stdio.h>
#include <cmath>

// The table dimensions and ball size
float tableWidth = 2.0;
float tableHeight = 4.0;
float tableDepth = 0.3;
float ballRadius = 0.08;
float holeRadius = 0.08; 
int numBalls = 16;

// Camera settings for the view
float cameraAngleX = -60.0; // balls kept appearing on the wrong side, so made it negative
float cameraHeight = 6.5;
bool is2D = false;

typedef struct 
{
        float x, y;        // Position
        float vx, vy;      // Velocity
        bool visible;      // Visibility
} Ball;

Ball balls[16]; 
bool animationActive = false;  

// Colors definitions for balls
GLfloat white[] = {1.0, 1.0, 1.0};
GLfloat solids[][3] =
{
        {1.0, 0.5, 0.0},   // 1, Yellow
        {0.0, 0.0, 1.0},   // 2, Blue
        {1.0, 0.0, 0.0},   // 3, Red
        {0.5, 0.0, 0.5},   // 4, Purple
        {1.0, 0.5, 0.0},   // 5, Orange
        {0.0, 0.2, 0.13},  // 6, Dark Green
        {0.5, 0.0, 0.0},   // 7, Maroon
        {0.0, 0.0, 0.0}    // 8, Black
};
GLfloat stripes[][3] =
{
        {1.0, 1.0, 0.0},   // 9, Yellow
        {0.0, 0.0, 1.0},   // 10, Blue
        {1.0, 0.0, 0.0},   // 11, Red
        {0.5, 0.0, 0.5},   // 12, Purple
        {1.0, 0.5, 0.0},   // 13, Orange
        {0.0, 0.2, 0.13},  // 14, Dark Green
        {0.5, 0.0, 0.0}    // 15, Maroon
};
// Lighting Setup
void setupLights() 
{
	glEnable(GL_LIGHTING);
    	glEnable(GL_LIGHT0);
    	glEnable(GL_LIGHT1);

    	// Primary light
    	GLfloat light0_position[] = {0.0, 5.0, 5.0, 1.0};
    	GLfloat light0_diffuse[] = {0.8, 0.8, 0.8, 1.0};  // Bright white light
    	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);

    	// Secondary ambient light for softer shadows
    	GLfloat light1_position[] = {-5.0, 5.0, 5.0, 1.0};
    	GLfloat light1_diffuse[] = {0.5, 0.5, 0.5, 1.0};  // Dimmer white light
    	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);

    	// General ambient light
    	GLfloat ambientLight[] = {0.2, 0.2, 0.2, 1.0};
    	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
}
// Material Properties for the balls
void setBallMaterialProperties() 
{
	GLfloat material_specular[] = {0.5, 0.5, 0.5, 1.0};
    	GLfloat material_shininess[] = {50.0};
    	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
}
// Functions to draw solid and striped balls
void drawSolidBall(float x, float y, int number, float radius)
{
        float heightOffset = 0.1;
        glColor3fv(solids[number - 1]);
        glPushMatrix();
        glTranslatef(x, y, heightOffset);
        setBallMaterialProperties();  // Set material properties for reflective light
	glutSolidSphere(radius, 20, 20);
        glPopMatrix();
}
void drawStripedBall(float x, float y, int number, float radius)
{
        int lats = 20;
        int longs = 20;
        float heightOffset = 0.1;
        int i, j;
        setBallMaterialProperties();  // Set material properties for reflective light

	for(i = 0; i <= lats; i++)
        {
                float lat0 = M_PI * (-0.5 + (float) (i - 1) / lats);
                float z0  = sin(lat0);
                float zr0 =  cos(lat0);

                float lat1 = M_PI * (-0.5 + (float) i / lats);
                float z1 = sin(lat1);
                float zr1 = cos(lat1);

                glBegin(GL_TRIANGLE_STRIP);
                for(j = 0; j <= longs; j++)
                {
                        float lng = 2 * M_PI * (float) (j - 1) / longs;
                        float x1 = cos(lng);
                        float y1 = sin(lng);

                        // Sets color depending on the latitude to create a stripe
                        if (i > lats/2 - 2 && i < lats/2 + 2)
                        {
                                glColor3fv(stripes[number - 9]); // Stripe color
                        }
                        else
                        {
                                glColor3fv(white); // Main color
                        }
                        glNormal3f(x1 * zr0, y1 * zr0, z0);
                        glVertex3f(x + x1 * zr0 * radius, y + y1 * zr0 * radius, heightOffset + z0 * radius);
                        glNormal3f(x1 * zr1, y1 * zr1, z1);
                        glVertex3f(x + x1 * zr1 * radius, y + y1 * zr1 * radius, heightOffset + z1 * radius);
                }
                glEnd();
        }
}
void drawPoolBalls()
{
    for (int i = 0; i < numBalls; ++i)
    {
        if (balls[i].visible)
        {
            if (i == 0) // Cue ball
            {
                glColor3fv(white);
                glPushMatrix();
                glTranslatef(balls[i].x, balls[i].y, 0.1);
                glutSolidSphere(ballRadius, 20, 20);
                glPopMatrix();
            }
            else if (i == 5 || (i >= 11 && i <= 15)) // Striped balls
            {
                drawStripedBall(balls[i].x, balls[i].y, i, ballRadius);
            }
            else // Solid balls
            {
                drawSolidBall(balls[i].x, balls[i].y, i, ballRadius);
            }
        }
    }
}
// Draws a quarter circle for pockets
void drawQuarterCircle(float radius, float depth)
{
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0, 0.0, depth);
        for (int i = 0; i <= 90; ++i)
        {
                float theta = i * (M_PI / 180.0);
                glVertex3f(radius * cos(theta), radius * sin(theta), depth);
        }
        glEnd();
}
// Draws half circle for middle pockets
void drawHalfCircle(float radius)
{
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0, 0.0, 0.0);  
        for (int i = 0; i <= 180; ++i)
        {
                float theta = i * (M_PI / 180.0);
                glVertex3f(radius * cos(theta), radius * sin(theta), 0.0);
        }
        glEnd();
}
// Draws the pockets
void drawPockets(float radius, float depth)
{
        glColor3f(0.0, 0.0, 0.0);
        struct Pocket
        {
                float x, y;  
                float angle; 
        };
        Pocket pockets[4] =
        {

                {-1.2, 1.08, 270},      // Top left corner, rotate 270 degrees
                {1.2, 1.08, 180},       // Top right corner, rotate 180 degrees
                {1.2, -1.08, 90},       // Bottom right corner, rotate 90 degrees
                {-1.2, -1.08, 0}        // Bottom left corner, rotate 0 degrees
        };
        for (const auto& pocket : pockets)
        {
                glPushMatrix();
                glTranslatef(pocket.x * (tableWidth / 2 - radius), pocket.y * (tableHeight / 2 - radius), 0);

                glRotatef(pocket.angle, 0.0, 0.0, 1.0);
                drawQuarterCircle(radius, 0.05);
                glPopMatrix();
        }
}
// Draws the middle pockets, had to split them up
void drawSidePockets(float radius)
{
        glColor3f(0.0, 0.0, 0.0); // Black color for pockets
        float xPositions[2] = {-(tableWidth / 2 - radius * -0.2), tableWidth / 2 - radius * -0.2};
        for (int i = 0; i < 2; ++i)
        {
                glPushMatrix();
                glTranslatef(xPositions[i], 0, 0.01);
                glRotatef(i == 0 ? 270 : 90, 0.0, 0.0, 1.0);
                drawHalfCircle(radius);
                glPopMatrix();
        }
}
// Draws the legs
void drawTableLeg()
{
        glColor3f(0.5, 0.35, 0.05);
        GLUquadric* quadric = gluNewQuadric();
        gluCylinder(quadric, 0.1, 0.1, tableHeight, 32, 32);
}
// Draws the table surface, with walls, all the pockets, and table legs
void drawTable()
{
        float cushionHeight = 0.2;
        float holeRadius = 0.15;
        float holeDepth = 0.2;

        glColor3f(0.0, 1.0, 0.0); // Green table color

        // Draws the table surface
        glBegin(GL_QUADS);
        glVertex3f(-tableWidth / 2, -tableHeight / 2, 0.0);
        glVertex3f(tableWidth / 2, -tableHeight / 2, 0.0);
        glVertex3f(tableWidth / 2, tableHeight / 2, 0.0);
        glVertex3f(-tableWidth / 2, tableHeight / 2, 0.0);
        glEnd();

        // Table sides with brown color
        glColor3f(0.3, 0.1, 0.0);
        glBegin(GL_QUADS);

        // Horizontal sides
        glBegin(GL_QUADS);
        glVertex3f(-tableWidth / 2, -tableHeight / 2, cushionHeight);
        glVertex3f(tableWidth / 2, -tableHeight / 2, cushionHeight);
        glVertex3f(tableWidth / 2, -tableHeight / 2, 0.0);
        glVertex3f(-tableWidth / 2, -tableHeight / 2, 0.0);

        glVertex3f(-tableWidth / 2, tableHeight / 2, cushionHeight);
        glVertex3f(tableWidth / 2, tableHeight / 2, cushionHeight);
        glVertex3f(tableWidth / 2, tableHeight / 2, 0.0);
        glVertex3f(-tableWidth / 2, tableHeight / 2, 0.0);
        glEnd();

        // Vertical sides
        glBegin(GL_QUADS);
        glVertex3f(-tableWidth / 2, -tableHeight / 2, cushionHeight);
        glVertex3f(-tableWidth / 2, tableHeight / 2, cushionHeight);
        glVertex3f(-tableWidth / 2, tableHeight / 2, 0.0);
        glVertex3f(-tableWidth / 2, -tableHeight / 2, 0.0);

        glVertex3f(tableWidth / 2, -tableHeight / 2, cushionHeight);
        glVertex3f(tableWidth / 2, tableHeight / 2, cushionHeight);
        glVertex3f(tableWidth / 2, tableHeight / 2, 0.0);
        glVertex3f(tableWidth / 2, -tableHeight / 2, 0.0);
        glEnd();

        // Draws all of the pockets
        drawPockets(holeRadius, holeDepth);
        drawSidePockets(0.15);

        // Draws the table legs
        glPushMatrix();
        glTranslatef(-tableWidth / 2 + 0.1, -tableHeight / 2 + 0.1, -tableHeight);
        drawTableLeg();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(tableWidth / 2 - 0.1, -tableHeight / 2 + 0.1, -tableHeight);
        drawTableLeg();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-tableWidth / 2 + 0.1, tableHeight / 2 - 0.1, -tableHeight);
        drawTableLeg();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(tableWidth / 2 - 0.1, tableHeight / 2 - 0.1, -tableHeight);
        drawTableLeg();
        glPopMatrix();
}
// Function to draw the cue stick
void drawCueStick(float x, float y, float angle, float length, GLfloat color1[3], GLfloat color2[3])
{
        glPushMatrix();
        glTranslatef(x, y, -1.3);     
        glRotatef(angle, 0, 0, 1);

        GLUquadric* quadric = gluNewQuadric();
        glColor3fv(color1);
        gluCylinder(quadric, 0.02, 0.02, length / 2, 32, 32);
        
        glTranslatef(0, 0, length / 2);
        glColor3fv(color2);
        gluCylinder(quadric, 0.02, 0.02, length / 2, 32, 32);
        glPopMatrix();
}
// Sets up the scene, allows for a change of perspective
void drawScene()
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (is2D)
        {
                glOrtho(-tableWidth / 2, tableWidth / 2, -tableHeight / 2, tableHeight / 2, -10, 10);
        }
        else
        {
                gluPerspective(45, 1, 0.1, 100);
                glTranslatef(0, 0, -cameraHeight);
                glRotatef(cameraAngleX, 1, 0, 0);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        drawTable();
        drawPoolBalls();
        GLfloat white[] = {1.0, 1.0, 1.0};
        GLfloat black[] = {0.0, 0.0, 0.0};
        drawCueStick(-1.0, 2.0, 45, tableHeight, white, black);
        drawCueStick(1.0, -2.0, -45, tableHeight, white, black);
        
        glutSwapBuffers();
}
// Based on real life setup
void initBalls()
{
	// Ball positioning constants
    	float startX = 0.0;
    	float startY = 0.0;
    	float spacingX = 2 * ballRadius;  
    	float spacingY = sqrt(3) * ballRadius;
	// For Triangle Shape pattern
    	// Row 1
    	balls[1] = (Ball){startX, startY, 0.0, 0.0, true}; // Yellow solid

    	// Row 2
    	balls[2] = (Ball){startX - spacingX / 2, startY + spacingY, 0.0, 0.0, true}; // Blue solid
    	balls[3] = (Ball){startX + spacingX / 2, startY + spacingY, 0.0, 0.0, true}; // Stripe brown

    	// Row 3
    	balls[4] = (Ball){startX - spacingX, startY + 2 * spacingY, 0.0, 0.0, true}; // Orange solid
    	balls[5] = (Ball){startX, startY + 2 * spacingY, 0.0, 0.0, true}; // Black ball
    	balls[6] = (Ball){startX + spacingX, startY + 2 * spacingY, 0.0, 0.0, true}; // Striped blue

    	// Row 4
    	balls[7] = (Ball){startX - 1.5 * spacingX, startY + 3 * spacingY, 0.0, 0.0, true}; // Solid purple
    	balls[8] = (Ball){startX - 0.5 * spacingX, startY + 3 * spacingY, 0.0, 0.0, true}; // Solid brown
    	balls[9] = (Ball){startX + 0.5 * spacingX, startY + 3 * spacingY, 0.0, 0.0, true}; // Stripe yellow
    	balls[10] = (Ball){startX + 1.5 * spacingX, startY + 3 * spacingY, 0.0, 0.0, true}; // Solid green

    	// Row 5
    	balls[11] = (Ball){startX - 2 * spacingX, startY + 4 * spacingY, 0.0, 0.0, true}; // Stripe Green
    	balls[12] = (Ball){startX - spacingX, startY + 4 * spacingY, 0.0, 0.0, true}; // Stripe Purple
    	balls[13] = (Ball){startX, startY + 4 * spacingY, 0.0, 0.0, true}; // Striped maroon
    	balls[14] = (Ball){startX + spacingX, startY + 4 * spacingY, 0.0, 0.0, true}; // Striped orange
    	balls[15] = (Ball){startX + 2 * spacingX, startY + 4 * spacingY, 0.0, 0.0, true}; // Solid maroon

    	// Cue ball setup
    	balls[0] = (Ball){0, startY - 4 * spacingY, 0.0, 0.0, true}; // Cue ball
}
// Based on slow downed video of shot, and an attempt to recreate the movemen
void setInitialVelocities() 
{
	for (int i = 0; i < numBalls; ++i) 
	{
        	balls[i].vx = 0;
        	balls[i].vy = 0;
    	}
    	balls[0].vy = 0.2;  
    	balls[1].vx = 0.05; balls[1].vy = -0.03; 
    	balls[13].vx = 0.01; balls[13].vy = -0.02; 
    	balls[9].vy = -0.05; 
    	balls[2].vx = 0.04; balls[2].vy = 0.03; 
    	balls[5].vx = 0.04; balls[5].vy = 0.03; 
    	balls[10].vx = -0.03; balls[10].vy = 0.04; 
    	balls[4].vx = -0.07; balls[4].vy = 0.07; 
    	balls[12].vx = 0.07; balls[12].vy = 0.07; 
    	balls[3].vx = -0.05; balls[3].vy = 0.01; 
    	balls[7].vx = 0.02; balls[7].vy = 0.04; 
    	balls[11].vx = 0.01; balls[11].vy = 0.03; 
}
// An attempt of making the balls move according to physics
void updateBallPositions()
{
    const float wallLeft = -tableWidth / 2 + ballRadius;
    const float wallRight = tableWidth / 2 - ballRadius;
    const float wallTop = tableHeight / 2 - ballRadius;
    const float wallBottom = -tableHeight / 2 + ballRadius;
    for (int i = 0; i < numBalls; i++)
    {
        if (balls[i].visible)
        {
            float nextX = balls[i].x + balls[i].vx;
            float nextY = balls[i].y + balls[i].vy;

            // Horizontal collision detection
            if (nextX < wallLeft)
            {
                balls[i].x = wallLeft; 
                balls[i].vx = 0; 
                if (i == 1 || i == 7) 
                {
                    balls[i].visible = false;
                }
            }
            else if (nextX > wallRight)
            {
                balls[i].x = wallRight; 
                balls[i].vx = 0; 
                if (i == 4 || i == 12) 
                {
                    balls[i].visible = false;
                }
            }
            else
            {
                balls[i].x = nextX; 
            }
            // Vertical collision detection
            if (nextY < wallBottom)
            {
                balls[i].y = wallBottom; 
                balls[i].vy = 0; 
                if (i == 10 || i == 14) 
                {
                    balls[i].visible = false;
                }
            }
            else if (nextY > wallTop)
            {
                balls[i].y = wallTop;
                balls[i].vy = 0; 
                if (i == 3 || i == 15) 
                {
                    balls[i].visible = false;
                }
            }
            else
            {
                balls[i].y = nextY; 
            }
        }
    }
}
void idleFunction() 
{
	if (animationActive) 
	{
        	updateBallPositions();
        	glutPostRedisplay();   // Ensures the display is refreshed
    	}
}
void keyboard(unsigned char key, int x, int y)
{
        switch (key)
        {
                case 'a':
                        cameraAngleX -= 5.0;
                        break;
                case 'd':
                        cameraAngleX += 5.0;
                        break;
                case 'w':
                        is2D = true;
                        break;
                case 's':
                        is2D = false;
                        break;
                case 27:
                        exit(0);
                        break;
		case ' ': // Starts animation
            		if (!animationActive) 
			{
				setInitialVelocities();
                		animationActive = true;
            		}
            		break;
        	case 'r': // Resets it
            		initBalls();
            		animationActive = false;
            		break;
	}
        glutPostRedisplay();
}
void init()
{
        glClearColor(0.2, 0.2, 0.2, 1.0); // Grey background color
        glEnable(GL_DEPTH_TEST);
	setupLights();  
	initBalls();
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}
int main(int argc, char** argv)
{
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(800, 600);
        glutIdleFunc(idleFunction);
	glutCreateWindow("3D/2D Pool Table");
        init();
        glutDisplayFunc(drawScene);
        glutKeyboardFunc(keyboard);
        glutMainLoop();
        return 0;
}
