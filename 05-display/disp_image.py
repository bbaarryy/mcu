from PIL import Image
import serial
import time

PORT = "/dev/ttyACM0"
BAUD = 115200

IMAGE_PATH = "/home/l3v/repo/ant-mcu/5 Как абстрагироваться от железа/Задания/pics/get.jpg"

try:
    # открыть изображение
    image = Image.open(IMAGE_PATH)

    # перевести в RGB
    image = image.convert("RGB")

    width, height = image.size
    print("Image size:", width, height)

    # открыть serial
    ser = serial.Serial(PORT, BAUD, timeout=1)

    time.sleep(2)

    for y in range(height):
        for x in range(width):

            r, g, b = image.getpixel((x, y))

            color = (r << 16) | (g << 8) | b

            cmd = f"disp_px {x} {y} {color:06X}\n"

            ser.write(cmd.encode())

finally:
    time.sleep(0.1)
    ser.close()