import pickle

# Load the video features and descriptors from the PKL files
with open('video_features.pkl', 'rb') as features_file:
    video_features = pickle.load(features_file)

with open('video_descriptors.pkl', 'rb') as descriptors_file:
    video_descriptors = pickle.load(descriptors_file)

# Now you can access and view the loaded data
print("Video Features:")
print(video_features)

print("Video Descriptors:")
print(video_descriptors)

