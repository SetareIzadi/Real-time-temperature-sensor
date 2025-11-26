import serial
import json
import csv
from datetime import datetime

ser = serial.Serial('/dev/tty.usbmodem1101', 9600)

csv_file = open("data.csv", "a", newline="")
writer = csv.writer(csv_file)
writer.writerow(["timestamp", "nr", "temp"])

while True:
    line = ser.readline().decode().strip()
    try:
        data = json.loads(line)
        timestamp = datetime.now().isoformat()

        print(f"{timestamp} | index={data['nr']} temp={data['temp']} C")

        writer.writerow([timestamp, data["nr"], data["temp"]])
        csv_file.flush()

    except:
        print("Invalid line:", line)