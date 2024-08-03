#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Window dimensions
int width = 800;
int height = 600;

// Atom characteristics
float nucleusRadius = 0.2f;
float electronRadius = 0.07f;
float baseOrbitRadius = 1.0f; // Adjusted for visibility

// Electron configuration for each shell
int maxElectronsInShells[7] = {2, 8, 18, 32, 32, 18, 8}; // Maximum electrons per shell
int numElectrons = 1;
char elementName[20] = "Hydrogen";

// Full list of elements and their names
const char* elementNames[118] = {
    "Hydrogen", "Helium", "Lithium", "Beryllium", "Boron", "Carbon", "Nitrogen", "Oxygen",
    "Fluorine", "Neon", "Sodium", "Magnesium", "Aluminum", "Silicon", "Phosphorus", "Sulfur",
    "Chlorine", "Argon", "Potassium", "Calcium", "Scandium", "Titanium", "Vanadium", "Chromium",
    "Manganese", "Iron", "Cobalt", "Nickel", "Copper", "Zinc", "Gallium", "Germanium",
    "Arsenic", "Selenium", "Bromine", "Krypton", "Rubidium", "Strontium", "Yttrium", "Zirconium",
    "Niobium", "Molybdenum", "Technetium", "Ruthenium", "Rhodium", "Palladium", "Silver", "Cadmium",
    "Indium", "Tin", "Antimony", "Tellurium", "Iodine", "Xenon", "Cesium", "Barium",
    "Lanthanum", "Cerium", "Praseodymium", "Neodymium", "Promethium", "Samarium", "Europium", "Gadolinium",
    "Terbium", "Dysprosium", "Holmium", "Erbium", "Thulium", "Ytterbium", "Lutetium", "Hafnium",
    "Tantalum", "Tungsten", "Rhenium", "Osmium", "Iridium", "Platinum", "Gold", "Mercury",
    "Thallium", "Lead", "Bismuth", "Polonium", "Astatine", "Radon", "Francium", "Radium",
    "Actinium", "Thorium", "Protactinium", "Uranium", "Neptunium", "Plutonium", "Americium", "Curium",
    "Berkelium", "Californium", "Einsteinium", "Fermium", "Mendelevium", "Nobelium", "Lawrencium",
    "Rutherfordium", "Dubnium", "Seaborgium", "Bohrium", "Hassium", "Meitnerium", "Darmstadtium",
    "Roentgenium", "Copernicium", "Nihonium", "Flerovium", "Moscovium", "Livermorium", "Tennessine", "Oganesson"
};

// Global variables
int currentInputNumber = 0;
bool isInputtingNumber = false;
bool inputInProgress = false;
unsigned long lastInputTime = 0;
unsigned long lastDigitTime = 0;
const unsigned long inputDelay = 2000; // 2 seconds delay for each digit

// Global variable to track electron movement state
bool electronsMoving = false;

// Global variable for error messages
char errorMessage[50] = "";

// Function to initialize OpenGL settings
void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // Background color (white)
    glEnable(GL_DEPTH_TEST);          // Enable depth testing for 3D rendering
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

// Function to draw a sphere (used for nucleus and electrons)
void drawSphere(float radius, float r, float g, float b) {
    glColor3f(r, g, b); // Set the color
    glutSolidSphere(radius, 50, 50); // Draw the sphere
}

// Function to draw an orbit as a thin line
void drawOrbit(float radius) {
    glColor3f(0.5, 0.5, 0.5); // Gray color for the orbit
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; ++i) {
        float angle = 2 * M_PI * i / 100;
        glVertex2f(radius * cos(angle), radius * sin(angle));
    }
    glEnd();
}

// Function to draw text on the screen
void drawText(const char *text, float x, float y) {
    glColor3f(0.0, 0.0, 0.0); // Set text color to black
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

void subMenu(int option) {
    // If the user selects option 1, stop the electrons from moving.
    if (option == 1){
        electronsMoving = false;
    }
    // If the user selects option 2, start the electrons moving.
    if (option == 2) {
        electronsMoving = true;
    }
    // Redisplay the OpenGL window to reflect the changes.
    glutPostRedisplay();
}


void mainMenu(int option) {
    // Switch statement to handle different menu options
    // Option 1 does nothing, so we add a semicolon to not execute any code for this option.
    if (option == 1) ;

    // Option 2 exits the program.
    // The exit() function is used to terminate the program with a status code of 0.
    if (option == 2) {
        // Exit the program.
        exit(0);
    }
    // Redisplay the OpenGL window to reflect the changes.
    glutPostRedisplay();
}

// Function to render the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen and depth buffer
    glLoadIdentity();                                   // Reset the modelview matrix

    // Fixed camera view (2D orthographic)
    glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0); // Set up an orthographic projection
    
    // Draw the nucleus at the center
    glPushMatrix();
    drawText(elementName, -0.3f, 0.3f);
    drawSphere(nucleusRadius, 1.0, 0.0, 0.0); // Red color for the nucleus
    glPopMatrix();

    // Draw electrons based on electron configuration
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Get the elapsed time
    float currentOrbitRadius = baseOrbitRadius;

    int remainingElectrons = numElectrons;
    for (int shell = 0; shell < 7 && remainingElectrons > 0; shell++) {
        int electronsInThisShell = (remainingElectrons > maxElectronsInShells[shell]) ? maxElectronsInShells[shell] : remainingElectrons;

        // Draw the orbital line
        drawOrbit(currentOrbitRadius);

        for (int i = 0; i < electronsInThisShell; i++) {
            float angle;
            if (electronsMoving) {
                angle = time * 0.9 + (2 * M_PI / electronsInThisShell) * i; // Speed up rotation
            } else {
                angle = (2 * M_PI / electronsInThisShell) * i; // Fixed position
            }
            float x = currentOrbitRadius * cos(angle);
            float y = currentOrbitRadius * sin(angle);

            // Draw the current electron position
            glPushMatrix();
            glTranslatef(x, y, 0.0f);
            drawSphere(electronRadius, 1.0, 1.0, 0.0); // Yellow color for the electrons
            glPopMatrix();
        }

        currentOrbitRadius += 0.4f; // Increase orbit radius for next shell
        remainingElectrons -= electronsInThisShell;
    }

    // Draw error message if there is one
    if (strlen(errorMessage) > 0) {
        drawText(errorMessage, -0.5f, -0.5f); // Adjusted position for better visibility
    }

    glutSwapBuffers(); // Swap the buffers
}

// Function to handle window resizing
void reshape(int w, int h) {
    glViewport(0, 0, w, h);                          // Set the viewport
    glMatrixMode(GL_PROJECTION);                     // Switch to projection matrix
    glLoadIdentity();                                // Reset the projection matrix
    glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0);       // Orthographic projection
    glMatrixMode(GL_MODELVIEW);                      // Switch back to modelview matrix
}

// Function to handle idle state (called when no other events are pending)
void idle() {
    glutPostRedisplay(); // Request a redraw of the window
}

// Function to handle keyboard input
void keyboard(unsigned char key, int x, int y) {
    if (key >= '0' && key <= '9') {
        unsigned long currentTime = glutGet(GLUT_ELAPSED_TIME);
        
        if (key == 0) {
            strncpy(errorMessage, "Invalid Input", sizeof(errorMessage) - 1);
            errorMessage[sizeof(errorMessage) - 1] = '\0'; // Ensure null-termination
            numElectrons = 0; // Reset or handle as needed
            currentInputNumber = 0;
            isInputtingNumber = true;
        } else {
            if (currentTime - lastDigitTime < inputDelay && isInputtingNumber) {
                currentInputNumber = currentInputNumber * 10 + (key - '0');
            } else {
                currentInputNumber = (key - '0');
                isInputtingNumber = true;
            }

            lastDigitTime = currentTime;

            if (currentInputNumber > 118) {
                strncpy(errorMessage, "Invalid Input", sizeof(errorMessage) - 1);
            errorMessage[sizeof(errorMessage) - 1] = '\0'; // Ensure null-termination
            numElectrons = 0; // Reset or handle as needed
            currentInputNumber = 0;
            isInputtingNumber = true;
            }

            numElectrons = currentInputNumber;

            if (numElectrons > 0) {
                strncpy(elementName, elementNames[numElectrons - 1], sizeof(elementName) - 1);
                elementName[sizeof(elementName) - 1] = '\0'; // Ensure null-termination
                strncpy(errorMessage, "", sizeof(errorMessage)); // Clear error message
            } else {
                strncpy(elementName, "", sizeof(elementName)); // Clear element name
                strncpy(errorMessage, "Invalid Input", sizeof(errorMessage) - 1);
                errorMessage[sizeof(errorMessage) - 1] = '\0'; // Ensure null-termination
            }

            printf("Element: %s, Atomic Number: %d\n", elementName, numElectrons);
            lastInputTime = currentTime;
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);                      // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Set display mode
    glutInitWindowSize(width, height);          // Set window size
    glutCreateWindow("Atom Visualization");     // Create the window

    init();                                     // Initialize OpenGL settings
  
    glutDisplayFunc(display);                   // Register display callback
    int SubMenu = glutCreateMenu(subMenu);
    glutAddMenuEntry("Fixed", 1);
    glutAddMenuEntry("Moving", 2);

    // Create the main menu and add its options.
    glutCreateMenu(mainMenu);
    glutAddSubMenu("Movement", SubMenu);
    glutAddMenuEntry("Exit", 2);
     glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutReshapeFunc(reshape);                   // Register reshape callback
    glutIdleFunc(idle);                        // Register idle callback
    glutKeyboardFunc(keyboard);                 // Register keyboard callback

    glutMainLoop();                             // Enter GLUT main loop
    return 0;
}
