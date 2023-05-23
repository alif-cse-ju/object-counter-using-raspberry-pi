/// This program sends number of objects passed last hour through mail

#include <ctime>
#include <curl.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <wiringPi.h>

using namespace std;

// GPIO pin number connected to the object detecting sensor output
#define SENSOR_PIN 0


// Email configuration
const string SMTP_SERVER = "your_smtp_server";
const int SMTP_PORT = 587;
const string SMTP_USERNAME = "your_email_username";
const string SMTP_PASSWORD = "your_email_password";
const string SENDER_EMAIL = "sender_email@example.com";
const string RECIPIENT_EMAIL = "recipient_email@example.com";



// Function to send the email using CURL
int sendEmail(const string& subject, const string& body)
{
    CURL* curl;
    CURLcode res = CURLE_OK;
    string curlErrorBuffer;

    // Initialize CURL
    curl = curl_easy_init();
    if (curl)
    {
        // Set the SMTP server and port
        curl_easy_setopt(curl, CURLOPT_URL, (SMTP_SERVER + ":" + to_string(SMTP_PORT)).c_str());

        // Enable STARTTLS
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        // Set the sender email address
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, SENDER_EMAIL.c_str());

        // Set the recipient email address
        struct curl_slist* recipients = NULL;
        recipients = curl_slist_append(recipients, RECIPIENT_EMAIL.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Set the email subject and body
        string emailData = "Subject: " + subject + "\r\n\r\n" + body;
        curl_easy_setopt(curl, CURLOPT_READDATA, emailData.c_str());

        // Set the username and password for authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, SMTP_USERNAME.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, SMTP_PASSWORD.c_str());

        // Set the callback function for sending the email
        curl_easy_setopt(curl, CURLOPT_MAIL_AUTH, NULL);

        // Perform the email sending
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) curlErrorBuffer = curl_easy_strerror(res);

        // Clean up
        curl_easy_cleanup(curl);
    }
    else
    {
        curlErrorBuffer = "Failed to initialize CURL";
        res = CURLE_FAILED_INIT;
    }

    if (res != CURLE_OK) cout << "Failed to send email. Error: " << curlErrorBuffer << endl;

    return (int)res;
}


int main()
{

    int objectCount = 0;
    bool isDetected = 0;

    const string subject = "Object Count Update";

    if(wiringPiSetup() == -1)cout << "Failed to initialize wiringPi" << endl;
    else
    {
        // Setting up GPIO pin
        pinMode(SENSOR_PIN, INPUT);

        // Loop to check 24/7
        while(1)
        {
            // Read the sensor state
            int sensorState = digitalRead(SENSOR_PIN);

            if(sensorState == HIGH) isDetected = 1;
            else objectCount += isDetected, isDetected = 0;

            // Get the current time
            time_t now = time(nullptr);
            tm* currentTime = localtime(&now);

             // Composing the email body
            string body = "The current object count is: " + to_string(objectCount)
                        + "\nTimestamp: " + to_string(currentTime->tm_hour)
                        + ":" + to_string(currentTime->tm_min);

            // Sending the email
            if(!currentTime->tm_min  &&  !currentTime->tm_sec)
            {
                int result = sendEmail(subject, body);
                if(result != CURLE_OK) cout << "Failed to send email" << endl;

                objectCount = 0;
            }
        }
    }

    return 0;
}
