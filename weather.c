#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// Function to write the response data into a string
size_t WriteCallback(void *contents, size_t size, size_t nmemb, char *output) {
    size_t realsize = size * nmemb;
    strncat(output, contents, realsize);
    return realsize;
}

int main() {
    // Your OpenWeatherMap API key and city
    const char *api_key = "your_api_key_here";
    const char *city = "your_city_here";
    char url[256];

    // Construct the URL for the API request
    snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric", city, api_key);

    // Initialize libcurl
    CURL *curl;
    CURLcode res;
    char response[4096] = ""; // Buffer to hold the response data

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the callback function to write the response to
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        // Perform the HTTP GET request
        res = curl_easy_perform(curl);

        // Check for errors in the request
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse the JSON response
            cJSON *json = cJSON_Parse(response);
            if (json == NULL) {
                printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
                return 1;
            }

            // Extract the relevant data from the JSON response
            cJSON *main = cJSON_GetObjectItemCaseSensitive(json, "main");
            cJSON *weather = cJSON_GetArrayItem(cJSON_GetObjectItemCaseSensitive(json, "weather"), 0);
            cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");

            if (main != NULL && weather != NULL && name != NULL) {
                double temperature = cJSON_GetObjectItemCaseSensitive(main, "temp")->valuedouble;
                double temp_min = cJSON_GetObjectItemCaseSensitive(main, "temp_min")->valuedouble;
                int humidity = cJSON_GetObjectItemCaseSensitive(main, "humidity")->valueint;
                const char *description = cJSON_GetObjectItemCaseSensitive(weather, "description")->valuestring;
                const char *city_name = name->valuestring;

                // Display the weather information
                printf("\n\n");
                printf("Weather in %s:\n", city_name);
                printf("Current Temperature: %.2f°C\n", temperature);
                printf("Min Temperature: %.2f°C\n", temp_min);
                printf("Humidity: %d%%\n", humidity);
                printf("Description: %s\n", description);
            } else {
                printf("Failed to extract data from JSON.\n");
            }

            // Cleanup the JSON object
            cJSON_Delete(json);
        }

        // Cleanup libcurl
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}
