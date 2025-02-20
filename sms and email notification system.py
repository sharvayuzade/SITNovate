import requests
import smtplib
import json
from email.mime.text import MIMEText as mt
from email.mime.multipart import MIMEMultipart as mmp
from twilio.rest import Client

# Server API
FIP = "http://192.168.1.100/status"

# Email Configuration
SMTP = "smtp.gmail.com"
PORT = 587
EADDRESS = "sharparcypher@gmail.com"
EPASS = "gzyf sczw oqlq kcim"  # Consider using environment variables for security

# Twilio Configuration
TWILIO_ACCOUNT_SID = "AC5d66c5fefba9ea2f30d028c60aa8079d"
TWILIO_AUTH_TOKEN = "61e9a424efb0bf0f709953843875337a"
TWILIO_MESSAGING_SERVICE_SID = "MGce95a5be288e9dade9f74ef4e6e86a7f"
TO_PHONE_NUMBER = "+919370692012"  # Recipient phone number

def chk_stat():
    try:
        r = requests.get(FIP, timeout=5)
        if r.status_code == 200:
            d = json.loads(r.text)
            return d.get("fall") == 1
        else:
            print("Error: Invalid response from server, Status Code:", r.status_code)
            return False
    except Exception as e:
        print("Error fetching data:", e)
        return False

def snd_eml():
    """Send an email alert when a fall is detected."""
    m = mmp()
    m["From"] = EADDRESS
    m["To"] = "sharvayu.zade.btech2023@sitnagpur.siu.edu.in"
    m["Subject"] = "Fall Detected!!"
    body = "Fall Detected!! Immediate action required."
    m.attach(mt(body, "plain"))

    try:
        s = smtplib.SMTP(SMTP, PORT)
        s.starttls()
        s.login(EADDRESS, EPASS)
        s.sendmail(EADDRESS, "sharvayu.zade.btech2023@sitnagpur.siu.edu.in", m.as_string())
        s.quit()
        print("Email was sent successfully.")
    except Exception as e:
        print("Error sending email:", e)

def snd_sms():
    try:
        client = Client(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN)
        message = client.messages.create(
            messaging_service_sid=TWILIO_MESSAGING_SERVICE_SID,
            body="🚨 Fall Detected!! Immediate action required.",
            to=TO_PHONE_NUMBER
        )
        print("SMS sent successfully. Message SID:", message.sid)
    except Exception as e:
        print("Error sending SMS:", e)
if chk_stat():
    snd_eml()
    snd_sms() 
else:
    print("No fall detected, no alerts sent.")