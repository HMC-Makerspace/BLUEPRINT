from flask import Flask
from flask import request

from PIL import Image
import pypdfium2 as pdfium
import PyPDF2 
from PyPDF2 import PdfFileWriter, PdfFileReader

app = Flask(__name__)

ENCODED_BYTES = [0xf401,0x5802,0xbc02,0x2003,0x8403,0xe803,0x4c04,0xb004,0x1405,0x7805,0xdc05,0x4006,0xa406,0x0807,0x6C07,0xd007,0x3408,0x9808,0xFC08,0x6009,0xc409,0x280a,0x8c0a,0xf00a,0x540b,0xB80B,0x1c0c,0x800c,0xe40c,0x480d,0xac0d,0x100e,0x740e,0xd80e,0x3c0f,0xa00f,0x0410,0x6810,0xcc10,0x3011,0x9411,0xf811,0x5c12,0xc012,0x2413,0x8813,0xec13,0x5014,0xb414,0x1815,0x7c15,0xe015,0x4416,0xa816,0x0c17,0x7017,0xd417,0x3818,0x9c18,0x0019,0x6419,0xc819,0x2c1a,0x901a,0xf41a,0x581b,0xbc1b,0x201c,0x841c,0xe81c,0x4c1d,0xb01d,0x141e,0x781e,0xdc1e,0x401f]

@app.route("/")
def index():
    # Return index.html from static/ directory
    return app.send_static_file("index.html")

@app.route("/processFile", methods=["POST"])
def processFile():
    # Always return a jpg or an error
    # Takes in a pdf or jpg
    # Returns a jpg

    # Get the file from the request
    file = request.files["file"]

    # Get options from the request
    options = request.form["options"]

    # Check the file type
    supported_files = ["image/jpeg", "image/jpg", "image/png", "image/gif", "image/bmp", "image/tiff", "image/tif", "image/webp"]
    image = None

    if file.content_type in supported_files:
        # If it's a jpg, just return it with the options rendered

        # turn file into a PIL image
        image = Image.open(file)

        # Render the options
        image = renderJPG(image, options)

        return image
    elif file.content_type == "application/pdf":
        image = convertPDF(file, options)

        image = renderJPG(image, options)

        return image
    
    if image == None:
        return "Error: Unsupported file type"
    
    if (options["print"]):
        printPhoto(image)

    if (options["preview"]):
        image = previewPhoto(image)

    return image


def convertPDF(file, options):
    pdf = pdfium.PdfDocument(file)
    pdf2 = PyPDF2.PdfFileReader(file,"rb")

    p = pdf2.getPage(0)
    page = pdf[0]

    width = p.mediaBox.getWidth()/72
    height = p.mediaBox.getHeight()/72

    min_side_length = min(width,height)
    desPixels = options.paper_width * 300
    dpi = int(desPixels/min_side_length)

    image = page.render_to(
        pdfium.BitmapConv.pil_image,
        scale = dpi/72,             #pdf's units are in 1/72 of an inch, picos
    )

    return image

def renderJPG(image, options):
    # Render the options on the image
    # Return the image

    def endcodedDistance(height):
        start = 65535 
        vector = [0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,1]
        # we always drop 39935
        # we always rise 25600
        result = []
        stage = 0
        
        for i in range(height):
            if (vector[stage] == 0):
                result.append(start - 39935)
                start = start - 39935
            else:
                result.append(start + 25600)
                start= start + 25600
            
            stage = stage + 1

            if (stage == 23):
                stage= 0

        return int(hex(result[i-1]))


    # Case 1: Specific height/width
    if options.specific_width != None or options.specific_height != None:
        # Flip the image so that the long side is the width
        if image.width > image.height:
            image = image.transpose(Image.ROTATE_90)

        # Resize the image to the specified width & height
        if options.specific_width != None:
            # Calc dpi
            dpi = int(options.specific_width / image.width)

            # Resize the image
            image = image.resize((options.specific_width, int(image.height * dpi)), Image.ANTIALIAS)
        elif options.specific_height != None:
            # Calc dpi
            dpi = int(options.specific_height / image.height)

            # Resize the image
            image = image.resize((int(image.width * dpi), options.specific_height), Image.ANTIALIAS)
        else :
            # Calc dpi from both width and height
            dpi = int(options.paper_width / image.width)
            dpi = max(dpi, int(options.paper_height / image.height))

            # Resize the image
            image = image.resize((int(image.width * dpi), int(image.height * dpi)), Image.ANTIALIAS)
        
        # Flip the image so that the long side is the width
        if image.width > image.height:
            image = image.transpose(Image.ROTATE_270)

    
    # Case 2: Specific DPI
    elif options.specific_dpi != None:
        # Resize the image to the specified dpi
        dpi = options.specific_dpi

        # Resize the image
        image = image.resize((int(image.width * dpi), int(image.height * dpi)), Image.ANTIALIAS)

        # Flip the image so that the long side is the width
        if image.width > image.height:
            image = image.transpose(Image.ROTATE_90)

    # Case 3: Specific paper size
        
    

if __name__ == "__main__":
    app.run()