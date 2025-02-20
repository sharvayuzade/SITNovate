import requests
import smtplib
import json
from email.mime.text import MIMEText as mt
from email.mime.multipart import MIMEMultipart as mmp
FIP = "http://192.168.1.100/status"
SMTP = "smtp.gmail.com"
PORT = 587
EADDRESS = "sharparcypher@gmail.com"
EPASS = "gzyf sczw oqlq kcim"
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
        print("Email was sent")
    except Exception as e:
        print("Error:", e)
if chk_stat():
    snd_eml()
else:
    print("No fall detected, no email sent.")
