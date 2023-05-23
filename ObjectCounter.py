import time
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import RPi.GPIO as GPIO

# GPIO pin number connected to the object detecting sensor output
SENSOR_PIN = 0

# Email configuration
SMTP_SERVER = "your_smtp_server"
SMTP_PORT = 587
SMTP_USERNAME = "your_email_username"
SMTP_PASSWORD = "your_email_password"
SENDER_EMAIL = "sender_email@example.com"
RECIPIENT_EMAIL = "recipient_email@example.com"

# Function to send the email
def send_email(subject, body):
    msg = MIMEMultipart()
    msg['From'] = SENDER_EMAIL
    msg['To'] = RECIPIENT_EMAIL
    msg['Subject'] = subject
    msg.attach(MIMEText(body, 'plain'))

    try:
        server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
        server.starttls()
        server.login(SMTP_USERNAME, SMTP_PASSWORD)
        server.send_message(msg)
        server.quit()
        print("Email sent successfully")
    except Exception as e:
        print("Failed to send email:", str(e))



def main():
    # Set up GPIO
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(SENSOR_PIN, GPIO.IN)

    objectCount = 0
    isDetected = False
    subject = "Object Count Update"

    while True:
        # Read the sensor state
        sensorState = GPIO.input(SENSOR_PIN)

        if sensorState == GPIO.HIGH:
            isDetected = True
        else:
            objectCount += int(isDetected)
            isDetected = False

        # Get the current time
        currentTime = time.localtime()

        # Composing the email body
        body = "The current object count is: {}\nTimestamp: {}:{}".format(
            objectCount, currentTime.tm_hour, currentTime.tm_min
        )

        # Sending the email
        if currentTime.tm_min == 0 and currentTime.tm_sec == 0:
            send_email(subject, body)
            objectCount = 0

        time.sleep(1)

    GPIO.cleanup()

if __name__ == '__main__':
    main()
