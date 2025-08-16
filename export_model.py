# export_model.py
# This script downloads YOLOv8n and exports it to ONNX

from ultralytics import YOLO

# Load YOLOv8 nano model (small and fast)
model = YOLO("yolov8n.pt")

# Export the model to ONNX format
model.export(format="onnx")
