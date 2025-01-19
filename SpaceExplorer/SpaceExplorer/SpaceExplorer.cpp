#include <iostream>
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

int main() {
    // Create an HTTP client
    httplib::Client client("http://jsonplaceholder.typicode.com");

    // Make a GET request
    auto response = client.Get("/posts/1");
    if (response && response->status == 200) {
        // Parse JSON response
        json data = json::parse(response->body);

        // Print parsed JSON data
        std::cout << "Title: " << data["title"] << std::endl;
        std::cout << "Body: " << data["body"] << std::endl;
    }
    else {
        std::cerr << "Error: Unable to fetch data" << std::endl;
    }

    return 0;
}
