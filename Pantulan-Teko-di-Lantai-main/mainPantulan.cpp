/**************************
 * Includes
 * 
 **************************/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "glut.h"
#include "glaux.h"
#include <cmath>

/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

GLfloat	xrot;				// deklarasi rotasi x 
GLfloat	yrot;				// deklarasi rotasi y
GLfloat	zrot;				// deklarasi rotasi z

float A[3] = { -1.0,  1.0, -1.0 }; 	// Titik 1 (atas)
float B[3] = {  -1.0,  1.0,  1.0 }; // Titik 2 (atas)
float C[3] = {  1.0,  1.0,  1.0 };	// Titik 3 (atas)
float D[3] = { 1.0,  1.0, -1.0 };	// Titik 4 (atas)

float teapotZ = 25.0f; // initial Z position

float ctrlPoints[4][4][3];
GLUnurbsObj* theNurb;

GLuint	texture[2];	//gambar loader

void generateGridFromCorners() {
    for (int i = 0; i < 4; ++i) {
        float s = i / 3.0f;
        float AB[3], DC[3];
        for (int k = 0; k < 3; ++k) {
            AB[k] = A[k] * (1 - s) + B[k] * s;
            DC[k] = D[k] * (1 - s) + C[k] * s;
        }
        for (int j = 0; j < 4; ++j) {
            float t = j / 3.0f;
            for (int k = 0; k < 3; ++k) {
                ctrlPoints[i][j][k] = AB[k] * (1 - t) + DC[k] * t;
            }
            if ((i == 1 || i == 2) && (j == 1 || j == 2)) {
                ctrlPoints[i][j][1] += 1.0f;
            }
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
        case 'W':
            teapotZ -= 0.5f;
            printf("Teapot Z: %.2f\n", teapotZ);
            break;
        case 's':
        case 'S':
            teapotZ += 0.5f;
            printf("Teapot Z: %.2f\n", teapotZ);
            break;
    }
    glutPostRedisplay();
}

AUX_RGBImageRec *LoadBMP(char *Filename) {
    FILE *File = NULL;
    if (!Filename) {
        return NULL;
    }
    File = fopen(Filename, "r");
    if (File) {
        fclose(File);
        return auxDIBImageLoad(Filename);
    }
    return NULL;
}

int LoadGLTextures() {
    int Status = FALSE;
    AUX_RGBImageRec *TextureImage[2];
    memset(TextureImage, 0, sizeof(void *) * 2);
    if (TextureImage[0] = LoadBMP("GUAMBAR.bmp")) {
        if (TextureImage[1] = LoadBMP("MOMO.bmp")) {
            Status = TRUE;
            glGenTextures(2, &texture[0]);
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, texture[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }
    for (int i = 0; i < 2; i++) {
        if (TextureImage[i]) {
            if (TextureImage[i]->data)
                free(TextureImage[i]->data);
            free(TextureImage[i]);
        }
    }
    return Status;
}

void kubus() {
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f,  1.0f, -1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f, -1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f,  1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f);
        glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

void gambarDinding() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float repeatX = 10.0f;
    float repeatY = 5.0f;
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-10.0f, 0.0f, -10.0f);
        glTexCoord2f(repeatX, 0.0f);
        glVertex3f( 10.0f, 0.0f, -10.0f);
        glTexCoord2f(repeatX, repeatY);
        glVertex3f( 10.0f, 10.0f, -10.0f);
        glTexCoord2f(0.0f, repeatY);
        glVertex3f(-10.0f, 10.0f, -10.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void gambarDindingKiri() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float repeatY = 5.0f;
    float repeatZ = 10.0f;
    glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-10.0f, 0.0f, -10.0f);
        glTexCoord2f(0.0f, repeatY);
        glVertex3f(-10.0f, 10.0f, -10.0f);
        glTexCoord2f(repeatZ, repeatY);
        glVertex3f(-10.0f, 10.0f,  50.0f);
        glTexCoord2f(repeatZ, 0.0f);
        glVertex3f(-10.0f, 0.0f,  50.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void gambarDindingKanan() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float repeatY = 5.0f;
    float repeatZ = 10.0f;
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(10.0f, 0.0f, -10.0f);
        glTexCoord2f(0.0f, repeatY);
        glVertex3f(10.0f, 10.0f, -10.0f);
        glTexCoord2f(repeatZ, repeatY);
        glVertex3f(10.0f, 10.0f,  50.0f);
        glTexCoord2f(repeatZ, 0.0f);
        glVertex3f(10.0f, 0.0f,  50.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void gambarBox() {
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    float xMin = -2.5f, xMax = 2.5f;
    float yMin = 0.0f, yMax = 1.0f;
    float zMin = 8.0f, zMax = 12.0f;
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMin, yMax, zMax);
    glEnd();
    glEnable(GL_LIGHTING);
    glColor3f(0.7f, 0.5f, 0.2f);
    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMin, yMin, zMax);
        glNormal3f(0, 0, 1);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMin, yMax, zMax);
        glNormal3f(0, 0, -1);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMin);
        glNormal3f(-1, 0, 0);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMin, yMax, zMax);
        glVertex3f(xMin, yMax, zMin);
        glNormal3f(1, 0, 0);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMax, yMax, zMin);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

void gambarTV() {
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    float xMin = 8.0f, xMax = 10.0f;  // Positioned on the right side
    float yMin = 0.0f, yMax = 3.0f;   // Height of 3 units
    float zMin = 10.0f, zMax = 10.5f; // Thin depth
    // Screen (black)
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMin, yMax, zMax);
    glEnd();
    glEnable(GL_LIGHTING);
    // Frame (silver)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMin, yMin, zMax);
        glNormal3f(0, 0, -1);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMin);
        glNormal3f(-1, 0, 0);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMin, yMax, zMax);
        glVertex3f(xMin, yMax, zMin);
        glNormal3f(1, 0, 0);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMax, yMax, zMin);
        glNormal3f(0, 1, 0);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMin, yMax, zMax);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

void Inisialisasi(int lebar, int tinggi) {
    glViewport(0, 0, lebar, tinggi);
    glClearColor(0.941, 0.306, 0.545, 1.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    LoadGLTextures();
    generateGridFromCorners();
    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
    gluPerspective(45.0, (GLfloat)lebar/(GLfloat)tinggi, 1, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glPushMatrix();
    glColor3f(0.0, 1.0, 0.0);
    gluBeginSurface(theNurb);
    GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
    gluNurbsSurface(theNurb, 8, knots, 8, knots, 4 * 3, 3, &ctrlPoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(theNurb);
    kubus();
    glPopMatrix();
}

void gambarLantai() {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float repeatX = 10.0f;
    float repeatZ = 10.0f;
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-10.0f, 0.0f, 50.0f);
        glTexCoord2f(repeatX, 0.0f);
        glVertex3f(10.0f, 0.0f, 50.0f);
        glTexCoord2f(repeatX, repeatZ);
        glVertex3f(10.0f, 0.0f, -10.0f);
        glTexCoord2f(0.0f, repeatZ);
        glVertex3f(-10.0f, 0.0f, -10.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void kubusTekstur() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    GLfloat light_pos[] = { 0.0f, 3.0f, 5.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    GLfloat mat_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    GLfloat mat_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    GLfloat mat_shininess[] = { 51.2f };
    GLfloat intensitasCahaya[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat posisiCahaya[] = {2.0f, 2.0f, 2.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, intensitasCahaya);
    GLfloat bahan_ambient[] = {1.0f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_diffuse[] = {0.6f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_specular[] = {0.0f, 0.0f, 1.0f, 1.0f};
    GLfloat bahan_shininess[] = {90.0f};
    GLfloat bahan_ambient1[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat bahan_diffuse1[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat bahan_specular1[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat bahan_shininess1[] = {90.0f};
    GLfloat bahan_ambient2[] = {0.0f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_diffuse2[] = {0.0f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_specular2[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat bahan_shininess2[] = {90.0f};
    GLfloat bahan_ambient3[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat bahan_diffuse3[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat bahan_specular3[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat bahan_shininess3[] = {90.0f};
    GLfloat bahan_ambient4[] = {1.0f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_diffuse4[] = {1.0f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_specular4[] = {1.0f, 1.0f, 0.0f, 1.0f};
    GLfloat bahan_shininess4[] = {90.0f};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TiF-UAJY";
    RegisterClass(&wc);
    hWnd = CreateWindow("TiF-UAJY", "210711282-quiz5", WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, 0, 0, 1024, 768, NULL, NULL, hInstance, NULL);
    EnableOpenGL(hWnd, &hDC, &hRC);
    Inisialisasi(1024, 768);
    while (!bQuit) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0.0, 3.0, 4.0, 0.0, 1.2, -4.0, 0.0, 1.0, 0.0);
            glPushMatrix();
                glTranslatef(0, -1, -30);
                glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
                glPushMatrix();
                    glScalef(1.0, -1.0, 1.0);
                    glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
                    glEnable(GL_NORMALIZE);
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess);
                    glPushMatrix();
                        glTranslatef(-2.0, 0.0, 11.0f);
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        glutSolidTeapot(0.7f);
                    glPopMatrix();
                    glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient1);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse1);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular1);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess1);
                    glPushMatrix();
                        glTranslatef(-0.4, 0.0, 11.0f);
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        glScalef(0.35f, 0.35f, 0.35f);
                        glutSolidTorus(0.7f, 1.0f, 200, 200);
                    glPopMatrix();
                    glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient2);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse2);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular2);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess2);
                    glPushMatrix();
                        glTranslatef(0.8f, 0.0, 11.0f);
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        glutSolidSphere(0.6f, 200, 200);
                    glPopMatrix();
                    glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient3);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse3);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular3);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess3);
                    glPushMatrix();
                        glTranslatef(2.0f, 0.0, 11.0f);
                        glScalef(0.5f, 0.5f, 0.5f);
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        kubusTekstur();
                    glPopMatrix();
                    glPushMatrix();
                        gambarTV();
                    glPopMatrix();
                    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
                    glPushMatrix();
                        glTranslatef(-6.0f, 0.0f, 14.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(-6.0f, 0.0f, 16.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(-6.0f, 0.0f, 18.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix();
                    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
                    glPushMatrix();
                        glTranslatef(-6.0f, 2.0f, 14.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(-6.0f, 2.0f, 16.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix();
                    glPushMatrix();
                        glTranslatef(-6.0f, 2.0f, 18.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix();
                    glCullFace(GL_BACK);
                    glDisable(GL_CULL_FACE);
                glPopMatrix();
                glPushMatrix();
                    gambarDinding();
                    gambarDindingKiri();
                    gambarDindingKanan();
                glPopMatrix();
                glPushMatrix();
                    glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor4f(0.2f, 0.2f, 0.4f, 0.8f);
                    gambarLantai();
                    glDisable(GL_BLEND);
                    glEnable(GL_CULL_FACE);
                    glFrontFace(GL_CW);
                    glColor4f(0.1f, 0.1f, 0.7f, 1.0);
                    gambarLantai();
                    glFrontFace(GL_CCW);
                    glDisable(GL_CULL_FACE);
                    glDisable(GL_BLEND);
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(0.0f, 0.0f, 0.0f);
                    gambarBox();
                glPopMatrix();
                glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess);
                glPushMatrix();
                    glTranslatef(-2.0, 1.6, 11.0f);
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    glutSolidTeapot(0.7f);
                glPopMatrix();
                glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient1);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse1);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular1);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess1);
                glPushMatrix();
                    glTranslatef(-0.4, 1.65, 11.0f);
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    glScalef(0.35f, 0.35f, 0.35f);
                    glutSolidTorus(0.7f, 1.0f, 200, 200);
                glPopMatrix();
                glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient2);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse2);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular2);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess2);
                glPushMatrix();
                    glTranslatef(0.8f, 1.65, 11.0f);
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    glutSolidSphere(0.6f, 200, 200);
                glPopMatrix();
                glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient3);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse3);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular3);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess3);
                glPushMatrix();
                    glTranslatef(2.0f, 1.65, 11.0f);
                    glScalef(0.5f, 0.5f, 0.5f);
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    kubusTekstur();
                glPopMatrix();
                glPushMatrix();
                    gambarTV();
                glPopMatrix();
                glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
                glPushMatrix();
                    glTranslatef(-6.0f, 0.0f, 14.0f);
                    glScalef(1.0f, 1.0f, 1.0f);
                    display();
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(-6.0f, 0.0f, 16.0f);
                    glScalef(1.0f, 1.0f, 1.0f);
                    display();
                glPopMatrix();
                glPushMatrix();
                    glTranslatef(-6.0f, 0.0f, 18.0f);
                    glScalef(1.0f, 1.0f, 1.0f);
                    display();
                glPopMatrix();
            glPopMatrix();
            xrot += 0.3f;
            yrot += 0.2f;
            zrot += 0.4f;
            glFlush();
            SwapBuffers(hDC);
        }
    }
    DisableOpenGL(hWnd, hDC, hRC);
    DestroyWindow(hWnd);
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            return 0;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    return 0;
            }
            return 0;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    *hDC = GetDC(hWnd);
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}
