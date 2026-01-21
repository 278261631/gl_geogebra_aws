#include "Application.h"
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Starting GeoGebra 3D..." << std::endl;

    try {
        std::cout << "Creating application..." << std::endl;
        Application app;

        std::cout << "Initializing application..." << std::endl;
        if (!app.Initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return -1;
        }

        std::cout << "Running application..." << std::endl;
        app.Run();

        std::cout << "Shutting down..." << std::endl;
        app.Shutdown();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return -1;
    }
}

