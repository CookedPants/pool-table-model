#include <GL/glut.h>
#include <cmath>

// The table dimensions and ball size
float tableWidth = 2.0;
float tableHeight = 4.0;
float tableDepth = 0.3;
float ballRadius = 0.08;
int numBalls = 16;

// Camera settings for the view
float cameraAngleX = -60.0; // balls kept appearing on the wrong side, so made it negative
float cameraHeight = 6.5;
bool is2D = false;

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

// Functions to draw solid and striped balls
void drawSolidBall(float x, float y, int number, float radius)
{
        float heightOffset = 0.2;
        glColor3fv(solids[number - 1]);
        glPushMatrix();
        glTranslatef(x, y, heightOffset);
        glutSolidSphere(radius, 20, 20);
        glPopMatrix();
}
void drawStripedBall(float x, float y, int number, float radius)
{
        float heightOffset = 0.2;
        // Draw the main part of the ball in white also doesn't work
        glColor3fv(white);
        glPushMatrix();
        glTranslatef(x, y, heightOffset);
        glutSolidSphere(radius, 20, 20);
        glPopMatrix();

        // Draw the colored stripe doesn't fully work yet
        glColor3fv(stripes[number - 9]);
        glPushMatrix();
        glTranslatef(x, y, heightOffset);
        glScalef(1.0, 0.3, 1.0);
        glutSolidSphere(radius, 20, 20);
        glPopMatrix();
}
// Function to draw pool balls in the correct arrangement ???
void drawPoolBalls()
{
        float startX = 0.0;
        float startY = 0.0;
        float offsetX = 0.0;
        float offsetY = 0.0;
        float spacing = 2 * ballRadius * sqrt(3);

        // Draw the cue ball
        glColor3fv(white);
        glPushMatrix();
        glTranslatef(startX, startY - 3 * spacing, 0.2); // Place the cue ball in the front
        glutSolidSphere(ballRadius, 20, 20);
        glPopMatrix();

        int solidIndex = 0, stripeIndex = 0;
        for (int row = 0; row < 5; ++row)
        {

                for (int col = 0; col <= row; ++col)
                {

                        float x = startX + offsetX + col * spacing;
                        float y = startY + offsetY;

                        if (row == 2 && col == 1)
                        {
                                // Draw the 8-ball at th
                                drawSolidBall(x, y, 8, ballRadius);
                        }
                        else if ((row + col) % 2 == 0 && solidIndex < 7)
                        {
                                // Alternate stripes and solids except the 8-ball
                                drawSolidBall(x, y, solidIndex + 1, ballRadius);
                                solidIndex++;
                        }
                        else if (stripeIndex < 7)
                        {
                                drawStripedBall(x, y, stripeIndex + 9, ballRadius);
                                stripeIndex++;
                        }
                }
                offsetX -= spacing / 2;
                offsetY += spacing;
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
void drawPockets(float radius, float depth)
{
        glColor3f(0.0, 0.0, 0.0);
        // Define offsets and rotation for each corner
        struct Pocket
        {
                float x, y; // Position offsets
                float angle; // Rotation angle
        };
        // Pockets with correct positioning and rotations
        Pocket pockets[4] =
        {

                {-1, 1, 270},   // Top left corner, rotate 270 degrees
                {1, 1, 180},      // Top right corner, rotate 180 degrees
                {1, -1, 90},    // Bottom right corner, rotate 90 degrees
                {-1, -1, 0}   // Bottom left corner, rotate 0 degrees
        };
        for (const auto& pocket : pockets)
        {
                glPushMatrix();
                glTranslatef(pocket.x * (tableWidth / 2 - radius), pocket.y * (tableHeight / 2 - radius), 0);
                glRotatef(pocket.angle, 0.0, 0.0, 1.0);
                drawQuarterCircle(radius, depth);
                glPopMatrix();
        }
}
void drawTableLeg()
{
        glColor3f(0.5, 0.35, 0.05);
        GLUquadric* quadric = gluNewQuadric();
        gluCylinder(quadric, 0.1, 0.1, tableHeight, 32, 32);
}
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

        // Draws the pockets
        drawPockets(holeRadius, holeDepth);
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
        glutSwapBuffers();
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
        }
        glutPostRedisplay();
}
void init()
{
        glClearColor(1.0, 1.0, 1.0, 1.0); // White background color
        glEnable(GL_DEPTH_TEST);
}
int main(int argc, char** argv)
{
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(800, 600);
        glutCreateWindow("3D/2D Pool Table");
        init();
        glutDisplayFunc(drawScene);
        glutKeyboardFunc(keyboard);
        glutMainLoop();
        return 0;
}
