# To be used in Google Colab
import numpy as np
import tensorflow as tf
from google.colab.patches import cv2_imshow
import cv2 as cv
import os

# Check the file path
file_path = '/content/drive/MyDrive/dog_bike_car.jpeg'
if not os.path.isfile(file_path):
    print(f"File not found: {file_path}")
else:
    # Read the graph.
    with tf.io.gfile.GFile('/content/drive/MyDrive/frozen_inference_graph.pb', 'rb') as f:
        graph_def = tf.compat.v1.GraphDef()
        graph_def.ParseFromString(f.read())

    with tf.compat.v1.Session() as sess:
        # Restore session
        sess.graph.as_default()
        tf.import_graph_def(graph_def, name='')

        # Read and preprocess an image.
        img = cv.imread(file_path)

        if img is not None:
            rows = img.shape[0]
            cols = img.shape[1]
            inp = cv.resize(img, (300, 300))
            inp = inp[:, :, [2, 1, 0]]  # BGR2RGB

            # Run the model
            out = sess.run([sess.graph.get_tensor_by_name('num_detections:0'),
                            sess.graph.get_tensor_by_name('detection_scores:0'),
                            sess.graph.get_tensor_by_name('detection_boxes:0'),
                            sess.graph.get_tensor_by_name('detection_classes:0')],
                            feed_dict={'image_tensor:0': inp.reshape(1, inp.shape[0], inp.shape[1], 3)})

            # Visualize detected bounding boxes.
            num_detections = int(out[0][0])
            detected_objects = set()  # To store unique detected object classes
            for i in range(num_detections):
                classId = int(out[3][0][i])
                score = float(out[1][0][i])
                bbox = [float(v) for v in out[2][0][i]]
                if score > 0.3:
                    x = bbox[1] * cols
                    y = bbox[0] * rows
                    right = bbox[3] * cols
                    bottom = bbox[2] * rows
                    cv.rectangle(img, (int(x), int(y)), (int(right), int(bottom)), (125, 255, 51), thickness=2)

                    # Count and classify detected objects
                    detected_objects.add(classId)
                    print(f"Detected object {classId}: {score}")

            # Print the count and unique classes
            print(f"Total detected objects: {len(detected_objects)}")

            cv2_imshow(img)
        else:
            print("Error reading the image.")
