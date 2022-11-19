from flask import Flask

app = Flask(__name__)

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

    if file.content_type in supported_files:
        # If it's a jpg, just return it with the options rendered

        # turn file into a PIL image
        image = Image.open(file)

        # Render the options
        image = renderJPG(image, options)

        return image
    elif file.content_type == "application/pdf":
        image = convertPDF(file, options)

        image = renderJPG(file, options)
        
        return image
    else:
        # If it's not a pdf or a jpg, return an error
        return "Not a pdf or jpg", 400

if __name__ == "__main__":
    app.run()