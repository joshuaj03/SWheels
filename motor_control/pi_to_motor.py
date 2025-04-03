import serial
import time
import json

def send_command(json_data):
    try:
        ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)  # Adjust baud rate if needed
        time.sleep(2)  # Wait for the serial connection to stabilize
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        exit(1)
    ser.write((json_data + "\n").encode())
    ser.flush()
    time.sleep(0.1)
    print(f"Sent: {json_data}")


def send_to_esp(command_set):
        json_data = json.dumps(command_set)
        send_command(json_data)

# send_to_esp("{"action": "move", "direction": "forward", "distance": 5, "time": null, "angle": null, "speed": null}")