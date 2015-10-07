//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2014-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fÃ¼ggvÃ©nyt kivÃ©ve, a lefoglalt adat korrekt felszabadÃ­tÃ¡sa nÃ©lkÃ¼l
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : <VEZETEKNEV(EK)> <KERESZTNEV(EK)>
// Neptun : <NEPTUN KOD>
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>


/********DEBUG MIATT INCLUDE STDIO*/
#include <iostream>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <openGL/gl.h>
#include <GL/glu.h>
#include <GLUT/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
    float x, y, z;
    double t;
    
    Vector( ) {
        x = y = z =  t = 0;
    }
    Vector(float x0, float y0, float z0 = 0) {
        x = x0; y = y0; z = z0;
    }
    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a);
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    float operator*(const Vector& v) { 	// dot product
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v) { 	// cross product
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length() { return sqrt(x * x + y * y + z * z); }
    
    
    
};
///Struct a controlpontok tarolasara///
struct ControlPoints{
    int amount;
    Vector points[20];
    long t[20];
    
    ControlPoints(){
        amount = 0;
        for(int i = 0; i < 20; i++){
            points[i] = *new Vector();
        }

    }
    
    void addPoint(Vector p, int time){
    
        if(amount == 0){
            points[0] = p;
            t[0] = time;
            amount++;
        }
         else   {
            points[amount] = p;
            t[amount] = time;
            amount++;
        }
    }
    
};
ControlPoints contpoints = ControlPoints();

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
    float r, g, b;
    
    Color( ) {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
    Color operator*(const Color& c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

const int screenWidth = 600;	// alkalmazÃ¡s ablak felbontÃ¡sa
const int screenHeight = 600;


Color image[screenWidth*screenHeight];	// egy alkalmazÃ¡s ablaknyi kÃ©p
/****************/
ControlPoints cps = *new ControlPoints();
Vector v;
Vector convertToNdc(float x, float y) {
    Vector ret;
    ret.x = (x - screenWidth/2) / (screenWidth/2);
    ret.y = (screenHeight/2 - y) / (screenHeight/2);
    return ret;
}


// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization( ) {
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Peldakent keszitunk egy kepet az operativ memoriaba
    for(int Y = 0; Y < screenHeight; Y++)
        for(int X = 0; X < screenWidth; X++)
            image[Y*screenWidth + X] = Color((float)X/screenWidth, (float)Y/screenHeight, 0);
    
}



double distance_to_Line(Vector p0, Vector p1, Vector point){
    double normalLength = hypot(p1.x - p0.x, p1.y - p0.y);
    double distance = (double)((point.x - p0.x) * (p1.y - p0.y) - (point.y - p0.y) * (p1.x - p0.x)) / normalLength;
    return distance;
}



void drawCatmullRom(ControlPoints cps ){
    //vesszuk az osszes kontrollpontot
    if (cps.amount > 2) {
        for (int i = 0; i < cps.amount-1; i++) {
            Vector v0,v1;
            
            if (i == 0) {
                v0.x = 0.5f;
                v0.y = 0.5f;
            }
            Vector e,s;
            //veszunk egy szegmenst
            for (long t = cps.t[i]; cps.t[i+1]; t++) {
                
                
                //EXPLICIT KIFEJEZZUK Az elsot es az utolsot
                if (t == cps.t[i]) {
                    s = cps.points[i];
                }
                
                //egy szegmensben a futo parameter a t, kezdete a kezdeti pont t-je, vege a vegpont t-je
                //hogy megtalajuk a polinomot, amibe beledobjuk a futo t-t, ki kell szamolni az egyutthatokat
                float a0x,a0y,a1x,a1y,a2x,a2y,a3x,a3y = 0;
                
                //mivel 2D ben vagyunk, a R 2d-s vektor, de egy doublellel (t) kell leosztani -- magyarul 1/t vel szorozni
                
                
                v1.x =  ( ((cps.points[i+1].x - cps.points[i].x) *  (1/(cps.t[i+1] - cps.t[i])))  +  ((cps.points[i].x - cps.points[i-1].x) * (1/(cps.t[i] - cps.t[i-1]))))  ;
                v1.y =  ( ((cps.points[i+1].y - cps.points[i].y) *  (1/(cps.t[i+1] - cps.t[i])))  +  ((cps.points[i].y - cps.points[i-1].y) * (1/(cps.t[i] - cps.t[i-1]))))  ;
                //2D ben ay x es az y koordinatakra is meg kell talanunk a polinomot,
                //a0 = xi...
                a0x = cps.points[i].x;
                a1x = v1.x;//EZ ITT A Vi sebesseg, veletlenszeru
                a2x = ( ( 3 * (cps.points[i+1].x - cps.points[i].x) ) / (powl(cps.t[i+1]- cps.t[i],2))  ) - ( (v1.x - ( 2 * v0.x ) * (1/(cps.t[+1] - cps.t[i]))) )  ;
                a3x = ( ( 2 * (cps.points[i].x - cps.points[i+1].x) ) / (powl(cps.t[i+1]- cps.t[i],3))  ) - ( (v1.x - ( v0.x ) * (1/ pow(cps.t[+1] - cps.t[i],2))) )  ;
                
                
                a0y = cps.points[i].y;
                a1y = v1.y;//EZ ITT A Vi sebesseg, veletlenszeru
                a2y = ( ( 3 * (cps.points[i+1].y - cps.points[i].y) ) / (powl(cps.t[i+1]- cps.t[i],2))  ) - ( (v1.y - ( 2 * v0.y ) * (1/(cps.t[+1] - cps.t[i]))) )  ;
                a3y = ( ( 2 * (cps.points[i].y - cps.points[i+1].y) ) / (powl(cps.t[i+1]- cps.t[i],3))  ) - ( (v1.y - ( v0.y ) * (1/ pow(cps.t[+1] - cps.t[i],2))) )  ;
                
                
                //megvannak az egyutthatok - most pedig bedobjuk a fgv-be a futo t parametert, illetve az elozo pontot
                
                e.x = (a3x*(pow (t - cps.t[i],3))) + (a2x*(pow(t - cps.t[i],2))) + (a1x*(t - cps.t[i])) + a0x;
                e.y = (a3y*(pow (t - cps.t[i],3))) + (a2y*(pow(t - cps.t[i],2))) + (a1y*(t - cps.t[i])) + a0y;
                
                glBegin(GL_LINES); {
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glVertex2f(s.x,s.y);
                    glVertex2f(e.x, e.y);
                } glEnd();
                
                s = e;
                
            }
        }
    }
    
    
}

void drawFilledCircle(float x,float y,float r){
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN); {
        for (int j = 0; j < 32; j++) {
            //float angle = float(j) / 32 * 2.0f * M_PI;
            float angle =   2.0f * 3.1415926f * float(j) / float(32);
            // Itt a kor paramtetrikus alakjat hasznaljuk: x = x0 + r*cos(t), y = y0 + r * sin(t)
            glVertex2f(x + r*cosf(angle), y + r*sinf(angle));
            //glVertex2f(-0.5f + r*cos(angle), 0.4f + r*sin(angle));
        }
    }glEnd();
    
}
void drawCircle(float x,float y,float r){
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP); {
        for (int j = 0; j < 32; j++) {
            //float angle = float(j) / 32 * 2.0f * M_PI;
            float angle =   2.0f * 3.1415926f * float(j) / float(32);
            // Itt a kor paramtetrikus alakjat hasznaljuk: x = x0 + r*cos(t), y = y0 + r * sin(t)
            glVertex2f(x + r*cosf(angle), y + r*sinf(angle));
            //glVertex2f(-0.5f + r*cos(angle), 0.4f + r*sin(angle));
        }
    }glEnd();
}
// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay( ) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles
    if (contpoints.amount != 0) {
            glColor3f(1.0f, 1.0f, 1.0f);
            for (int i = 0; i < contpoints.amount; i++) {
                drawFilledCircle(contpoints.points[i].x,contpoints.points[i].y,0.04f);
                drawCircle(contpoints.points[i].x,contpoints.points[i].y,0.04f);
                
            }
        drawCatmullRom(contpoints);
    }
   
    glutSwapBuffers();     				// Buffercsere: rajzolas vege
    
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay( ); 		// d beture rajzold ujra a kepet
    
}

// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {
    
}

float x,y;
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {
    if ( (button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) ) {  // A GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON illetve GLUT_DOWN / GLUT_UP
        Vector v;
        v = convertToNdc(x, y);
        long t = glutGet(GLUT_ELAPSED_TIME);
        contpoints.addPoint(v,t);
        std::cout << contpoints.amount  << "  ";
        glutPostRedisplay( );   // Ilyenkor rajzold ujra a kepet
    }
    
}




// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y)
{
    
}

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle( ) {
    long time = glutGet(GLUT_ELAPSED_TIME);		// program inditasa ota eltelt ido
    
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();					// Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}
