#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Window dimensions
int width = 800;
int height = 600;

// Atom characteristics
float nucleusRadius = 0.1f;
float electronRadius = 0.05f;
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

// Function to initialize OpenGL settings
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Background color (black)
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

// Function to draw text on the screen
void drawText(const char *text, float x, float y) {
    glColor3f(1.0, 1.0, 1.0); // Set text color to white
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

// Function to render the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen and depth buffer
    glLoadIdentity();                                   // Reset the modelview matrix

    // Fixed camera view (2D orthographic)
    glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0); // Set up an orthographic projection

    // Draw the nucleus at the center
    glPushMatrix();
    drawSphere(nucleusRadius, 1.0, 0.0, 0.0); // Red color for the nucleus
    glPopMatrix();

    // Draw electrons based on electron configuration
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // Get the elapsed time
    float currentOrbitRadius = baseOrbitRadius;

    int remainingElectrons = numElectrons;
    for (int shell = 0; shell < 7 && remainingElectrons > 0; shell++) {
        int electronsInThisShell = (remainingElectrons > maxElectronsInShells[shell]) ? maxElectronsInShells[shell] : remainingElectrons;

        for (int i = 0; i < electronsInThisShell; i++) {
            float angle = time + (2 * M_PI / electronsInThisShell) * i;
            float x = currentOrbitRadius * cos(angle);
            float y = currentOrbitRadius * sin(angle);

            // Draw the current electron position
            glPushMatrix();
            glTranslatef(x, y, 0.0f);
            drawSphere(electronRadius, 1.0, 1.0, 0.0); // Yellow color for the electrons
            glPopMatrix();
        }

        currentOrbitRadius += 0.5f; // Increase orbit radius for next shell
        remainingElectrons -= electronsInThisShell;
    }

    // Draw element name
    drawText(elementName, -1.5f, 1.5f); // Adjusted position for better visibility

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

// Timer function to handle delay
void timer(int value) {
    if (inputInProgress) {
        // Check if 2 seconds have passed since the last input
        unsigned long currentTime = glutGet(GLUT_ELAPSED_TIME);
        if (currentTime - lastInputTime >= inputDelay) {
            // 2 seconds have passed, allow new input
            inputInProgress = false;
        }
    }
    glutTimerFunc(100, timer, 0); // Call timer function every 100ms
}

// Function to handle keyboard input
void keyboard(unsigned char key, int x, int y) {
    if (key >= '0' && key <= '9') {
        unsigned long currentTime = glutGet(GLUT_ELAPSED_TIME);
        
        // Check if delay period has elapsed since last digit input
        if (currentTime - lastDigitTime < inputDelay && isInputtingNumber) {
            // If not enough time has passed, consider it as part of the current number input
            currentInputNumber = currentInputNumber * 10 + (key - '0');
        } else {
            // If enough time has passed, treat it as a new input
            currentInputNumber = (key - '0');
            isInputtingNumber = true;
        }
        
        lastDigitTime = currentTime; // Update the time of last digit input

        if (currentInputNumber > 118) {
            currentInputNumber = 118; // Limit to the maximum number of elements
        }

        numElectrons = currentInputNumber;

        // Set element name based on the atomic number
        if (currentInputNumber > 0 && currentInputNumber <= 118) {
            strcpy(elementName, elementNames[currentInputNumber - 1]);
        } else {
            strcpy(elementName, "Unknown");
        }

        lastInputTime = currentTime; // Update the time of last input
        inputInProgress = true; // Start the delay period
        glutPostRedisplay(); // Request a redraw
    } else if (key == 'a' || key == 'A') { // Reset to Hydrogen
        numElectrons = 1;
        strcpy(elementName, "Hydrogen");
        currentInputNumber = 1;
        isInputtingNumber = false;
        glutPostRedisplay(); // Request a redraw
    } else if (key == 27) { // ESC key to reset input
        isInputtingNumber = false;
        currentInputNumber = 0;
    }
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);                           // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Double buffering, RGB color, depth buffer
    glutInitWindowSize(width, height);               // Set the window size
    glutCreateWindow("2D Atom Simulator");           // Create the window with title

    init();                                          // Initialize OpenGL settings
    glutDisplayFunc(display);                        // Register display callback function
    glutReshapeFunc(reshape);                        // Register reshape callback function
    glutKeyboardFunc(keyboard);                      // Register keyboard callback function
    glutTimerFunc(100, timer, 0);                    // Initialize timer
    glutMainLoop();                                  // Enter the main event loop
    return 0;
}
