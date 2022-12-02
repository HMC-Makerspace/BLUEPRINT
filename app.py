from flask import Flask
from flask import request
from flask import send_file
from markupsafe import escape

from PIL import Image
from PIL import ImageDraw
import pypdfium2 as pdfium
import PyPDF2
from PyPDF2 import PdfFileWriter, PdfFileReader

import time
import json
import shutil
import os
import subprocess
import time
import math

app = Flask(__name__)


@app.route("/")
def index():
    # Return index.html from static/ directory
    return app.send_static_file("index.html")


@app.route("/renderImage", methods=["POST"])
def renderImage():
    # Start timer:
    start_time = time.time()

    # Always return a jpg or an error
    # Takes in a pdf or jpg
    # Returns a jpg

    # Get the file from the request
    file = request.files["file"]

    # Get options from the request
    options = request.form["options"]
    # Parse json into dict
    options = json.loads(options)

    # Check the file type
    supported_files = ["image/jpeg", "image/jpg", "image/png",
                       "image/gif", "image/bmp", "image/tiff", "image/tif", "image/webp"]
    image = None

    if file.content_type in supported_files:
        # If it's a jpg, just return it with the options rendered

        # turn file into a PIL image
        image = Image.open(file)

        # Render the options
        image, width, height, dpi = calculateJPG(image, options)

    elif file.content_type == "application/pdf":
        image = convertPDF(file, options)

        image, width, height, dpi = calculateJPG(image, options)

    if image == None:
        return "Error: Unsupported file type"

    if (options["print"]):
        printPhoto(image, width, height, dpi)

    image = previewPhoto(image, width, height, options["paper_width"])

    # Save to temp output file with timestamp
    timestamp = str(time.time())
    #replace the decimal
    timestamp = timestamp.replace(".", "_")
    image.save("cache/" + timestamp + "output.png")

    # Stop timer
    end_time = time.time()
    print("Rendered image in " + str(end_time - start_time) + " seconds")

    # Return body, status code, headers
    return "/getImage/" + timestamp, 200, {"Content-Type": "image/png"}


@app.route("/getImage/<timestamp>", methods=["GET"])
def getImage(timestamp):
    # Get timestamp from request

    # Check if the file exists
    if os.path.exists("cache/" + escape(timestamp) + "output.png"):
        # Return body, status code, headers
        return send_file("cache/" + escape(timestamp) + "output.png", mimetype="image/png")
    else:
        return "Error: File not found"


def convertPDF(file, options):
    pdf = pdfium.PdfDocument(file)
    pdf2 = PyPDF2.PdfFileReader(file, "rb")

    p = pdf2.getPage(0)
    page = pdf[0]

    width = p.mediaBox.getWidth()/72
    height = p.mediaBox.getHeight()/72

    min_side_length = min(width, height)
    desPixels = options["paper_width"] * 300
    dpi = int(desPixels/min_side_length)

    image = page.render_to(
        pdfium.BitmapConv.pil_image,
        scale=dpi/72,  # pdf's units are in 1/72 of an inch, picos
    )

    return image


def calculateJPG(image, options):
    # Render the options on the image
    # Return the image

    final_width_inches = 0
    final_height_inches = 0

    # Flip the image so that the long side is the width
    if image.width > image.height and options["side"] == "short":
        image = image.transpose(Image.ROTATE_90)

    if image.width < image.height and options["side"] == "long":
        image = image.transpose(Image.ROTATE_90)

    # Case 1: Specific height/width
    if options["specific_width"] != None or options["specific_height"] != None:
        # Resize the image to the specified width & height
        if options["specific_width"] != None: #specified width, calculate height
            # Calc dpi
            final_dpi = image.width / options["specific_width"]
            final_width_inches = options["specific_width"]
            final_height_inches = image.height / final_dpi

        elif options["specific_height"] != None: #specified height, calculate width
            # Calc dpi
            final_dpi = image.height / options["specific_height"]
            final_width_inches = image.width / final_dpi
            final_height_inches = options["specific_height"]

        else: #specified both, just resize
            # Calc dpi from both width and height
            final_dpi = image.width / options["paper_width"]

            # Resize the image
            final_width_inches = options["specific_width"]
            final_height_inches = options["specific_height"]

    # Case 2: Specific DPI
    elif options["specific_dpi"] != None:
        # Resize the image to the specified dpi
        final_dpi = options["specific_dpi"]

        final_width_inches = image.width / final_dpi
        final_height_inches = image.height / final_dpi

    # Case 3: Auto max size
    else:
        side_a = 0
        side_b = 0
        if (options["side"] == "short" and image.width > image.height) or (options["side"] == "long" and image.width < image.height):  # shorter sidea
            side_a = image.height
            side_b = image.width
        else:
            side_a = image.width
            side_b = image.height

        final_dpi = side_a / options["paper_width"]

        final_height_inches = side_b / final_dpi
        final_width_inches = options["paper_width"]

    # Make sure all final sizes are ints
    final_width_inches = int(final_width_inches)
    final_height_inches = math.ceil(final_height_inches)
    final_dpi = int(final_dpi)

    print("Final width: " + str(final_width_inches) + " inches")
    print("Final height: " + str(final_height_inches) + " inches")

    return image, final_width_inches, final_height_inches, final_dpi


def encodedDistance(inches):
    start = 65535
    vector = [0, 1, 0, 1, 1, 0, 1, 0, 1, 1,
              0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1]
    # we always drop 39935
    # we always rise 25600
    result = []
    stage = 0

    for i in range(inches + 1):
        if (vector[stage] == 0):
            result.append(start - 39935)
            start = start - 39935
        else:
            result.append(start + 25600)
            start = start + 25600

        stage = stage + 1

        if (stage == 23):
            stage = 0

    return int(hex(result[i-1]), 16)


def setEpsonConfig(width, height):
    configLocation = "C:\ProgramData\EPSON\EPSON SC-P8000 Series\E_31CL01LE.UCF"
    filename = "E_31CL01LE.UCF"

    height = min(580, height)
    width = min(44, width)
    
    with open(filename, 'r+b') as f:
        newDec = encodedDistance()
        newbytes = newDec.to_bytes(2, byteorder='big')
        f.seek(0X0004173C)
        f.write(newbytes)

        newDec = encodedDistance()
        newbytes = newDec.to_bytes(2, byteorder='big')
        f.seek(0X00041738)
        f.write(newbytes)

        if (height > width):
            f.seek(0X00041736)
            newDec = 0x00
            newbytes = newDec.to_bytes(2, byteorder='big')
            f.write(newbytes)
        else:
            f.seek(0X00041736)
            newDec = 0x01
            newbytes = newDec.to_bytes(2, byteorder='big')
            f.write(newbytes)

    # copy file to config location
    shutil.copy("E_31CL01LE.UCF", configLocation)


def previewPhoto(image, width, height, paper_width):
    # Create a 1000x862 blank image transparent image
    preview = Image.new('RGBA', (1000, 862), (255, 255, 255, 0))

    width_pix = 420 * (width / 44)
    # Calc height from width
    height_pix = width_pix * (height/width)

    # Resize the image
    image = image.resize((int(width_pix), int(height_pix)), Image.ANTIALIAS)

    # Paste the image so bottom right aligns with 705 px X and 669 px Y
    preview.paste(image, (705 - int(width_pix), 669 - int(height_pix)))

    return preview


def printPhoto(image, width, height, dpi):
    setEpsonConfig(width, height)

    # Save to temp output file
    image.save("output.png")

    # Print the image
    # Call PrintGUI/Executable/PrintGUI.exe
    current_dir = os.path.dirname(os.path.realpath(__file__))

    path = "PrintGUI/Executable/PrintGUI.exe"

    # Get cwd
    cwd = os.getcwd()

    print("Printing...")
    print(cwd + "\\output.png")

    p = subprocess.Popen([path, cwd + "\\output.png"], shell=False)


if __name__ == "__main__":
    app.run()
