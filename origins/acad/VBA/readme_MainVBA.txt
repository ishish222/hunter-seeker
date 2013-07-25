Readme for AutoCAD Operations Samples

This Readme contains information about the AutoCAD® VBA samples that include 
AutoCAD® functionality.

================== 
To Use the Samples
==================

The samples are AutoCAD VBA project (DVB) files. See the "Description of the 
Samples" section below for any specific information about the samples.

To use an AutoCAD VBA sample or view its code
---------------------------------------------


1. Start AutoCAD and drag a sample into AutoCAD.

2. On the AutoCAD Tools menu, click Macro > Macros.

3. In the Macros dialog box, in the Macro list, select a macro to use or view.

4. Do one of the following:

   * To use the selected macro, click Run.

   * To view the code of the selected macro in the VBA Integrated Development 
     Environment (VBA IDE), click Edit.

===========================
Descriptions of the Samples
===========================

-attext.dvb: This sample extracts data from a drawing and puts the data into 
Microsoft® Word and Microsoft® Excel files. This sample requires Microsoft® Office 97 
or Microsoft® Office 2000. Before using this sample, open the attrib.dwg sample 
drawing file in the ExtAttr directory. To use the sample, run the macro named 
Extract_Dialog.

-acad_cg.dvb: This sample runs APIs involving drawings, windows, object properties, 
user input, selection sets, layers, preferences, plotting, viewports, the utility 
object, and Xdata.

-BlockReplace.dvb: This sample replaces block references in model space. 

-chplywid.dvb: This sample changes the width of all polylines in a drawing.

-cntrline.dvb: This sample adds centerlines to circles, arcs, and ellipses.

-drawline.dvb: This sample draws a line.

-ExcelLink.dvb: This sample creates an Excel workbook that contains a bill of materials (BOM) for the current drawing. After the BOM has been edited in Excel, the data from the workbook can be passed back into the drawing. This sample requires Microsoft Office 97. Before using this sample, open the attrib.dwg sample drawing file in the ExtAttr directory. Before updating a drawing, make the workbook with the BOM active. 

-ibeam3d.dvb: This sample creates and dynamically adjusts a 3D solid representing an I-beam. The solid is made from input parameters adjusted by mathematical calculations based on Moment of Inertia and Section Modulus values for the beam.

-Map2Globe.Dwg: This sample generates 3D Polylines on a Sphere from 2D polylines.

-Menu.dvb: This sample runs APIs involving the MenuGroups and MenuBar collections.

-ObjectTracker.dvb: This sample runs APIs involving the XRecord object and shows how to track information and store it in a dictionary containing an XRecord. For example, to use the sample,
start AutoCAD, load the DVB file, draw a line, modify its color, run the macro, and select the line. To view tracking information for the drawing, select the drawing background.

-Tower.dwg: This sample draws an electrical power transmission tower and computes tables of weights and foundation reactions. The sample also does a stress analysis of the tower structure and displays the deformed tower for given loads. The forces in each member can be queried interactively. To use the sample, select the tabs in the Tower dialog box, from left to right. In each tab, use the controls before moving to the next tab. Users can change the shape, size, materials values, cross sections, or position of the Tower, or the direction or value of the applied loads. A set of 8 blocks is defined in the DWG and are provided to place different node loads (FXPT, FXPC, FXNT, FXNC, FYPT, FYPC, FYNT, FYNC) where F=Nodal Force; X=on X axis; Y=on Y axis; P=Positive; N=Negative; T=Tension; and C=Compresion. After the structure model is complete, you can run the stress analysis using the Deformation tab. Each analysis generates a results.txt file, with node displacements and bar forces. The source code is commented in Spanish.

-txtht.dvb: This sample changes the height of the text in a drawing.

