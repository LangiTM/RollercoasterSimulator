/*
 *	rollercoaster.c
 * Tyler Langille
 * B00615482
 * CSCI3161 Assign 3
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* -- type definitions --------------------------------------------------- */

typedef struct Point {
  float x, y, z;
  struct Point* next;
} Point;

typedef struct Curve {
  struct Point* pBez;
  struct Point* pSub;
  struct Circle* centR;
  struct Circle* leftR;
  struct Circle* rightR;
  struct Point* up;
  struct Curve* next;
} Curve;

typedef struct Circle {
  struct Point point[7];
  struct Point center;
  struct Circle* next;
} Circle;


/* -- function prototypes --------------------------------------------------- */

        /*    callback functions    */
void	myDisplay(void);
void	myTimer(int value);
void	myKey(unsigned char key, int x, int y);

void	init(void);
void    drawCurves(void);
void    drawCylinder(Circle* a, Circle* b, int t);
void    drawTrack(int t);
float*  lookAt(float[3], float[3], float[3]);
void    rideTrack(void);

        /*    helper functions    */

//Misc Functions
void displayText(float x, float y, float r, float g, float b,
			    const char *string);

//Matrix Functions
float    normalize(float u[3]);
float    dotProd(float u[3], float v[3]);
void     sub(float u[3], float v[3], float r[3]);
void     crossMult(float u[3], float v[3], float r[3]);
void     multMM(float* a, float* b, float* r);
void     multVM(float* a, float* b);

void     toArray(Point* p, float v[3]);
void     toPoint(float v[3], Point* p);

//Curve processing Functions
void buildLL(void);
void buildCircle(Circle* a, float b, float c, float d, float e, float* f);
void buildCurve(void);
void buildRail(void);
Circle* nextCurveCircle(Curve*, Curve*, Circle*, int);
Point* nextCurvePoint(Curve*, Curve*, Point*);
Point* nextPoint(Point*, Point*);
Curve* nextCurve(Curve*, Curve*);
float* midP(float*, float*);
void subdivideBez(Curve*, int, float*, float*, float*, float*);
void subdivideCurve();

/* -- global variables ------------------------------------------------------ */

int      Win_Width = 840, Win_Height = 620;
int      rideCoaster = 0, showSun = 200;
float    velocity = 0.03;
float    eyeX = 0.0, eyeY = 90.0, eyeZ = 2.0, atX = 0.0, atY = 30.0, atZ = 0.0, upX = 0.0, upY = 1.0, upZ = 0.0;
int      cAt = 0, cEye = 0, currUp = 1;
static GLuint   track, curves;
static GLfloat  ambient[] = { 1, 1, 1, 1.0 };
static GLfloat  diffuse[] = {.5, .5, .5, 1.0 };
static GLfloat  specular[] = { .3, .3, .3, 1.0 };
static GLfloat  position[] = { 1.0, 2.0, 0.0, 1.0 };
float    controlPoints[100][3] = {
  {20.0, 40.0, 20.0}, {5.0, 45.0, 15.0}, {-5.0, 45.0, 10.0}, {-20.0, 40.0, 5.0}, {-25.0, 35.0, 0.0},
    {-25.0, 10.0, 0.0}, {-32.0, 0.0, 5.0}, {-50.0, 0.0, 10.0}, {-60.0, 10.0, 10.0}, {-65.0, 25.0, 5.0},
    {-65.0, 35.0, 15.0}, {-65.0, 45.0, 10.0}, {-60.0, 50.0, 0.0}, {-55.0, 40.0, 5.0}, {-40.0, 40.0, -10.0},
    {-30.0, 40.0, -15.0}, {-20.0, 40.0, -20.0}, {-13.0, 40.0, -15.0}, {-0.0, 40.0, -10.0}, {10.0, 40.0, -5.0},
    {20.0, 40.0, -2.5}, {30.0, 35.0, 0.0}, {35.0, 25.0, 2.5}, {45.0, 20.0, 5.0}, {55.0, 25.0, 7.5},
    {60.0, 35.0, 10.0}, {55.0, 45.0, 12.5}, {45.0, 50.0, 15.0}, {35.0, 45.0, 17.5}, {30.0, 35.0, 20.0},
    {35.0, 25.0, 22.5}, {45.0, 20.0, 25.0}, {55.0, 25.0, 27.5}, {60.0, 35.0, 30.0}, {55.0, 45.0, 32.5},
    {45.0, 50.0, 35.0}, {35.0, 45.0, 37.5}, {30.0, 35.0, 40.0}, {35.0, 25.0, 42.5}, {45.0, 20.0, 45.0},
    {55.0, 25.0, 47.5}, {60.0, 35.0, 50.0}, {55.0, 45.0, 52.5}, {45.0, 50.0, 55.0}, {35.0, 45.0, 57.5},
    {30.0, 35.0, 60.0}, {35.0, 25.0, 60.0}, {45.0, 20.0, 60.0}, {55.0, 20.0, 60.0}, {65.0, 20.0, 60.0},
    {75.0, 20.0, 0.0}, {85.0, 12.5, 0.0}, {85.0, 2.5, 0.0}, {75.0, 0.0, 0.0}, {65.0, 0.0, 0.0},
    {55.0, 0.0, 10.0}, {45.0, 20.0, 20.0}, {35.0, 15.0, 20.0}, {25.0, 5.0, 10.0}, {25.0, 0.0, 0.0},
    {35.0, 0.0, -10.0}, {45.0, 0.0, -15.0}, {55.0, 5.0, -20.0}, {55.0, 10.0, -15.0}, {45.0, 15.0, -10.0},
    {35.0, 20.0, 0.0}, {25.0, 25.0, 10.0}, {20.0, 10.0, 20.0}, {5.0, 15.0, 15.0}, {-5.0, 15.0, 10.0}
 };
float           d = 0.0, s[3], sNext[3];
Point*          dP = NULL;
Point*          cUP = NULL;
Point*          currP = NULL;
Point*          tempP = NULL;
Curve*          currC = NULL;
Curve*          front = NULL;

/* -- main ------------------------------------------------------------------ */

int
main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(Win_Width, Win_Height);
    glutCreateWindow("Rollercoaster");
    glutDisplayFunc(myDisplay);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(myKey);
    glutTimerFunc(33, myTimer, 0);

    init();
    glutMainLoop();
    return 0;
}

/* -- callback functions ---------------------------------------------------- */

void
myDisplay()
{
  GLfloat m[16] = {0.0};
  int i;
  float s;
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(80, 2, 0.2, 300);
  glTranslated(0, -1, 0);
  gluLookAt(eyeX, eyeY, eyeZ, atX, atY, atZ, upX, upY, upZ);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* draw ground plane */
  glBegin(GL_POLYGON);
    glColor3f(0.06640625, 0.55859375, 0.0390625);
    glVertex3f(-300, 0, -300);
    glVertex3f(300, 0, -300);
    glVertex3f(300, 0, 300);
    glVertex3f(-300, 0, 300);    
  glEnd();
  
  /* draw sky */
  s = M_PI / 15;
  glBegin(GL_TRIANGLE_FAN);
    glColor3i(193, 208, 232);
    glVertex3f(0, 200, 0);
    glColor3f(.7294, .8578, 1);
    for(i = 0; i <= 30; i++){   
      glVertex3f(showSun * cos(i * s), 0, showSun * sin(i * s));
    }
  glEnd();
  
  glEnable(GL_LIGHTING);  
  glCallList(track);
  glDisable(GL_LIGHTING);
    
  glutSwapBuffers();
}

void
myTimer(int value)
{
  if (rideCoaster == 0){
    d += M_PI / 360.0;
    eyeX = 80.0 * sin(d);
    eyeZ = 80.0 * cos(d);
  }
  else {
    rideTrack();
  }
  glutPostRedisplay();
  glutTimerFunc(33, myTimer, value);		/* 30 frames per second */
}

void
myKey(unsigned char key, int x, int y)
{
    /*
     *	keyboard callback function
     */
  if (key=='t'||key=='T') {
    if (rideCoaster == 0) {
      rideCoaster = 1;
      currC = front;
      currP = currC->pSub;
      cUP = currC->up;
      eyeX = currP->x;
      eyeY = currP->y;
      eyeZ = currP->z;
      atX = currP->next->x;
      atY = currP->next->y;
      atZ = currP->next->z;
      cEye = 0;
    }
    else {
      rideCoaster = 0;
      eyeX = 30.0*sin(d);
      eyeY = 70.0;
      eyeZ = 30.0*cos(d);
      atX = 0.0;
      atY = 40.0;
      atZ = 0.0;
      upX = 0.0;
      upY = 1.0;
      upZ = 0.0;
    } 
  }
if (key=='s'||key=='S') {
    if (showSun == 200) {
      showSun = 300;
    }
    else {
      showSun = 200;
    } 
  }
  glutPostRedisplay();
}

/* -- other functions ------------------------------------------------------- */

void init() {
  buildLL();
  buildCurve();
  subdivideCurve();
  buildRail();
  glShadeModel(GL_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_DEPTH_TEST);
 
  track = glGenLists(1);
  glNewList(track, GL_COMPILE);
  drawTrack(1);
  glEndList();

  glEnable(GL_LIGHT0);/* enabled by default? */
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glClearDepth(1.0);
  /* Makes background colour yellow, looks like sunset */
  glClearColor(0.9453125, 0.9888, 0.00025, 1.0);
  glLineWidth(1);
}

void drawCylinder(Circle* b, Circle* t, int type){
  float p0[3], p1[3], p2[3], p3[3], d0[3], d1[3], n[3];
  int i;
  
  if(type == 2) {
    glBegin(GL_LINE_STRIP);   
  }
  else {
    glBegin(GL_TRIANGLE_STRIP); 
  }

  for(i = 0; i < 7; i++){
    toArray(&b->point[i], p0);
    toArray(&t->point[i], p1);
    if(i == 0){
      toArray(&b->point[6], p2);
      toArray(&t->point[6], p3);
    }
    else{
      toArray(&b->point[i - 1], p2);
      toArray(&t->point[i - 1], p3);
    }
    
    /* calc normalized up */
    sub(p2, p0, d0);
    sub(p3, p0, d1);
    crossMult(d0, d1, n);
    normalize(n);
    glNormal3fv(n);
    
    glVertex3fv(p0);
    
    /* calc normalized up */
    sub(p0, p1, d0);
    sub(p3, p1, d1);
    crossMult(d0, d1, n);
    normalize(n);
    glNormal3fv(n);
    
    glVertex3fv(p1);
  }
  glEnd();
}

void drawTrack(int type) {
  Curve* curr = front;
  Circle* currCenter, * currLeft, * currRight, * next;
  Circle tempPCircle1, tempPCircle2;
  float* m1, * m2, p1[3], p2[3], p3[3], p4[3];
  
  while(curr != NULL) {
    currCenter = curr->centR;
    currLeft = curr->leftR;
    currRight = curr->rightR;
    
    while(currCenter != NULL) {
      next = nextCurveCircle(front, curr, currCenter, 1);
      drawCylinder(currCenter, next, type);
      GLfloat ambient[] = {.9, .02, .02, 1.0 };
      GLfloat diffuse[] = {.4, .4, .4, 1.0 };
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    
      next = nextCurveCircle(front, curr, currLeft, 2);
      drawCylinder(currLeft, next, type);     
      next = nextCurveCircle(front, curr, currRight, 3);
      drawCylinder(currRight, next, type);
      
      toArray(&currCenter->center, p1);
      toArray(&currLeft->center, p2);
      toArray(&currRight->center, p3);
      toArray(curr->up, p4);
      m1 = lookAt(p1, p2, p4);
      m2 = lookAt(p1, p3, p4);
      
      buildCircle(&tempPCircle1, .05, 0, 0, 0, m1);
      buildCircle(&tempPCircle2, .05, 0, 0, 1, m1);
      drawCylinder(&tempPCircle1, &tempPCircle2, type);
      
      buildCircle(&tempPCircle1, .05, 0, 0, 0, m2);
      buildCircle(&tempPCircle2, .05, 0, 0, 1, m2);     
      drawCylinder(&tempPCircle1, &tempPCircle2, type);
      
      free(m1);
      free(m2);
      
      currCenter = currCenter->next;
      currLeft = currLeft->next;
      currRight = currRight->next;
    }
    curr = curr->next;
  }
}

float* lookAt(float p0[3], float p1[3], float up[3]) {
  float n[3], u[3], v[3], length;
  float* m = malloc(sizeof(float) * 17);
  sub(p1, p0, n);
  length = normalize(n);  
  crossMult(n, up, u);
  normalize(u);
  crossMult(u, n, v);
  
  m[0] = u[0];
  m[1] = u[1];
  m[2] = u[2];
  m[4] = v[0];
  m[5] = v[1];
  m[6] = v[2];
  m[8] = n[0] * length;
  m[9] = n[1] * length;
  m[10] = n[2] * length;
  m[12] = p0[0];
  m[13] = p0[1];
  m[14] = p0[2];
  m[15] = 1;
  m[3] = m[7] = m[11] = 0;
  m[16] = length;
  
  return m;
}

void rideTrack() {
  Point* next = currP, * nextnext;
  float length = 0, speedMAX = .4, speedMIN = .1;
  float  p[3], at[3];
  int i;
  
  if(cEye == 0){    
    next = currP;
    velocity -= (s[1] * .01);
    if(velocity > speedMAX) velocity = speedMAX;
    if(velocity < speedMIN) velocity = speedMIN;
    at[0] = eyeX;
    at[1] = eyeY;
    at[2] = eyeZ;
    for(i = 0; i < 2; i++){
      if(next->next == NULL){     
        currC = nextCurve(front, currC);
        next = currC->pSub;
        cUP = currC->up;
      }
      else{
        next = next->next;
        cUP = cUP->next;
      }
    }
    currP = next;
    toArray(next, p);
    sub(p, at, s);
    length = normalize(s);
    cEye = length / velocity;
  }
  
  /* update camera pos */
  eyeX += s[0] * velocity;
  eyeY += s[1] * velocity;
  eyeZ += s[2] * velocity;
  
  nextnext = nextCurvePoint(front, currC, currP);
  atX += (nextnext->x - atX) / cEye;
  atY += (nextnext->y - atY) / cEye;
  atZ += (nextnext->z - atZ) / cEye;
  
  upX += (cUP->x - upX) / cEye;
  upY += (cUP->y - upY) / cEye;
  upZ += (cUP->z - upZ) / cEye;
  
  cEye--;
}

/* -- helper function ------------------------------------------------------- */

void displayText(float x, float y, float r, float g, float b, const char *string) {
  int k = strlen(string), i;
  glColor3f(r,g,b);
  glRasterPos2f(x,y);
  for (i=0; i<k; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}

float normalize(float vertex[3]) {
  float result = sqrt(vertex[0]*vertex[0]+vertex[1]*vertex[1]+vertex[2]*vertex[2]);
  if (result>-0.001&&result<0.001) {
    return result;
  }
  vertex[0] = vertex[0]/result;
  vertex[1] = vertex[1]/result;
  vertex[2] = vertex[2]/result;
  return result;
}

float dotProd(float vertexU[3], float vertexV[3]) {
  return (vertexU[0]*vertexV[0]+vertexU[1]*vertexV[1]+vertexU[2]*vertexV[2]);
}

void sub(float vertexU[3], float vertexV[3], float result[3]) {
  result[0] = vertexU[0]-vertexV[0];
  result[1] = vertexU[1]-vertexV[1];
  result[2] = vertexU[2]-vertexV[2];
 }

void crossMult(float vertexU[3], float vertexV[3], float result[3]) {
  result[0] = vertexU[1]*vertexV[2]-vertexU[2]*vertexV[1];
  result[1] = vertexU[2]*vertexV[0]-vertexU[0]*vertexV[2];
  result[2] = vertexU[0]*vertexV[1]-vertexU[1]*vertexV[0];
}

void multMM(float* a, float* b, float* result) {
  int i,j,k;
  for (i=0;i<16;i++) {
    result[i] = 0.0;
  }
  for (i=0;i<4;i++) {
    for (j=0;j<4;j++) {
      for (k=0;k<4;k++) {
        result[4*j+i] += a[4*k+i]*b[4*j+k];
      }
    }
  }
}

void multVM(float* a, float* vertexV) {
  float tmp[3];
  tmp[0] = vertexV[0]*a[0]+vertexV[1]*a[4]+vertexV[2]*a[8]+a[12];
  tmp[1] = vertexV[0]*a[1]+vertexV[1]*a[5]+vertexV[2]*a[9]+a[13];
  tmp[2] = vertexV[0]*a[2]+vertexV[1]*a[6]+vertexV[2]*a[10]+a[14];
  vertexV[0] = tmp[0];
  vertexV[1] = tmp[1];
  vertexV[2] = tmp[2];
}

void toArray(Point* p, float v[3]){
  v[0] = p->x;
  v[1] = p->y;
  v[2] = p->z;
}

void toPoint(float v[3], Point* p){
  p->x = v[0];
  p->y = v[1];
  p->z = v[2];
}

void buildLL() {
  int i;
  Point* curr;
  Point* last;
  for (i=0; i<35; i++) {
    curr = malloc(sizeof(Point));
    curr->x = controlPoints[i][0];
    curr->y = controlPoints[i][1];
    curr->z = controlPoints[i][2];
    curr->next = NULL;
    if (dP == NULL) {
      dP = curr;
      last = curr;
    }
    else {
      last->next = curr;
      last = curr;
    }
  }
}

void buildCircle(Circle* c, float r, float x, float y, float z, float* m) {
  float theta = M_PI, temp[3];
  int i;
  for(i = 0; i < 7; i++) {
    temp[0] = r * sin(theta) + x;
    temp[1] = r * cos(theta) + y;
    temp[2] = z;
    multVM(m, &temp[0]);
    toPoint(&temp[0], &c->point[i]);
    theta += M_PI * 2 / 6;
  }
}

void buildCurve() {
  Point* currPoint = dP, * p0, * p1, * p2, * p3;
  Curve* currCurve, * lastCurve = NULL;
  Point* pArr;
  while(currPoint != NULL) {
    /* select 4 control points */
    p0 = nextPoint(dP, currPoint);
    p1 = nextPoint(dP, p0);
    p2 = nextPoint(dP, p1);
    p3 = nextPoint(dP, p2);   
    
    /* convert points to bezier control points */
    currCurve = malloc(sizeof(Curve));
    pArr = malloc(sizeof(Point) * 4);   
    pArr[0].x = (p0->x / 6) + (p1->x * 2 / 3) + (p2->x / 6);
    pArr[0].y = (p0->y / 6) + (p1->y * 2 / 3) + (p2->y / 6);
    pArr[0].z = (p0->z / 6) + (p1->z * 2 / 3) + (p2->z / 6);
    pArr[1].x = (p1->x * 2 / 3) + (p2->x / 3);
    pArr[1].y = (p1->y * 2 / 3) + (p2->y / 3);
    pArr[1].z = (p1->z * 2 / 3) + (p2->z / 3);
    pArr[2].x = (p1->x / 3) + (p2->x * 2 / 3);
    pArr[2].y = (p1->y / 3) + (p2->y * 2 / 3);
    pArr[2].z = (p1->z / 3) + (p2->z * 2 / 3);
    pArr[3].x = (p1->x / 6) + (p2->x * 2 / 3) + (p3->x / 6);
    pArr[3].y = (p1->y / 6) + (p2->y * 2 / 3) + (p3->y / 6);
    pArr[3].z = (p1->z / 6) + (p2->z * 2 / 3) + (p3->z / 6);
    
    currCurve->pBez = pArr;
    currCurve->pSub = NULL;
    currCurve->leftR = NULL;
    currCurve->rightR = NULL;
    currCurve->next = NULL;
    currCurve->up = NULL;
    
    if (lastCurve == NULL) {
      lastCurve = currCurve;
      front = currCurve;
    }
    else {
      lastCurve->next = currCurve;
      lastCurve = currCurve;
    }
    currPoint = currPoint->next;
  }
}

void buildRail() {
  Curve* curr = front; 
  Point* currPoint, * next, * nextnext, * lastUpPoint, * upPoint;
  Circle* currCenterCircle, * lastCenterCircle = NULL;
  Circle* currLeftCircle, * lastLeftCircle = NULL;
  Circle* currRightCircle, * lastRightCircle = NULL;
  float p0[3], p1[3], p2[3], temp[3], up[3];
  float d0[3], d1[3], dd0[3];
  float* m, * rm, tempM[16];
  float r, k, tiltVal;
  
  while(curr != NULL) {
    currPoint = curr->pSub;
    
    while (currPoint != NULL){
      up[0] = up[2] = 0;
      up[1] = 1;
      toArray(currPoint, p0);
      next = nextCurvePoint(front, curr, currPoint);
      toArray(next, p1);
      nextnext = nextCurvePoint(front, curr, next);
      toArray(nextnext, p2);
      
      /* tilt controls */
      sub(p1, p0, d0);
      sub(p2, p1, d1);
      sub(d1, d0, dd0);
      r = sqrt(d0[0] * d0[0] + d0[1] * d0[1] + d0[2] * d0[2]);
      if(r < .001) k = 0;
      else k = (d0[2] * dd0[0] - d0[0] * dd0[2]) / (r * r * r);
      tiltVal += k * .03;
      tiltVal *= .09;
      rm = malloc(sizeof(float) * 16);
      rm[2] = rm[3] = rm[6] = rm[7] = rm[8] = rm[9] =rm[11] = 0;
      rm[12] = rm[13] = rm[14] = 0;
      rm[10] = rm[15] = 1;
      rm[0] = rm[5] = cos(tiltVal);
      rm[1] = sin(tiltVal);
      rm[4] = -sin(tiltVal);
      m = lookAt(&p0[0], &p1[0], &up[0]);
      multMM(m, rm, &tempM[0]);
      upPoint = malloc(sizeof(Point));      
      upPoint->x = tempM[4];
      upPoint->y = tempM[5];
      upPoint->z = tempM[6];
      upPoint->next = NULL;
      
      if(curr->up == NULL){
        curr->up = upPoint;
        lastUpPoint = upPoint;
      }
      else{
        lastUpPoint->next = upPoint;
        lastUpPoint = upPoint;
      }
      
      currCenterCircle = malloc(sizeof(Circle));
      toPoint(p0, &currCenterCircle->center);
      buildCircle(currCenterCircle, .15, 0, 0, 0, m);
      currCenterCircle->next = NULL;

      if(lastCenterCircle == NULL){
        curr->centR = currCenterCircle;
        lastCenterCircle = currCenterCircle;
      }
      else{
        lastCenterCircle->next = currCenterCircle;
        lastCenterCircle = currCenterCircle;
      }
      
      currLeftCircle = malloc(sizeof(Circle));
      temp[0] = -.5;
      temp[1] = .3;
      temp[2] = 0;
      multVM(&tempM[0], &temp[0]);
      toPoint(temp, &currLeftCircle->center);
      buildCircle(currLeftCircle, .15, -.5, .3, 0, &tempM[0]);
      currLeftCircle->next = NULL;

      if(lastLeftCircle == NULL){
        curr->leftR = currLeftCircle;
        lastLeftCircle = currLeftCircle;
      }
      else{
        lastLeftCircle->next = currLeftCircle;
        lastLeftCircle = currLeftCircle;
      }

      currRightCircle = malloc(sizeof(Circle));
      temp[0] = .5;
      temp[1] = .3;
      temp[2] = 0;
      multVM(&tempM[0], &temp[0]);
      toPoint(temp, &currRightCircle->center);
      buildCircle(currRightCircle, .15, .5, .3, 0, &tempM[0]);
      currRightCircle->next = NULL;

      if(lastRightCircle == NULL){
        curr->rightR = currRightCircle;
        lastRightCircle = currRightCircle;
      }
      else{
        lastRightCircle->next = currRightCircle;
        lastRightCircle = currRightCircle;
      }
      
      free(m);
      free(rm);
      currPoint = currPoint->next;
    }
    lastCenterCircle = NULL;
    lastLeftCircle = NULL;
    lastRightCircle = NULL;
    curr = curr->next;
  }
}

Circle* nextCurveCircle(Curve* n, Curve* c, Circle* c2, int rType) {
    if (c2->next == NULL){
    switch (rType) {
    case 1:
      return nextCurve(n, c)->centR;
    case 2:
      return nextCurve(n, c)->leftR;
    case 3:
      return nextCurve(n, c)->rightR;
    default:
      return NULL;
    }
  }
  else {
    return c2->next;
  }
}

Point* nextCurvePoint(Curve* n, Curve* c, Point* p) {
  if (p->next == NULL) {
    return nextCurve(n, c)->pSub;
  }
  else {
    return p->next;
  }
}

Point* nextPoint(Point* n, Point* p) {
  if (p->next == NULL) {
    return n;
  }
  else {
    return p->next;
  }
}

Curve* nextCurve(Curve* n, Curve* c) {
  if (c->next == NULL) {
    return n;
  }
  else {
    return c->next;
  }
}

float* midP(float* p1, float* p2) {
  float* result = malloc(sizeof(float)*3);
  result[0] = (p1[0] + p2[0])/2.0;
  result[1] = (p1[1] + p2[1])/2.0;
  result[2] = (p1[2] + p2[2])/2.0;
  return result;
}

void subdivideBez(Curve* curve, int depth, float* p0, float* p1, float* p2, float* p3) {
  float* r0, * r1, * r2, * s0, * s1, * t0, tem[3];
  Point* pp0, * pp1, * pp2;
  float length;
  sub(p1, p0, &tem[0]);
  length = normalize(&tem[0]);

  if (depth >= 6 || length < 1){
    pp0 = malloc(sizeof(Point));
    toPoint(p0, pp0);
    pp1 = malloc(sizeof(Point));
    toPoint(p1, pp1);
    pp2 = malloc(sizeof(Point));
    toPoint(p2, pp2);
    pp0->next = pp1;
    pp1->next = pp2;
    pp2->next = NULL;

    if(tempP == NULL){     
      curve->pSub = pp0;
      tempP = pp2;
    }
    else{
      tempP->next = pp0;
      tempP = pp2;
    }
  }
  else{
    r0 = midP(p0, p1);
    r1 = midP(p1, p2);
    r2 = midP(p2, p3);
    s0 = midP(r0, r1);
    s1 = midP(r1, r2);
    t0 = midP(s0, s1);
    subdivideBez(curve, ++depth, p0, r0, s0, t0);
    subdivideBez(curve, ++depth, t0, s1, r2, p3);

    /* won't need these again, unalloc */
    free(r0);
    free(r1);
    free(r2);
    free(s0);
    free(s1);
    free(t0);
  }
}

void subdivideCurve() {
  Curve* curr = front; 
  float p0[3], p1[3], p2[3], p3[3];
  while (curr != NULL) {
    toArray(&curr->pBez[0], p0);
    toArray(&curr->pBez[1], p1);
    toArray(&curr->pBez[2], p2);
    toArray(&curr->pBez[3], p3);
    
    tempP = NULL;
    subdivideBez(curr, 0, &p0[0], &p1[0], &p2[0], &p3[0]);
    curr = curr->next;
  }
}

