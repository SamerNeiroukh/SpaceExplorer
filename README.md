# SpaceExplorer Fetcher

## Overview

SpaceExplorer is an advanced C++ project that integrates multi-threading, HTTP API communication, and a graphical user interface using ImGui. The application fetches and displays real-time data from various space-related APIs, including planetary data, space missions, astronauts, and NASA's Astronomy Picture of the Day (APOD). It presents this information in a user-friendly GUI.

## Features

- **Multi-threaded Data Fetching**: Uses `std::thread` and `std::atomic` for concurrent API requests.
- **HTTP API Integration**: Fetches data from:
  - [Le Système Solaire API](https://api.le-systeme-solaire.net/)
  - [SpaceX API](https://github.com/r-spacex/SpaceX-API)
  - [Open Notify API (Astronauts)](http://api.open-notify.org/astros.json)
  - [NASA API (APOD)](https://api.nasa.gov/)
- **Graphical User Interface (GUI)**: Implements **ImGui** for a modern and interactive UI.
- **File Management**: Stores data locally using `fstream` and `filesystem`.
- **Search Functionality**: Allows real-time searching for astronauts.
- **Image Viewing**: Displays NASA's APOD images directly within the application.
  
## Technologies Used

* C++17: Core programming language.
* Standard Template Library (STL):  Utilized vector, unordered_map, fstream, and filesystem.
* httplib:  Facilitates HTTP requests.
* nlohmann/json:  Parses and processes JSON responses.
* OpenGL & GLFW: Used for rendering.
* Standard Template Library (STL):  Utilized vector, unordered_map, fstream, and filesystem.
* Multi-threading: Implemented using std::thread and std::atomic.
* ImGui: Provides an interactive GUI.


## Getting Started

### Prerequisites

* C++ Compiler: Supporting C++17 or later (e.g., g++, clang++, Visual Studio 2019+).
* CMake:  For building the project across different platforms.
* OpenGL: for rendering.
* Visual Studio: 2022 (for Windows).
* GLFW: for window management.
* ImGui : for GUI components.
* ImGui : for GUI components.

### Installation

1. Clone the Repository:

   bash
   git clone https://github.com/SamerNeiroukh/SpaceExplorer.git
   
   cd SpaceExplorer
   

3. Install dependencies (Ensure you have necessary libraries):

    * On Ubuntu:
      bash
      sudo apt install libglfw3-dev libglm-dev libglew-dev
      

    * On Windows (using vcpkg):
      bash
      vcpkg install glfw3 glm glew
      
4. Build the project:
    bash
    mkdir build && cd build
    cmake ..
    make 
    

5. Run the application: ./SpaceExplorer

## Project Structure


SpaceExplorer/

├── SpaceExplorer/               # Core application code
│   ├── main.cpp                 # Entry point
│   ├── file_writer.h            # Handles file writing
│   ├── planet.h                 # Planet class
│   ├── mission.h                # Space mission class
│   ├── astronaut.h              # Astronaut class
│   ├── astronomy_picture.h       # NASA APOD class
│   ├── fetch_data.h              # API data fetch logic
│   ├── gui.h                     # GUI rendering logic
│   ├── utils.h                   # Utility functions
│   ├── SpaceExplorer.sln         # Visual Studio Solution
│   ├── x64/Debug/                # Compiled binary directory
├── external/                     # Third-party libraries
│   ├── glfw/                     # OpenGL GLFW library
│   ├── imgui/                    # ImGui library for GUI
│   ├── httplib.h                 # HTTP requests library
│   ├── json.hpp                   # JSON parsing library
└── README.md                     # Project documentation


## Multi-threading Architecture

The application uses multi-threading for concurrent API requests:

* Main Thread: Handles GUI rendering and event processing.

* fetchPlanetData(): Fetches planetary data.
* fetchMissionData(): Fetches the latest SpaceX mission.
* fetchAstronautData():  Retrieves a list of astronauts in space.
* fetchAPODData():  Fetches NASA's Astronomy Picture of the Day.


## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bug reports, feature requests, or suggestions.# SpaceExplorer

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contact

For any inquiries or feedback, please reach out to:

**Authors**: Samer Neiroukh, Mahmoud faroun

**GitHub**: [@SamerNeiroukh](https://github.com/SamerNeiroukh), [@mahmoudfiron](https://github.com/mahmoudfiron)

**Email`s**:  samer.neiroukh@gmail.com , ‫mahmoudfiron13@gmail.com
