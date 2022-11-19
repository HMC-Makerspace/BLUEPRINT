import pip
try:
    import pypdfium2 as pdfium
    import PyPDF2 
    from PyPDF2 import PdfFileWriter, PdfFileReader
    from PIL import Image
    import easygui
    import os
    import shutil
    from fileinput import filename
    from datetime import date
    import math
except:
    pip.main(['install','pypdfium2'])
    pip.main(['install','pillow'])
    pip.main(['install','easygui'])
    pip.main(['install','PyPDF2'])
    pip.main(['install','fpdf'])
    pip.main(['install','matplotlib'])
    import pypdfium2 as pdfium
    import PyPDF2
    from PIL import Image
    import easygui
    import os
    import shutil
    from fileinput import filename
    from datetime import date
    import math

#Offputting when rendering pdf doesnt see anything
#add a way to specify size via DPI
# make a non-answer kill program
# make the output parameters more clear (such as the calculated size, DPI, etc)

def endcodedDistance(height):
    if height < 5:
        height = 5
        #the crazy ass LUT to convert the distance to the encoded value
    whyIsItSoLong=[0xf401,0x5802,0xbc02,0x2003,0x8403,0xe803,0x4c04,0xb004,0x1405,0x7805,0xdc05,0x4006,0xa406,0x0807,0x6C07,0xd007,0x3408,0x9808,0xFC08,0x6009,0xc409,0x280a,0x8c0a,0xf00a,0x540b,0xB80B,0x1c0c,0x800c,0xe40c,0x480d,0xac0d,0x100e,0x740e,0xd80e,0x3c0f,0xa00f,0x0410,0x6810,0xcc10,0x3011,0x9411,0xf811,0x5c12,0xc012,0x2413,0x8813,0xec13,0x5014,0xb414,0x1815,0x7c15,0xe015,0x4416,0xa816,0x0c17,0x7017,0xd417,0x3818,0x9c18,0x0019,0x6419,0xc819,0x2c1a,0x901a,0xf41a,0x581b,0xbc1b,0x201c,0x841c,0xe81c,0x4c1d,0xb01d,0x141e,0x781e,0xdc1e,0x401f]
    return whyIsItSoLong[height-5]

def reRenderPDF(path,desiredWidth):

    pdf_reader = PdfFileReader(path)
    pdf_writer = PdfFileWriter()
    for page in range(pdf_reader.getNumPages()):
        page = pdf_reader.getPage(page)
        pdf_writer.addPage(page)
    with open(path, 'wb') as out:
        
        # writes to the respective output_pdf provided
        pdf_writer.write(out)
        renderPDF(path,desiredWidth)
    return ".pdf"

def renderPDF(path,desiredWidth):
    pdf = pdfium.PdfDocument(path)
    pdf2 = PyPDF2.PdfFileReader(path,"rb")
    n_pages = len(pdf)  # get the number of pages in the document

    if n_pages > 1:
        pagenum=easygui.integerbox(msg='Which page would you like to print\n (Pages 1 - '+str(n_pages)+')', title=' ', default=1, lowerbound=1, upperbound=n_pages, image=None, root=None)
        p = pdf2.getPage(pagenum-1)
        page = pdf[pagenum-1]
    else:
        p = pdf2.getPage(0)
        page = pdf[0]

    width=p.mediaBox.getWidth()/72
    height=p.mediaBox.getHeight()/72
    smallest=min(width,height)
    largest=max(width,height)
    desPixels=desiredWidth*300
    dpi=desPixels/smallest
    dpi=int(dpi)

    #display waiting message
    easygui.msgbox(msg='Please wait while the image is rendered\n Press "OK" to start rendering.', title=' ', ok_button='OK', image=None, root=None)
    
    image = page.render_to(
    pdfium.BitmapConv.pil_image,
    scale = dpi/72,             #pdf's units are in 1/72 of an inch
)
    image.save("out.jpg")
    return ".jpg"

printPreferences=0 #0=as large as possible, 1=custom size, 2=custom DPI
calcDimens=0 # If Print Preferences==1; 0=Defined Width and height, 1=Width, calculate height, 2=Height, calculate width
flipImg=1 #if 1, flip image to always be horiz, if 0, flip img to always be vertical.

sizeChoice=easygui.indexbox(msg='Are you printing as large as possible, to a specific size, or to a specific DPI.', title='Image Size', choices=('As large as possible', 'Specific size','Specific DPI'), image=None)
paperWidth=easygui.buttonbox(msg='What is the size of paper currently loaded in the printer?', title='Paper Size', choices=('17', '24', '36','44'), image=None)
paperWidth=int(paperWidth)

if sizeChoice == 0: #as large as possible
    printPreferences=0

elif sizeChoice==1: #specific size
    manualAutomatic=easygui.indexbox(msg='Do you want to specify both the width and the height or specify one and let the program calculate the other automatically?', title='Width and Height', choices=('Width and Height','Auto Calculate'), image=None)
    printPreferences=1
    if manualAutomatic == 0: #manual define width and height
        widthChoice=easygui.indexbox(msg='Do you want the short side of the image or the long side of the image printed across the width of the paper?\n Please only choose "short" if you need to print the long side of the image longer than the width of paper loaded in the printer.', title='Paper Width ', choices=('Long', 'Short'), image="longShortSide.jpg")
        if widthChoice == 0: #width is the longer side
            imageWidth=easygui.integerbox(msg='Please enter the width of the image you would like to print in inches. \n Please note the width is the longer side of the image', title='Image Width', default=17, lowerbound=1, image="widthHeight.jpg" , upperbound=paperWidth)
            imageHeight=easygui.integerbox(msg='Please enter the height of the image you would like to print in inches. \n Please note the height is the shorter side of the image', title='Image Height', default=11, lowerbound=5, upperbound=80, image="widthHeight.jpg") #max height is 80 inches because thats all my LUT supports
            calcDimens=0
        else: #height is the longer side
            imageWidth=easygui.integerbox(msg='Please enter the width of the image you would like to print in inches. \n Please note the width is the shorter side of the image', title='Image Width', default=17, lowerbound=1, image="heightWidth.jpg" , upperbound=paperWidth) 
            imageHeight=easygui.integerbox(msg='Please enter the height of the image you would like to print in inches. \n Please note the height is the longer side of the image, this should be greater than the paper width', title='Image Height', default=11, lowerbound=paperWidth, upperbound=80, image="heightWidth.jpg") #max width is 80 inches because thats all my LUT supports. Lower bound is paperWidth because if they want to print the short side of the image smaller than the width of paper loaded in the printer, they should have selected the other option to conserve paper.
            calcDimens=0
            flipImg=0
    else: #auto define width or height
        widthHeight=easygui.indexbox(msg='Do you want to specify the width or the height? \n Note that the width is defined as the longer side while the height is specified as the shorter side.', title='Width or Height', choices=('Width','Height'), image="widthHeight.jpg")
        if widthHeight == 0: #manual define width, calculate height
            imageWidth=easygui.integerbox(msg='Please enter the width of the image you would like to print in inches. \n This is the longer side of the image', title='Image Width', default=17, lowerbound=1, upperbound=paperWidth, image="widthHeight.jpg")
            calcDimens=1 #note to calculate height later
        else: #manual define height, calculate width
            imageHeight=easygui.integerbox(msg='Please enter the height of the image you would like to print in inches. \n This is the shorter side of the image', title='Image Height', default=11, lowerbound=5, upperbound=paperWidth, image="widthHeight.jpg") #max height is 80 inches because thats all my LUT supports
            calcDimens=2 #note to calculate width later

else: #specific DPI
    printPreferences=2

# choose an image to open
myfile = easygui.fileopenbox(msg="Choose an Image", default=r"C:\\Users\\Jordan\\downloads\\")

# get file information
filePath, fileName=os.path.split(os.path.abspath(myfile))
filePathWithName, fileExtension = os.path.splitext(myfile) 

fileName=fileName[0:-len(fileExtension)] #remove the .pdf extension

if fileExtension == ".pdf": #render pdf to image if pdf
    fileExtension=renderPDF(myfile,paperWidth)
elif fileExtension == ".svg":
    #program doesnt support svg yet
    easygui.msgbox(msg='Sorry, this program does not support SVG files yet. Please convert to PDF or JPG and try again.', title=' ', ok_button='OK', image=None, root=None)
    sys.exit()

else : #if not pdf, copy file to local dir -> out.(extension)
    shutil.copy(myfile, "out"+fileExtension)

# get image
Image.MAX_IMAGE_PIXELS = None #remove image size limit
filepath = "out"+fileExtension
img = Image.open(filepath)

# get width and height in pixels
width = img.width
height = img.height

#rotate image to be horizontal if needed
if flipImg == 1:
    if width < height:
        img = img.rotate(90, expand=True)
        width = img.width
        height = img.height
else: #flip image to be vertical if needed
    if width > height:
        img = img.rotate(90, expand=True)
        width = img.width
        height = img.height

#overwrite image with rotated image
img.save(filepath)

if printPreferences == 0:
    sideChoice=easygui.indexbox(msg='Do you want the short side of the image or the long side of the image printed across the width of the paper?', title='Paper Width ', choices=('Long', 'Short'), image="longShortSide.jpg")
    sidea=0
    sideb=0
    if sideChoice == 1: #shorter sidea
        if width > height:
            sidea=height
            sideb=width
        else:
            sidea=width
            sideb=height
    elif sideChoice == 0: #larger sidea
        if width > height:
            sidea=width
            sideb=height
        else:
            sidea=height
            sideb=width

    dpi=sidea/paperWidth
    imageHeight=sideb/dpi
    imageWidth=paperWidth
    

elif printPreferences==1:
    if calcDimens==0: #manual define width and height
        dpi1=width/imageWidth
        dpi2=height/imageHeight
        dpi=max(dpi1,dpi2)
        
    elif calcDimens==1: #manual define width, calculate height
        dpi=width/imageWidth
        imageHeight=height/dpi
        if imageHeight > 80: #max height is 80 inches because thats all my LUT supports
            easygui.msgbox(msg="To print this image with a width of "+str(imageWidth)+"in you need paper that is "+str(imageHeight)+"in tall. The maximum paper height is 80in due to an artificial limitation imposed by the developer of the program. Please email jlstone@hmc.edu if you ran into this error and consult the legacy manual on how to continue.", title=' ', ok_button='OK', image=None, root=None)
            import sys
            sys.exit()
        
    else: #manual define height, calculate width
        dpi=height/imageHeight
        imageWidth=int(width/dpi)
        if imageWidth > paperWidth:
            img = img.rotate(90, expand=True)
            img.save(filepath)
            temp=imageHeight #flip imageHeight and imageWidth
            imageHeight=imageWidth
            imageWidth=temp
            if imageHeight > 80: #max height is 80 inches because thats all my LUT supports
                easygui.msgbox(msg="This image was flipped beause the height specified resulted in a width ("+str(imageHeight)+"in) which was wider than the width of the paper. While the software will normally deal with this issue, the software cannot make the printer extrude anything that is >80 inches long due to programming difficulties. Please restart the program and either reduce the desired height or follow the manual printing method found at the bottom of the LFP manual. Please email jlstone@hmc.edu if you ran into this issue so that I may add more values than 80 inches.", title=' ', ok_button='OK', image=None, root=None)
                import sys
                sys.exit()

elif printPreferences==2: #manual define dpi
    userOK=1
    while userOK == 1:
        lowDPI=math.ceil(width/paperWidth)
        dpi=easygui.integerbox(msg='Please enter the DPI you would like to print at. \n For a high quality print, enter a DPI between 150 and 300. Lower your DPI for a larger print.', title='DPI', default=300, lowerbound=40, upperbound=10000, image=None)
        if dpi<lowDPI:
            imageWidth=height/dpi #flip imageHeight and imageWidth
            imageHeight=width/dpi
            if imageWidth > paperWidth:
                easygui.msgbox(msg="To print at this DPI the paper needs to be "+str(imageWidth)+"in wide. The maximum paper width is "+str(paperWidth)+"in. Please enter a DPI that will result in a paper width less than "+str(paperWidth)+"in.", title=' ', ok_button='OK', image=None, root=None)
                continue
        else:
            imageWidth=width/dpi
            imageHeight=height/dpi
            if imageHeight > 80:
                easygui.msgbox(msg="To print at this DPI the paper needs to be "+str(imageHeight)+"in tall. The maximum paper height is 80in. Please enter a DPI that will result in a paper height less than 80in.", title=' ', ok_button='OK', image=None, root=None)
                continue
        userOK=easygui.indexbox(msg="Using a DPI of "+str(dpi)+" will result in an image that is "+str(imageWidth)+"in wide and "+str(imageHeight)+"in tall. Is this size good or do you want to choose a different DPI?.", title=' ', choices=('Good', 'Change DPI'), image=None)
        if dpi<lowDPI and userOK==0:
            #rotate image
            img = img.rotate(90, expand=True)
            img.save(filepath)

imageWidth=int(imageWidth)
imageHeight=math.ceil(imageHeight)
today = date.today()
date=today.strftime("%b-%d-%Y")
dpi=int(dpi)

#warn if DPI is too low
if dpi < 100:
    easygui.msgbox(msg="The DPI of this image is "+str(dpi)+". For best results use a DPI ~150-300. Beware that results may not look the best.", title=' ', ok_button='OK', image=None, root=None)


newFileName="PRINT_ME_"+fileName+"_"+str(dpi)+"dpi_"+str(imageWidth)+"in_"+str(imageHeight)+"in"+fileExtension

newFilePath=filePath+"\\"+newFileName
shutil.copy("out"+fileExtension,newFilePath) #copy file to local dir with new name

configLocation="C:\ProgramData\EPSON\EPSON SC-P8000 Series\E_31CL01LE.UCF"
filename="E_31CL01LE.UCF"

with open(filename, 'r+b') as f:
    newDec = endcodedDistance(imageHeight)
    newbytes = newDec.to_bytes(2, byteorder='big')
    f.seek(0X0004173C)
    f.write(newbytes)

    newDec = endcodedDistance(imageWidth)
    newbytes = newDec.to_bytes(2, byteorder='big')
    f.seek(0X00041738)
    f.write(newbytes)

    if (imageHeight>imageWidth):
        f.seek(0X00041736)
        newDec = 0x00
        newbytes = newDec.to_bytes(2, byteorder='big')
        f.write(newbytes)
    else:
        f.seek(0X00041736)
        newDec = 0x01
        newbytes = newDec.to_bytes(2, byteorder='big')
        f.write(newbytes)
shutil.copy("E_31CL01LE.UCF",configLocation) #copy file to config location

trash, fileNames=os.path.split(os.path.abspath(newFileName)) #get file name without path
heightMsg='The custom paper size has been saved. Please use CALCULATED PAPER as the paper size in the print dialog box. Remember to Refresh'
# dpiMsg='The approximate dpi of the image that nomacs should display before continuing is: '+str(dpi)+' dpi.'
dpiMsg="" #no longer needed
fileSavedMsg='The image has been saved as '+fileNames+' at '+filePath+'.'
openFile=easygui.indexbox(msg=heightMsg+"\n\n"+dpiMsg+"\n\n"+fileSavedMsg, title=' ', choices=('OK', 'Open File Location'), image=None)
if openFile==1:
    os.startfile(filePath)