#define CPPHTTPLIB_OPENSSL_SUPPORT // Enable SSL/TLS support for httplib

#include <iostream> 
#include <string> 
#include <vector> 
#include <fstream> 
#include <filesystem> 
#include <thread> 
#include <atomic> 

#include "httplib.h" 
#include "json.hpp" 

// ImGui includes
#include "imgui.h"
#include "imgui_impl_glfw.h" 
#include "imgui_impl_opengl3.h" 

// OpenGL and GLFW includes
#include <GLFW/glfw3.h> 

using json = nlohmann::json; 
std::atomic<bool> isFetchingData(false); // Atomic variable to manage data fetching status
std::atomic<bool> dataReady(false); // Ensure data fetching is complete before UI interactions

// Class to handle file writing
class FileWriter {
public:
    static void writeToFile(const std::string& content) { // Method to write content to a file
        std::cout << "Attempting to write to: " << std::filesystem::current_path() << "/output.txt" << std::endl; 

        std::ofstream outFile("output.txt", std::ios::app); // Open in append mode
        if (outFile.is_open()) { 
            outFile << content << std::endl; 
            outFile.close(); 
            std::cout << "File written successfully." << std::endl; 
        }
        else {
            std::cerr << "Error: Could not open file for writing!" << std::endl; 
        }
    }
};

// Planet class to store planetary information
class Planet {
public:
    std::string name; 
    double radius; 
    double distanceFromSun; 

    void loadFromJson(const json& data) { // Method to load data from JSON
        name = data.contains("englishName") && data["englishName"].is_string() ? data["englishName"].get<std::string>() : "Unknown Planet"; // Extract name
        radius = data.contains("meanRadius") && data["meanRadius"].is_number() ? data["meanRadius"].get<double>() : 0.0; // Extract radius
        distanceFromSun = data.contains("semimajorAxis") && data["semimajorAxis"].is_number() ? data["semimajorAxis"].get<double>() / 1e6 : 0.0; // Extract distance
    }
};

// Mission class to store space mission information
class Mission {
public:
    std::string name; 
    std::string objective; 
    int year; 

    void loadFromJson(const json& data) { // Method to load data from JSON
        name = data.contains("name") && data["name"].is_string() ? data["name"].get<std::string>() : "Unknown Mission"; 
        objective = data.contains("details") && data["details"].is_string() ? data["details"].get<std::string>() : "No details available"; 
        year = data.contains("date_utc") && data["date_utc"].is_string()
            ? stoi(data["date_utc"].get<std::string>().substr(0, 4)) : 0; 
    }
};

// Astronaut class to store astronaut information
class Astronaut {
public:
    std::string name; // Name of the astronaut
    std::string craft; // Spacecraft the astronaut is currently on

    void loadFromJson(const json& person) { // Method to load data from JSON
        name = person.contains("name") && person["name"].is_string() ? person["name"].get<std::string>() : "Unknown Astronaut"; // Extract name
        craft = person.contains("craft") && person["craft"].is_string() ? person["craft"].get<std::string>() : "Unknown Craft"; // Extract craft
    }
};

// Astronomy Picture of the Day (APOD) class
class AstronomyPicture {
public:
    std::string title; // Title of the picture
    std::string explanation; // Explanation of the picture
    std::string url; // URL of the picture

    void loadFromJson(const json& data) { // Method to load data from JSON
        title = data.contains("title") && data["title"].is_string() ? data["title"].get<std::string>() : "Unknown Title"; // Extract title
        explanation = data.contains("explanation") && data["explanation"].is_string() ? data["explanation"].get<std::string>() : "No explanation available"; // Extract explanation
        url = data.contains("url") && data["url"].is_string() ? data["url"].get<std::string>() : "No URL available"; // Extract URL
    }
};

// Function to fetch Planet data
void fetchPlanetData(Planet& earth) { 
    httplib::SSLClient planetClient("api.le-systeme-solaire.net"); // Create SSL client for planet API
    auto planetResponse = planetClient.Get("/rest/bodies/terre"); // Send GET request
    if (planetResponse && planetResponse->status == 200) { // Check if response is successful
        json planetData = json::parse(planetResponse->body); 
        earth.loadFromJson(planetData); 
    }
}

// Function to fetch SpaceX Mission data
void fetchMissionData(Mission& latestMission) { 
    httplib::SSLClient missionClient("api.spacexdata.com"); // Create SSL client for SpaceX API
    auto missionResponse = missionClient.Get("/v4/launches/latest"); // Send GET request
    if (missionResponse && missionResponse->status == 200) {
        json missionData = json::parse(missionResponse->body); 
        latestMission.loadFromJson(missionData); 
    }
}

// Function to fetch Astronaut data
void fetchAstronautData(std::vector<Astronaut>& astronauts) { 
    httplib::Client astronautClient("http://api.open-notify.org"); // Create HTTP client for astronaut API
    auto astronautResponse = astronautClient.Get("/astros.json"); 

    if (astronautResponse && astronautResponse->status == 200 && !astronautResponse->body.empty()) { 
        try {
            json astronautData = json::parse(astronautResponse->body); 
            for (const auto& person : astronautData["people"]) { 
                Astronaut astronaut;
                astronaut.loadFromJson(person); 
                astronauts.push_back(astronaut); 
            }
        }
        catch (const json::exception& e) { 
            std::cerr << "Astronaut JSON Parsing Error: " << e.what() << std::endl; 
        }
    }
    else {
        std::cerr << "Failed to fetch astronaut data!" << std::endl; 
    }
}

// Function to fetch NASA APOD data
void fetchAPODData(AstronomyPicture& apod, const std::string& apiKey) { 
    httplib::SSLClient nasaClient("api.nasa.gov"); // Create SSL client for NASA API
    auto nasaResponse = nasaClient.Get(("/planetary/apod?api_key=" + apiKey).c_str()); 

    if (nasaResponse && nasaResponse->status == 200 && !nasaResponse->body.empty()) { 
        try {
            json nasaData = json::parse(nasaResponse->body); // Parse JSON response
            apod.loadFromJson(nasaData); // Load data into APOD object
        }
        catch (const json::exception& e) { // Catch JSON parsing errors
            std::cerr << "APOD JSON Parsing Error: " << e.what() << std::endl; // Error message
        }
    }
    else {
        std::cerr << "Failed to fetch APOD data!" << std::endl; // Error message
    }
}

int main() { // Main function
    const std::string apiKey = "DQaiSGm528HhbknR2AnA3GsCXxeg0g9a6XLImVgh"; // NASA API key

    // Initialize data objects
    Planet earth; // Earth object
    Mission latestMission; // Latest mission object
    std::vector<Astronaut> astronauts; // Vector of astronauts
    AstronomyPicture apod; // APOD object

    // Start threads for parallel data fetching
    std::thread planetThread(fetchPlanetData, std::ref(earth));
    std::thread missionThread(fetchMissionData, std::ref(latestMission)); 
    std::thread astronautThread(fetchAstronautData, std::ref(astronauts)); 
    std::thread apodThread(fetchAPODData, std::ref(apod), apiKey); 

    // Wait for threads to complete
    planetThread.join(); // Wait for planet thread
    missionThread.join(); // Wait for mission thread
    astronautThread.join(); // Wait for astronaut thread
    apodThread.join(); // Wait for APOD thread

    // Set data as ready
    dataReady.store(true); // Set atomic flag to true

    // Initialize GLFW
    if (!glfwInit()) { // Initialize GLFW
        std::cerr << "Error initializing GLFW!" << std::endl; 
        return -1; 
    }

    // Configure GLFW for OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set OpenGL major version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Set OpenGL minor version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set OpenGL profile

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "SpaceExplorer - ImGui Integration", nullptr, nullptr); // Create GLFW window
    if (!window) { // Check if window creation failed
        std::cerr << "Error creating GLFW window!" << std::endl; 
        glfwTerminate(); // Terminate GLFW
        return -1; 
    }

    glfwMakeContextCurrent(window); // Create and manage OpenGL contexts and windows.

    // Initialize ImGui context
    IMGUI_CHECKVERSION(); // Check ImGui version
    ImGui::CreateContext(); // Create ImGui context
    ImGuiIO& io = ImGui::GetIO(); // Get ImGui IO object
    (void)io; // Suppress unused variable warning

    // Set up ImGui GLFW and OpenGL3 bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize ImGui GLFW bindings
    ImGui_ImplOpenGL3_Init("#version 330"); // Initialize ImGui OpenGL3 bindings

    // Style ImGui
    ImGui::StyleColorsDark(); // Set ImGui style to dark mode

    char astronautSearch[50] = ""; 

    // Main loop
    bool showContent = false; 
    std::string displayedContent; // String to store displayed content

    bool showAstronautSearch = false; 
    bool showViewImageButton = false; 

    while (!glfwWindowShouldClose(window)) { 
        // Poll events
        glfwPollEvents(); 

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame(); 
        ImGui_ImplGlfw_NewFrame(); 
        ImGui::NewFrame(); 

        // Create ImGui window
        ImGui::Begin("Space Explorer"); // Begin ImGui window

        if (ImGui::Button("Planet Data")) { 
            displayedContent = "Name: " + earth.name +
                "\nRadius: " + std::to_string(earth.radius) + " km" +
                "\nDistance from Sun: " + std::to_string(earth.distanceFromSun) + " million km"; // Set content
            showContent = true; 
            showAstronautSearch = false; 
            showViewImageButton = false; 
        }

        if (ImGui::Button("Latest SpaceX Mission")) { 
            displayedContent = "Name: " + latestMission.name +
                "\nObjective: " + latestMission.objective +
                "\nYear: " + std::to_string(latestMission.year); 
            showContent = true; 
            showAstronautSearch = false; 
            showViewImageButton = false; 
        }

        if (ImGui::Button("NASA Astronomy Picture of the Day")) {
            displayedContent = "NASA Astronomy Picture of the Day\n\n"; 
            displayedContent += "Title: " + apod.title + "\n"; 
            displayedContent += "Explanation: " + apod.explanation + "\n"; 

            if (!apod.url.empty()) { 
                displayedContent += "Image URL: " + apod.url + "\n"; 
                showViewImageButton = true;  
            }

            showContent = true; 
            showAstronautSearch = false; 
        }

        if (showViewImageButton) { // Check if view image button should be shown
            if (ImGui::Button("View Image")) { // Button to view image
                if (!apod.url.empty()) { // Check if URL is available
#ifdef _WIN32
                    ShellExecuteA(0, "open", apod.url.c_str(), 0, 0, SW_SHOWNORMAL); // Open URL on Windows
#else
                    system(("xdg-open " + apod.url).c_str()); // Open URL on Linux/Mac
#endif
                }
            }
        }

        if (ImGui::Button("Astronauts")) { 
            displayedContent = "Astronauts Currently in Space:\n"; 
            for (const auto& astronaut : astronauts) { 
                displayedContent += astronaut.name + " (" + astronaut.craft + ")\n"; 
            }
            showContent = true; // Show content
            showAstronautSearch = true;  // Show search bar
            showViewImageButton = false; // Hide view image button
        }

        if (showAstronautSearch) { // Check if astronaut search bar should be shown
            if (ImGui::InputText("Search Astronaut", astronautSearch, IM_ARRAYSIZE(astronautSearch))) { // Input text for search
                displayedContent = "Astronauts Currently in Space:\n"; // Reset content
                for (const auto& astronaut : astronauts) { // Iterate through astronauts
                    if (strlen(astronautSearch) == 0 || strstr(astronaut.name.c_str(), astronautSearch)) { // Check search filter
                        displayedContent += astronaut.name + " (" + astronaut.craft + ")\n"; // Add astronaut info
                    }
                }
            }
        }

        if (showContent) { // Check if content should be shown
            ImGui::Separator(); // Add separator
            ImGui::TextWrapped("%s", displayedContent.c_str()); // Display content
        }

        ImGui::End(); 

        // Rendering
        ImGui::Render(); 
        int display_w, display_h; // Variables for display dimensions
        glfwGetFramebufferSize(window, &display_w, &display_h); // Get framebuffer size
        glViewport(0, 0, display_w, display_h); 
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT); // Clear color buffer
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
        glfwSwapBuffers(window); 
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown(); // Shutdown ImGui OpenGL3
    ImGui_ImplGlfw_Shutdown(); // Shutdown ImGui GLFW
    ImGui::DestroyContext(); // Destroy ImGui context
    glfwDestroyWindow(window); // Destroy GLFW window
    glfwTerminate(); // Terminate GLFW

    return 0; 
}