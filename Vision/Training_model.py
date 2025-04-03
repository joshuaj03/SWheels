from ultralytics import YOLO
from roboflow import Roboflow

rf = Roboflow(api_key="Qv3qeNeDOMfBITA48OI8")

project = rf.workspace("swheels").project("corridor-and-pathway-annotations")
version = project.version(4)
dataset = version.download("yolov8")

# Load Model
model = YOLO("yolo11n.pt")

data_path = "/content/Corridor-and-Pathway-Annotation-4/data.yml"

# Train yolo11n
print("Training the model...")
model.train(data = data_path, epochs = 25, imgsz = 640 )

# Validate
model.val(data = data_path, epochs = 25, imgs = 640)

# Predict
predict_model_path = "/content/runs.detect/train/weights/best.pt"
print("Making Prediction")
