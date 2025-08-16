from ultralytics import YOLO
import cv2

# Load exported ONNX model
model = YOLO("yolov8n.onnx")

# Run inference on one of your images
results = model("Plates/car.jpg")   # you can try car2.jpg, car3.png too

# Show results
results.show()

# Save results to a new image
results.save(filename="output_car.jpg")
print("✅ Results saved as output_car.jpg")
