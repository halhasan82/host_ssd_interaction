import cv2
import pickle

# Load the video
video_path = '/home/hasan/host_ssd_interaction/dataset/hitting/uccrime_Fighting050_x264.mp4'
cap = cv2.VideoCapture(video_path)

# Create a feature detector (SIFT, ORB, or other feature detector)
# Create an ORB feature detector
detector = cv2.ORB_create()


# Lists to store features and descriptors
video_features = []
video_descriptors = []

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Convert the frame to grayscale for feature detection
    gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect features and compute descriptors
    keypoints, descriptors = detector.detectAndCompute(gray_frame, None)

    # Convert KeyPoints to a format that can be pickled
    keypoints_info = []
    for kp in keypoints:
        keypoint_dict = {
            'pt': kp.pt,
            'size': kp.size,
            'angle': kp.angle,
            'response': kp.response,
            'octave': kp.octave,
            'class_id': kp.class_id
        }
        keypoints_info.append(keypoint_dict)

    # Store the features and descriptors
    video_features.append(keypoints_info)
    video_descriptors.append(descriptors)

cap.release()

# Save features and descriptors using pickle
with open("video_features.pkl", "wb") as file:
    pickle.dump(video_features, file)
with open("video_descriptors.pkl", "wb") as file:
    pickle.dump(video_descriptors, file)

