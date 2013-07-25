Readme for the External Call Sample

This Readme contains information about the External Call sample. 

=================
To Use the Sample
=================

The External Call sample shows how to pass data to an external application from AutoCAD® VBA. The sample includes an AutoCAD VBA macro and a Microsoft® Visual Basic® component. The Visual Basic component calculates the volume of a cube 
using data from the AutoCAD VBA macro.  

To use the External Call sample
-------------------------------
1. Open a DOS command window, and in the directory containing externalfunction.dll, 
   enter the following command: 

   Regsvr32 externalfunction.dll
   
   Note: If you are using Windows Vista, you need to run DOS command window as Administrator. 
   You can run DOS command window with full admin rights by right-clicking on the
   executable or shortcut and choosing "Run as Administrator".

2. Start AutoCAD®, load the ExternalCall.dvb file, and run its macro. 

3. In the Calculate the Volume of a Cube dialog box, enter the dimensions of a 
   cube and click Calculate. 

   The Cube volume is displayed.

To view the code from the External Call sample
----------------------------------------------
1. On the AutoCAD Tools menu, click Macro > Macros.

2. In the Macros dialog box, in the Macro list, select the External Call macro. 

3. In the Macros dialog box, click Edit. 

   The code from the External Call sample is displayed.

4. In Windows® Explorer, double-click MyVB6DLL.vbp. 

   In Visual Basic, the code for the Visual Basic component is displayed.
