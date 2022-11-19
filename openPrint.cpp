using System;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
#include <string>
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(printStuff)
{
    // Add regular functions to the module.
    def("printPictures", OpenPrintPictures);
}


namespace {
    /// <param name="filename">File to print</param>
public static void OpenPrintPictures(string filename) {
    var dataObj = new DataObject(DataFormats.FileDrop, new string[] { filename });
    var memoryStream = new MemoryStream(4);
    var buffer = new byte[] { 5, 0, 0, 0 };

    memoryStream.Write(buffer, 0, buffer.Length);
    dataObj.SetData("Preferred DropEffect", memoryStream);

    var CLSID_PrintPhotosDropTarget = new Guid("60fd46de-f830-4894-a628-6fa81bc0190d");
    var dropTargetType = Type.GetTypeFromCLSID(CLSID_PrintPhotosDropTarget, true);
    var dropTarget = (IDropTarget)Activator.CreateInstance(dropTargetType);

    dropTarget.Drop(dataObj, 0, new Point(), 0);
}
}