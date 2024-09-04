import boto3

# Initialize AWS Rekognition client
rekognition_client = boto3.client('rekognition', region_name='your_region')

# Function to detect gender using Rekognition


def detect_gender(image_path):
    with open(image_path, 'rb') as image_file:
        # Upload image to Rekognition and detect faces
        response = rekognition_client.detect_faces(
            Image={'Bytes': image_file.read()},
            # Adjust attributes based on what you need (e.g., 'DEFAULT', 'ALL')
            Attributes=['ALL']
        )

        # Process face details
        for face_detail in response['FaceDetails']:
            gender = face_detail['Gender']['Value']
            print(f"Detected gender: {gender}")


# Example usage
image_path = 'path_to_your_image.jpg'
detect_gender(image_path)
