*/
using System;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ImageGlass.Library.WinAPI {
    public static class PrintService {
        [ComImport]
        [Guid("00000122-0000-0000-C000-000000000046")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IDropTarget {
            int DragEnter(
                [In] System.Runtime.InteropServices.ComTypes.IDataObject pDataObj,
                [In] int grfKeyState,
                [In] Point pt,
                [In, Out] ref int pdwEffect);

            int DragOver(
                [In] int grfKeyState,
                [In] Point pt,
                [In, Out] ref int pdwEffect);

            int DragLeave();

            int Drop(
                [In] System.Runtime.InteropServices.ComTypes.IDataObject pDataObj,
                [In] int grfKeyState,
                [In] Point pt,
                [In, Out] ref int pdwEffect);
        }


        /// <summary>
        /// Open Print Pictures dialog
        /// </summary>
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
}