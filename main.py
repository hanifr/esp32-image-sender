import cv2
import urllib.request
import numpy as np
from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive

# Connect to Google Drive
gauth = GoogleAuth()
gauth.LocalWebserverAuth()
drive = GoogleDrive(gauth)

# Folder ID in Google Drive
folder_id = "1ptNc18b2-0lSM5w7lOHWtRpP012kJSGm"

url = 'http://192.168.1.191/cam-lo.jpg'

count = 0

while True:
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    frame = cv2.imdecode(imgnp, -1)

    # Save the image to Colab's temporary storage
    image_path = '/content/{}.png'.format(count)
    cv2.imwrite(image_path, frame)

    # Upload the image to Google Drive
    file_drive = drive.CreateFile({'parents': [{'id': folder_id}], 'title': image_path})
    file_drive.Upload()

    print("Image uploaded as: " + image_path)

    count += 1

    # Break the loop after a certain number of iterations (optional)
    if count >= 10:
        break

# Note: Google Colab doesn't support real-time image streaming or continuous display.
# You can check your Google Drive for the uploaded images.
