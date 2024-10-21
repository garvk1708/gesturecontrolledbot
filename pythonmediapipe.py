import cv2
import mediapipe as mp
import socket

width = 1000
height = 1000

# Initialize MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False,
                       max_num_hands=1,
                       min_detection_confidence=0.8,
                       min_tracking_confidence=0.8)

# Initialize MediaPipe Drawing utility
mp_drawing = mp.solutions.drawing_utils

# Function to count fingers
def count_fingers(landmarks):
    finger_tips = [8, 12, 16, 20]  # Index of finger tips in landmarks
    finger_base = [5, 9, 13, 17]   # Index of finger base in landmarks

    count = 0
    for tip, base in zip(finger_tips, finger_base):
        if landmarks[tip].y * height < landmarks[base].y * height:  # Finger is up
            count += 1

    # Check for thumb
    if landmarks[4].x * width > landmarks[3].x * width:  # Thumb is up
        count += 1

    return count

# Function to map finger counts to Arduino commands
def map_finger_to_command(finger_count):
    if finger_count == 0:
        return "x"  # Stop
    elif finger_count == 1:
        return "w"  # Move forward
    elif finger_count == 2:
        return "a"  # Turn left
    elif finger_count == 3:
        return "d"  # Turn right
    elif finger_count == 4:
        return "s"  # Move backward
    return "none"

esp_ip = '192.168.137.125'
esp_port = 80

# Keep the socket connection open during the whole session
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((esp_ip, esp_port))

    cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    cap.set(cv2.CAP_PROP_FPS, 30)

    last_message = b"none"

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Error: Could not read frame")
            break

        # Convert the BGR image to RGB
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        # Process the frame with MediaPipe Hands
        results = hands.process(rgb_frame)

        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                # Draw hand landmarks on the frame
                mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

                # Convert hand landmarks to a list of (x, y) coordinates
                landmarks = hand_landmarks.landmark

                # Count the number of fingers
                finger_count = count_fingers(landmarks)

                # Display the number of fingers on the frame
                cv2.putText(frame, f"Fingers: {finger_count}", (50, 50), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 0, 0), 2)

                # Map the finger count to the corresponding command
                command = map_finger_to_command(finger_count)
                message = command.encode() + b"\r"

                # Send the command if it has changed
                if message != last_message:
                    s.sendall(message)
                    print(f"Sent command: {command}")
                    last_message = message

        # Show the frame with landmarks
        cv2.imshow('Frame', frame)

        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# Release resources
cap.release()
cv2.destroyAllWindows()
hands.close()
