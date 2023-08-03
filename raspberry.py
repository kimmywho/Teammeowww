#!/usr/bin/python3

# Import necessary libraries / 필요한 라이브러리 불러오기
import ssl
import time
import serial
import paho.mqtt.publish as publish
import psutil
import string
import slack_sdk as slack
import requests

# Open the serial port to communicate with the Arduino Nano 33 BLE
# 아두이노 나노 33 BLE와 통신하기 위해 시리얼 포트 열기
serial1 = serial.Serial('/dev/serial/by-id/usb-Arduino_Nano_33_BLE_68E43EFDB6068771-if00', 115200)

# Discord webhook URL to send messages / 메시지를 보내기 위한 디스코드 웹훅
url = "https://discord.com/api/webhooks/1135818152408776804/KF23LacbjLVvgtM90WyN-hk-v3L1lWi_b4qyZkCdIuVIqdEwS2lgVxQc24Y49K-IryjU"
data = {"content": "Detecting Fire"}

# ThingSpeak configuration
# ThingSpeak 설정
channel_ID = "0000000"  # Replace with the actual ThingSpeak Channel ID
mqtt_host = "mqtt3.thingspeak.com"
mqtt_client_ID = "XXXXXXXXXXXXXXXXXXXXXXX"  # Replace with your MQTT client ID
mqtt_username = "XXXXXXXXXXXXXXXXXXXXXXX"  # Replace with your MQTT username
mqtt_password = "XXXXXXXXXXXXXXXXXXXXXXXX"  # Replace with your MQTT password
t_transport = "websockets"
t_port = 80

# Create the MQTT topic string.
topic = "channels/" + channel_ID + "/publish"

# Initialize some variables
index = 1
my_float = 0.23

# Create a session with disabled SSL certificate verification (for debugging purposes)
session = requests.Session()
session.verify = False

# Initialize the Slack client with your Slack API token
client = slack.WebClient("xoxb-0000000000000-000000000000-XXXXXXXXXXXXXXXXXXXXXXXX")

# Main loop
while True:
    dataString = serial1.readline()
    person_score = float(dataString)

    # If the person_score is greater than 0.6, send a message to Discord and Slack
    if person_score > 0.6:
        try:
            result = requests.post(url, json=data)
            client.chat_postMessage(channel="#meowo", text=data['content'])
        except:
            pass

    # Build the payload string to publish to ThingSpeak
    payload = "field1=" + str(person_score)

    # Attempt to publish this data to the ThingSpeak topic via MQTT
    # MQTT를 사용하여 ThingSpeak에 데이터를 게시
    try:
        print("Writing Payload =", payload, "to host:", mqtt_host, "clientID=", mqtt_client_ID, "User", mqtt_username, "PWD", mqtt_password)
        publish.single(
            topic,
            payload,
            hostname=mqtt_host,
            transport=t_transport,
            port=t_port,
            client_id=mqtt_client_ID,
            auth={'username': mqtt_username, 'password': mqtt_password}
        )
    except Exception as e:
        print(e)

    time.sleep(2)

# Close the serial interface after exiting the loop / 반복을 종료하면 시리얼 인터페이스를 기
serial1.close()
