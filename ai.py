import cv2
import telepot

token = '5407124593:AAHDcAY2kA9lk__Vo7k3RQMdt-QJ2GbcCSQ'
receiver_id = '5561934635'

bot = telepot.Bot(token)

cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)   # screen HD size
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)   # screen HD size


while True:
    _, frame = cap.read()
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)    # change input video to HSV
    height, width, _ = frame.shape

    cx = int(width / 2)    #make width center point
    cy = int(height / 2)   #make height center point

    # Pick pixel value
    pixel_center = hsv_frame[cy, cx]
    hue_value = pixel_center[0]
    color = "Undefined"
    if hue_value < 5:                      #selection red color hue number, < 5 = red
        color = "MATANG SEMPURNA"                   #Red
        bot.sendMessage(receiver_id, "Tomat Merah Siap Panen")
    elif hue_value < 22 :                  #selection orange color hue number, 5 - 21= orange
        color = "MATANG"          #Orange
        bot.sendMessage(receiver_id, "Tomat Orange Siap Panen")
    elif hue_value < 33:                   #selection yellow color hue number, 22 - 32 = yellow
        color = "SETENGAH MATANG"          #Yellow
        bot.sendMessage(receiver_id, "Tomat Kuning Siap Panen")
    elif hue_value < 78:                   #selection green color hue number, 33 - 77 = green
        color = "MENTAH"                   #Green
    else:
        color = "No Object"

    pixel_center_bgr = frame[cy, cx]
    b, g, r = int(pixel_center_bgr[0]), int(pixel_center_bgr[1]), int(pixel_center_bgr[2])

    cv2.rectangle(frame, (cx - 220, 10), (cx + 100, 80), (255, 255, 255), -1)   # Create rectangle for text
    cv2.putText(frame, color, (cx - 200, 60), 0, 1, (b, g, r), 5)    # text for matang, setengah matang, dan mentah
    cv2.circle(frame, (cx, cy), 5, (25, 25, 25), 3)   # center point color and size of circle

    cv2.imshow("Frame", frame)
    key = cv2.waitKey(1)
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()