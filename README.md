# Win32 API window/class Simplification

This project simplifies window creation and class registration using Win32 API.

**Note**: This project is for learning purposes only. It is not intended for widespread use as it lacks many of the functionalities of the Win32 API.

## Features

- Converts a custom hierarchical structure (similar to CSS) into Win32 API code.
- Loads resources (e.g., icon, cursor) from files in a resources folder.

## Requirements

- A C++ Compiler (GCC/Clang for Linux/macOS, Visual Studio for Windows).

## Installation and Usage

1. **Clone the Repository**
    ```bash
    git clone https://github.com/Spiderman62626/Win32-API-class-window-simplification.git
    cd Win32-API-class-window-simplification
    ```
2. **Compile the Project**
    **Using GCC or Clang**:
    ```bash
    g++ -o app main.cpp -mwindows -municode -DUNICODE -D_UNICODE -lstdc++
    ```
    **Using Visual Studio**:
    - Open the project in Visual Studio.
    - Select the appropriate build configuration (e.g., `x64` or `x86`).
    - Build the solution.
3. **Run the Project**
    On Linux/macOS:
    ```bash
    ./app
    ```
    On Windows:
    ```bash
    app.exe
    ```

## Additional Notes

- Ensure that all resource files are in the correct resources folder for the project to work properly.
- This is a simplified version of the Win32 API and does not cover all of the Win32 API's functionalities.