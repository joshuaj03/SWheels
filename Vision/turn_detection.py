import multiprocessing.managers
from ultralytics import YOLO
import cv2
import sys
import os
import multiprocessing

parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.append(os.path.join(parent_dir, "motor_control"))
import pi_to_motor

# Load YOLOv8N Model
model_path = "/home/jake/Downloads/best (1).pt"
model = YOLO(model_path)

# Global Variables
manager = multiprocessing.Manager()
object_to_find = manager.Value("s", "") # Object to find --> door, path etc
is_object_found = manager.Value("b", False)

def update_object(new_object):
    object_to_find.set(new_object)
    print("Object to find: ", object_to_find.get())

def detect_object():
    global is_object_found

    # Open Camera
    cap = cv2.VideoCapture("/dev/video0", cv2.CAP_V4L2)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    frame_width = 640
    command_set = {"direction": None,"distance": 0, "time": 0,"angle": 0,"speed": 0}

    if not cap.isOpened():
        print("Failed to open camera. Exiting...")
        exit()

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Failed to capture frame. Exiting...")
            break

        # Run Inference
        results = model(frame, verbose=False)

        is_object_found.set(False)

        for result in results:
                for box in result.boxes:
                    x1, y1, x2, y2 = map(int, box.xyxy[0])  # Get bounding box coordinates
                    conf = float(box.conf[0])  # Confidence score
                    cls = int(box.cls[0])  # Class ID
                    
                    # Get class name
                    class_name = model.names[cls]  # Ensure your model has names attribute 

                    if conf > 0.5 and class_name == object_to_find.get():
                        is_object_found.set(True)
                        print(f"{class_name} detected with confidence {conf:.2f}!")

                        # Calculate path to object
                        center_x = (x1+x2) // 2
                        error = (center_x - (frame_width // 2)) / (frame_width // 2)

                        if abs(error) < 0.1:
                            command_set["direction"] = "forward"
                            command_set["angle"] = 0
                        elif error > 0:
                            command_set["direction"] = "right"
                            command_set["angle"] = 30
                        else:
                            command_set["direction"] = "left"
                            command_set["angle"] = 30
                        pi_to_motor.send_to_esp(command_set) # Send command to motor controls
                    
                    # Draw bounding box
                    cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)

                    # Draw label text (Class + Confidence Score)
                    label = f"{class_name} {conf:.2f}"
                    cv2.putText(frame, label, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 
                                0.5, (0, 255, 0), 2, cv2.LINE_AA)

        # Show Output
        #cv2.imshow("YOLOv8 Detection", frame)

        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    # Cleanup
    cap.release()
    cv2.destroyAllWindows()

def start_detection():
    process = multiprocessing.Process(target=detect_object, daemon=True)
    process.start()

    if process.is_alive():
        print("Detection process is running in the background.")
    else:
        print("Detection process is not running.")

