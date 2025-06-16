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

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
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
//******************************************************
// Awal kode yang diambil dari bukunya Suyoto

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

            // (Opsional) lengkungkan tengah permukaan
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
            teapotZ -= 0.5f; // move closer (decrease Z)
            printf("Teapot Z: %.2f\n", teapotZ);
            break;
        case 's':
        case 'S':
            teapotZ += 0.5f; // move farther (increase Z)
            printf("Teapot Z: %.2f\n", teapotZ);
            break;
        // ... other keys ...
    }
    glutPostRedisplay(); // request redraw
}


AUX_RGBImageRec *LoadBMP(char *Filename)				// fungsi menampilkan gambar
{
	FILE *File=NULL;									

	if (!Filename)										
	{
		return NULL;								
	}

	File=fopen(Filename,"r");							// digunakan untuk membaca file 

	if (File)											
	{
		fclose(File);									
		return auxDIBImageLoad(Filename);				// jika gambar ada tampilkan gambar
	}

	return NULL;										
}

int LoadGLTextures()									
{
    int Status = FALSE;									

    AUX_RGBImageRec *TextureImage[2];

    memset(TextureImage, 0, sizeof(void *) * 2); 

    // Load first texture
    if (TextureImage[0] = LoadBMP("GUAMBAR.bmp")) // 256x256
    {
        // Load second texture
        if (TextureImage[1] = LoadBMP("MOMO.bmp")) // another BMP file
        {
            Status = TRUE;

            glGenTextures(2, &texture[0]); // Generate 2 texture

            // Texture 1
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Texture 2
            glBindTexture(GL_TEXTURE_2D, texture[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }

    // Clean up texture image memory
    for (int i = 0; i < 2; i++)
    {
        if (TextureImage[i])
        {
            if (TextureImage[i]->data)
                free(TextureImage[i]->data);
            free(TextureImage[i]);
        }
    }

    return Status;
}

void kubus()
{	     
        glBegin(GL_QUADS);        
        // Muka depan
        glVertex3f(-1.0f, -1.0f,  1.0f);	// Titik 1 (depan)
        glVertex3f( 1.0f, -1.0f,  1.0f);	// Titik 2 (depan)
        glVertex3f( 1.0f,  1.0f,  1.0f);	// Titik 3 (depan)
        glVertex3f(-1.0f,  1.0f,  1.0f);	// Titik 4 (depan)
    
        // Muka belakang		
       	glVertex3f(-1.0f, -1.0f, -1.0f);	// Titik 1 (belakang)
       	glVertex3f(-1.0f,  1.0f, -1.0f);	// Titik 2 (belakang)
       	glVertex3f( 1.0f,  1.0f, -1.0f);	// Titik 3 (belakang)
       	glVertex3f( 1.0f, -1.0f, -1.0f);	// Titik 4 (belakang)
	
	    // Muka Atas	
	    glVertex3f(-1.0f,  1.0f, -1.0f);	// Titik 1 (atas)
	    glVertex3f(-1.0f,  1.0f,  1.0f);	// Titik 2 (atas)
	    glVertex3f( 1.0f,  1.0f,  1.0f);	// Titik 3 (atas)
	    glVertex3f( 1.0f,  1.0f, -1.0f);	// Titik 4 (atas)
	
	    // Muka bawah
	    glVertex3f(-1.0f, -1.0f, -1.0f);	// Titik 1 (bawah)
	    glVertex3f( 1.0f, -1.0f, -1.0f);	// Titik 2 (bawah)
	    glVertex3f( 1.0f, -1.0f,  1.0f);	// Titik 3 (bawah)
	    glVertex3f(-1.0f, -1.0f,  1.0f);	// Titik 4 (bawah)
	
    	// Muka kanan
    	glVertex3f( 1.0f, -1.0f, -1.0f);	// Titik 1 (kanan)
    	glVertex3f( 1.0f,  1.0f, -1.0f);	// Titik 2 (kanan)
    	glVertex3f( 1.0f,  1.0f,  1.0f);	// Titik 3 (kanan)
        glVertex3f( 1.0f, -1.0f,  1.0f);	// Titik 4 (kanan)
   	
	    // Muka kiri
	    glVertex3f(-1.0f, -1.0f, -1.0f);	// Titik 1 (kiri)
	    glVertex3f(-1.0f, -1.0f,  1.0f);	// Titik 2 (kiri)
	    glVertex3f(-1.0f,  1.0f,  1.0f);	// Titik 3 (kiri)
	    glVertex3f(-1.0f,  1.0f, -1.0f);	// Titik 4 (kiri)
	
	    glEnd();
        // Akhir kode yang diambil dari bukunya Suyoto
        //******************************************************
}

void gambarDinding() {
    // Set wall color (e.g., blue)
    glDisable(GL_TEXTURE_2D);      // No texture
    glColor3f(0.2f, 0.2f, 0.8f);   // Blue color

    glBegin(GL_QUADS);
        // Normal facing forward (towards +Z)
        glNormal3f(0.0f, 0.0f, 1.0f);

        glVertex3f(-10.0f, 0.0f, -10.0f);  // Bottom-left
        glVertex3f( 10.0f, 0.0f, -10.0f);  // Bottom-right
        glVertex3f( 10.0f, 10.0f, -10.0f); // Top-right
        glVertex3f(-10.0f, 10.0f, -10.0f); // Top-left
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f); // Reset color (optional)
}

void gambarDindingKiri() {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.8f, 0.2f, 0.2f); // Red color for left wall

    glBegin(GL_QUADS);
        // Normal facing right (+X)
        glNormal3f(1.0f, 0.0f, 0.0f);

        glVertex3f(-10.0f, 0.0f, -10.0f);  // Back-bottom
        glVertex3f(-10.0f, 10.0f, -10.0f); // Back-top
        glVertex3f(-10.0f, 10.0f,  50.0f); // Front-top
        glVertex3f(-10.0f, 0.0f,  50.0f);  // Front-bottom
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
}

void gambarDindingKanan() {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.2f, 0.8f, 0.2f); // Green color for right wall

    glBegin(GL_QUADS);
        // Normal facing left (-X)
        glNormal3f(-1.0f, 0.0f, 0.0f);

        glVertex3f(10.0f, 0.0f, -10.0f);  // Back-bottom
        glVertex3f(10.0f, 10.0f, -10.0f); // Back-top
        glVertex3f(10.0f, 10.0f,  50.0f); // Front-top
        glVertex3f(10.0f, 0.0f,  50.0f);  // Front-bottom
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
}

void gambarBox() {
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.7f, 0.5f, 0.2f); // Brownish color

    float xMin = -2.5f, xMax = 2.5f;   // width 6
    float yMin = 0.0f, yMax = 1.0f;    // height 1
    float zMin = 8.0f, zMax = 12.0f;   // depth 4

    // Top face
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(xMin, yMax, zMin);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMin, yMax, zMax);
    glEnd();

    // Bottom face
    glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMin, yMin, zMax);
    glEnd();

    // Front face
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMin, yMax, zMax);
    glEnd();

    // Back face
    glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMax, zMin);
        glVertex3f(xMin, yMax, zMin);
    glEnd();

    // Left face
    glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3f(xMin, yMin, zMin);
        glVertex3f(xMin, yMin, zMax);
        glVertex3f(xMin, yMax, zMax);
        glVertex3f(xMin, yMax, zMin);
    glEnd();

    // Right face
    glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(xMax, yMin, zMin);
        glVertex3f(xMax, yMin, zMax);
        glVertex3f(xMax, yMax, zMax);
        glVertex3f(xMax, yMax, zMin);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
    glPopMatrix();
}





void Inisialisasi(int lebar, int tinggi)
{
	glViewport (0, 0, lebar, tinggi); 
	
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glClear(GL_COLOR_BUFFER_BIT);     // bersihkan layar

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
    
    
    LoadGLTextures();

    // Buat grid titik kontrol
    generateGridFromCorners();
    
	// Buat objek NURBS
    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL); // GLU_OUTLINE_POLYGON untuk wireframe


  	gluPerspective(45.0, (GLfloat) lebar/(GLfloat) tinggi, 1, 50.0);

	glMatrixMode(GL_MODELVIEW);

}

void display() {
    glPushMatrix();

    // Tampilkan titik kontrol
    // glPointSize(5.0);
    glColor3f(0.0, 1.0, 0.0);
    // glBegin(GL_POINTS);
    // for (int i = 0; i < 4; ++i)
    //     for (int j = 0; j < 4; ++j)
    //         glVertex3fv(ctrlPoints[i][j]);
    // glEnd();

    // Gambar permukaan NURBS
    gluBeginSurface(theNurb);

    // Knot vector uniform untuk degree 3 (order 4) dan 4 control points: [0,0,0,0,1,1,1,1]
    GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0,
                         1.0, 1.0, 1.0, 1.0 };

    gluNurbsSurface(theNurb,
        8, knots,
        8, knots,
        4 * 3,           // Distance antar kolom dalam array
        3,               // Distance antar baris dalam array
        &ctrlPoints[0][0][0],
        4, 4,            // order (degree + 1)
        GL_MAP2_VERTEX_3);

    gluEndSurface(theNurb);
    
    kubus();

    glPopMatrix();
}

void gambarLantai()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float repeatX = 10.0f;
    float repeatZ = 10.0f; 

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal

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


void kubusTekstur()
{
        glEnable(GL_TEXTURE_2D);	
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        
        glBegin(GL_QUADS);        
        // Muka depan
        glNormal3f( 0.0f, 0.0f, 1.0f);		// Normal menuju Anda
        glTexCoord2f(0.0f, 0.0f); 
        glVertex3f(-1.0f, -1.0f,  1.0f);	// Titik 1 (depan)
        glTexCoord2f(1.0f, 0.0f); 
        glVertex3f( 1.0f, -1.0f,  1.0f);	// Titik 2 (depan)
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f( 1.0f,  1.0f,  1.0f);	// Titik 3 (depan)
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f,  1.0f,  1.0f);	// Titik 4 (depan)
    
        // Muka belakang		
       	glNormal3f( 0.0f, 0.0f,-1.0f);		// Normal meninggalKan Anda
       	glTexCoord2f(1.0f, 0.0f);
       	glVertex3f(-1.0f, -1.0f, -1.0f);	// Titik 1 (belakang)
       	glTexCoord2f(1.0f, 1.0f);
       	glVertex3f(-1.0f,  1.0f, -1.0f);	// Titik 2 (belakang)
       	glTexCoord2f(0.0f, 1.0f);
       	glVertex3f( 1.0f,  1.0f, -1.0f);	// Titik 3 (belakang)
       	glTexCoord2f(0.0f, 0.0f);
       	glVertex3f( 1.0f, -1.0f, -1.0f);	// Titik 4 (belakang)
	
	    // Muka Atas	
	    glNormal3f( 0.0f, 1.0f, 0.0f);		// Normal berarah atas
	    glTexCoord2f(0.0f, 1.0f);
	    glVertex3f(-1.0f,  1.0f, -1.0f);	// Titik 1 (atas)
	    glTexCoord2f(0.0f, 0.0f);
	    glVertex3f(-1.0f,  1.0f,  1.0f);	// Titik 2 (atas)
	    glTexCoord2f(1.0f, 0.0f);
	    glVertex3f( 1.0f,  1.0f,  1.0f);	// Titik 3 (atas)
	    glTexCoord2f(1.0f, 1.0f);
	    glVertex3f( 1.0f,  1.0f, -1.0f);	// Titik 4 (atas)
	
	    // Muka bawah
	    glNormal3f( 0.0f,-1.0f, 0.0f);		// Normal berarah bawah
	    glTexCoord2f(1.0f, 1.0f);
	    glVertex3f(-1.0f, -1.0f, -1.0f);	// Titik 1 (bawah)
	    glTexCoord2f(0.0f, 1.0f);
	    glVertex3f( 1.0f, -1.0f, -1.0f);	// Titik 2 (bawah)
	    glTexCoord2f(0.0f, 0.0f);
	    glVertex3f( 1.0f, -1.0f,  1.0f);	// Titik 3 (bawah)
	    glTexCoord2f(1.0f, 0.0f);
	    glVertex3f(-1.0f, -1.0f,  1.0f);	// Titik 4 (bawah)
	
    	// Muka kanan
    	glNormal3f( 1.0f, 0.0f, 0.0f);		// Normal berarah ke kanan
    	glTexCoord2f(1.0f, 0.0f);
    	glVertex3f( 1.0f, -1.0f, -1.0f);	// Titik 1 (kanan)
    	glTexCoord2f(1.0f, 1.0f);
    	glVertex3f( 1.0f,  1.0f, -1.0f);	// Titik 2 (kanan)
    	glTexCoord2f(0.0f, 1.0f);
    	glVertex3f( 1.0f,  1.0f,  1.0f);	// Titik 3 (kanan)
    	glTexCoord2f(0.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f,  1.0f);	// Titik 4 (kanan)
   	
	    // Muka kiri
	    glNormal3f(-1.0f, 0.0f, 0.0f);		// Normal berarah ke kiri
	    glTexCoord2f(0.0f, 0.0f);
	    glVertex3f(-1.0f, -1.0f, -1.0f);	// Titik 1 (kiri)
	    glTexCoord2f(1.0f, 0.0f);
	    glVertex3f(-1.0f, -1.0f,  1.0f);	// Titik 2 (kiri)
	    glTexCoord2f(1.0f, 1.0f);
	    glVertex3f(-1.0f,  1.0f,  1.0f);	// Titik 3 (kiri)
	    glTexCoord2f(0.0f, 1.0f);
	    glVertex3f(-1.0f,  1.0f, -1.0f);	// Titik 4 (kiri)
	
	    glEnd();
	    // glDisable(GL_TEXTURE_2D);
        // Akhir kode yang diambil dari bukunya Suyoto
        //******************************************************
}

// Akhir kode yang diambil dari bukunya Suyoto
//******************************************************

/**************************
 * WinMain
 *
 **************************/

int WINAPI 
WinMain (HINSTANCE hInstance,
         HINSTANCE hPrevInstance,
         LPSTR lpCmdLine,
         int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;

    // Cahaya
	GLfloat light_pos[] = { 0.0f, 3.0f, 5.0f, 1.0f };
	
	// Bahan / material
	GLfloat mat_ambient[]   = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
	GLfloat mat_diffuse[]   = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
	GLfloat mat_specular[]  = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
	GLfloat mat_shininess[] = { 51.2f };

	GLfloat    intensitasCahaya[ ] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat    posisiCahaya[ ]={2.0f,2.0f,2.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, intensitasCahaya);

	GLfloat    bahan_ambient[ ]= {1.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_diffuse[ ] = {0.6f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_specular[ ] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat    bahan_shininess[ ] = {90.0f};

	GLfloat    bahan_ambient1[ ]= {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat    bahan_diffuse1[ ] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat    bahan_specular1[ ] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat    bahan_shininess1[ ] = {90.0f};

	GLfloat    bahan_ambient2[ ]= {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_diffuse2[ ] = {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_specular2[ ] = {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_shininess2[ ] = {90.0f};
	
    GLfloat    bahan_ambient3[ ]= {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat    bahan_diffuse3[ ] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat    bahan_specular3[ ] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat    bahan_shininess3[ ] = {90.0f};
	
	GLfloat    bahan_ambient4[ ]= {1.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_diffuse4[ ] = {1.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_specular4[ ] = {1.0f, 1.0f, 0.0f, 1.0f};
	GLfloat    bahan_shininess4[ ] = {90.0f};	      
      
    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TiF-UAJY";
    RegisterClass (&wc);

    /* create main window */
    hWnd = CreateWindow (
      "TiF-UAJY", "210711282-quiz5", 
      WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
      0, 0, 1024, 768,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    
    
    //******************************************************
    // Awal kode yang diambil dari bukunya Suyoto
    Inisialisasi(1024, 768);  
    // glutDisplayFunc(display);
    // glutKeyboardFunc(keyboard); 
    // Akhir kode yang diambil dari bukunya Suyoto
    //******************************************************
    
    
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(
                0.0, 1.0, 8.0,   // Eye position (camera)
                0.0, 1.0, 0.0,    // Look-at target (center of scene)
                0.0, 1.0, 0.0     // Up vector
            );

            
            //******************************************************
            // Awal kode yang diambil dari bukunya Suyoto
            glPushMatrix();	
            	// Translasikan Objek ke posisi (0,0,0). 
            	glTranslatef(0, -1, -30);
            		
            	//posisi cahaya di balik (di bawah)
            	glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
            
            	glPushMatrix();
            		// Langkah pantulan kritis: Pantulkan Objek ke lantai
            		//(bidang Y=0) untuk membuat pantulan. 
            		glScalef(1.0, -1.0, 1.0);
            		// Posisi sinar cahaya otomatis juga dipantulkan.
            		glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
            		glEnable(GL_NORMALIZE);
            		glEnable(GL_CULL_FACE);
            		glCullFace(GL_FRONT);

                    float yBoxTop = 2.0f;
            			
            		// Gambar Objek terpantul 
			        //******************************************************
			        // Awal kode yang diambil dari bukunya Suyoto
			        glMaterialfv(GL_FRONT,GL_AMBIENT, bahan_ambient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess);
                    glPushMatrix();
                        glTranslatef(-4.0, yBoxTop + 0.1, 0.0); // +0.1 to avoid z-fighting
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        glutSolidTeapot(0.7f);
                    glPopMatrix();

                    glMaterialfv(GL_FRONT,GL_AMBIENT, bahan_ambient1);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse1);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular1);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess1);
                    glPushMatrix();
                        glTranslatef(1.0, yBoxTop + 0.2, 0.0);
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        glutSolidTorus(0.7f, 1.0f, 200, 200);
                    glPopMatrix();

                    glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient2);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse2);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular2);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess2);
                    glPushMatrix();
                        glTranslatef(-2.0f, yBoxTop + 0.2f, 2.0f); // Adjust z if needed
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        glutSolidSphere(1.4f, 200, 200);
                    glPopMatrix();

                    glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient3);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse3);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular3);
                    glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess3);
                    glPushMatrix();
                        glTranslatef(5.0f, yBoxTop + 0.2f, -4.0f); // Adjust z if needed
                        glScalef(2.0f, 2.0f, 2.0f);
                        glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                        kubusTekstur();
                    glPopMatrix();

                    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
                    glPushMatrix( );
                        glTranslatef (-6.0f, 2.0f, 14.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix( ); 
                    glPushMatrix( );
                        glTranslatef (-6.0f, 2.0f, 16.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix( ); 
                    glPushMatrix( );
                        glTranslatef (-6.0f, 2.0f, 18.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix( ); 
            			
            		glCullFace(GL_BACK);
					glDisable(GL_NORMALIZE);
					glDisable(GL_CULL_FACE);
					
            	glPopMatrix();

                glPushMatrix();
                    gambarDinding();       // Back wall
                    gambarDindingKiri();   // Left wall
                    gambarDindingKanan();  // Right wall
                glPopMatrix();
            	
            	glPushMatrix( );
	                	//posisi cahaya dikembalikan ke posisi semula
	            	glLightfv(GL_LIGHT0, GL_POSITION, posisiCahaya);
	            
	            	// gambar bagian atas lantai
	            	glEnable(GL_BLEND);
	            	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	            	glColor4f(0.2f, 0.2f, 0.4f, 0.8f);
	            	gambarLantai();
                    
	            	glDisable(GL_BLEND);
	            
	            	// gambar bagian bawah lantai
	            	glEnable(GL_CULL_FACE);
	            	glFrontFace(GL_CW);  // pindah orientasi permukaan
	            	glColor4f(0.1f, 0.1f, 0.7f, 1.0);
	            	gambarLantai();
	            	glFrontFace(GL_CCW);
	            	glDisable(GL_CULL_FACE);
					glDisable(GL_BLEND);
				glPopMatrix();

                
            
                

            	// gambar objek aslinya, tidak pantulannya
                glPushMatrix();
                    glTranslatef(0.0f, 0.0f, 10.0f);
                    gambarBox();
                glPopMatrix();
                

                glMaterialfv(GL_FRONT,GL_AMBIENT, bahan_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess);
                glPushMatrix();
                // W AND S here
                    glTranslatef(-2.0, yBoxTop + 0.1, teapotZ); // +0.1 to avoid z-fighting
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    glutSolidTeapot(0.7f);
                glPopMatrix();
                glMaterialfv(GL_FRONT,GL_AMBIENT, bahan_ambient1);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse1);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular1);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess1);
                glPushMatrix();
                    glTranslatef(1.0, yBoxTop + 0.2, 0.0);
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    glutSolidTorus(0.7f, 1.0f, 200, 200);
                glPopMatrix();

                glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient2);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse2);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular2);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess2);
                glPushMatrix();
                    glTranslatef(-2.0f, yBoxTop + 0.2f, 2.0f); // Adjust z if needed
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    glutSolidSphere(1.4f, 200, 200);
                glPopMatrix();

                glMaterialfv(GL_FRONT, GL_AMBIENT, bahan_ambient3);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, bahan_diffuse3);
                glMaterialfv(GL_FRONT, GL_SPECULAR, bahan_specular3);
                glMaterialfv(GL_FRONT, GL_SHININESS, bahan_shininess3);
                glPushMatrix();
                    glTranslatef(5.0f, yBoxTop + 0.2f, -4.0f); // Adjust z if needed
                    glScalef(2.0f, 2.0f, 2.0f);
                    glRotatef(yrot+20, 0.0f, 1.0f, 0.0f);
                    kubusTekstur();
                glPopMatrix();

                glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
                    glPushMatrix( );
                        glTranslatef (-6.0f, 0.0f, 14.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix( ); 
                    glPushMatrix( );
                        glTranslatef (-6.0f, 0.0f, 16.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix( ); 
                    glPushMatrix( );
                        glTranslatef (-6.0f, 0.0f, 18.0f);
                        glScalef(1.0f, 1.0f, 1.0f);
                        display();
                    glPopMatrix( ); 

		        	
            glPopMatrix();

            xrot+=0.3f;   //Mengatur arah putaran object
	        yrot+=0.2f;   //Mengatur arah putaran object
	        zrot+=0.4f;   //Mengatur arah putaran object

            glFlush ( );
            // Akhir kode yang diambil dari bukunya Suyoto
            //******************************************************
            
            SwapBuffers (hDC);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK 
WndProc (HWND hWnd, UINT message,
         WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void
EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

}


/******************
 * Disable OpenGL
 *
 ******************/

void
DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}
