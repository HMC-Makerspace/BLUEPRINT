using System;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Diagnostics;
using System.Windows;

namespace PrintGUI // Note: actual namespace depends on the project name.
{
    internal class Program
    {
        [DllImport("user32.dll")]
        private static extern bool SetForegroundWindow(IntPtr hWnd);

        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("No file specified");
                return;
            }

            int num_open_start = NumOpenPrintWindows();

            System.Threading.Thread.Sleep(50);

            Console.WriteLine("Printing file:");
            Console.WriteLine(args[0]);
            OpenPrintPictures(args[0]);
            Console.WriteLine("Done");

            // Bring the print window to the foreground
            Process[] processes = Process.GetProcesses();

            foreach (var process in processes)
            {
                if (process.MainWindowTitle
                           .IndexOf("Print Pictures", StringComparison.InvariantCulture) > -1)
                {
                    SetForegroundWindow(process.MainWindowHandle);
                    break;
                }
            }

            int num_times_wrong = 0;

            while (true)
            {
                int num_open_now = NumOpenPrintWindows();

                if (num_open_now == num_open_start)
                {
                    num_times_wrong++;
                } else
                {
                    num_times_wrong = 0;
                }

                if (num_times_wrong == 10000)
                {
                    return;
                }
            }
        }

        public static int NumOpenPrintWindows()
        {
            string[] result = new string[50];
            int count = 0;
            Process[] processes = Process.GetProcesses();
            foreach (var process in processes)
            {
                if (process.MainWindowTitle
                           .IndexOf("Print Pictures", StringComparison.InvariantCulture) > -1)
                {
                    result[count] = process.MainWindowTitle;
                    count++;
                }
            }

            return count;
        }

        [ComImport]
        [Guid("00000122-0000-0000-C000-000000000046")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IDropTarget
        {
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
        public static void OpenPrintPictures(string filename)
        {
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