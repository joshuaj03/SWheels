from roboflow import Roboflow
rf = Roboflow(api_key="Qv3qeNeDOMfBITA48OI8")
project = rf.workspace("swheels").project("corridor-and-pathway-annotations")
version = project.version(4)
dataset = version.download("yolov8")
