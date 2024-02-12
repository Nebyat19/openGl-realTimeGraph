#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <cstring>
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int UPDATE_INTERVAL_MS = 100;  

int serial_port; 
float oldx[10000];
float oldy[10000];
int lastindex = 0;

void initSerialPort() {
    serial_port = open("/dev/ttyACM0", O_RDWR);
    if (serial_port == -1) {
        std::cerr << "Error: Could not open serial port." << std::endl;
        exit(1);
    }

    struct termios tty;
    tcgetattr(serial_port, &tty);
    tty.c_cflag = B9600 | CS8 | CREAD | CLOCAL; 
    tcsetattr(serial_port, TCSANOW, &tty);
}

void readSerialData() {
    char buf[255]; 
    int n = read(serial_port, buf, sizeof(buf));
    if (n > 0) {
        std::string data(buf, n);
        size_t pos = data.find('\n');
        if (pos != std::string::npos) {
            std::string line = data.substr(0, pos);
            try {
                float value = std::stof(line);
                value*=2;
                oldx[lastindex] = lastindex; // x-coordinate
                oldy[lastindex] = value;     // y-coordinate
                lastindex++;
            } catch (const std::invalid_argument& e) {
              
            }
        }
    }
}

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (size_t i = 0; i < strlen(text); ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
}

void drawSinGraph() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    const char* text = "DATA FROM LDR SENSOR";
    drawText(text, 0.4f, 0.9f);

    glBegin(GL_LINE_STRIP);
    
    glColor3f(0.0f, 1.0f, 0.2f); 
    for (int i = 0; i < lastindex; i++) {
        glVertex2f((oldx[i] / lastindex)-0.05f, (oldy[i] / 1000.0f)+0.3f); 

    }
    glEnd();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    readSerialData();
    drawSinGraph();
    glutTimerFunc(UPDATE_INTERVAL_MS, update, 0);
}

int main(int argc, char** argv) {
    initSerialPort();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("REAL TIME Graph");

    glutDisplayFunc(drawSinGraph);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);  

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    glutMainLoop();

    
    close(serial_port);

    return 0;
}
