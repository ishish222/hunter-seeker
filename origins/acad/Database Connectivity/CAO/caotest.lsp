;;;                                                                    ;
;;; CAOTEST.LSP                                                        ;
;;;                                                                    ;
;;;  Copyright 1987, 1988, 1990, 1992, 1994, 1996, 1997, 1998          ;
;;;  by Autodesk, Inc. All Rights Reserved.                            ;
;;;                                                                    ;
;;;  You are hereby granted permission to use, copy and modify this    ;
;;;  software without charge, provided you do so exclusively for       ;
;;;  your own use or for use by others in your organization in the     ;
;;;  performance of their normal duties, and provided further that     ;
;;;  the above copyright notice appears in all copies and both that    ;
;;;  copyright notice and the limited warranty and restricted rights   ;
;;;  notice below appear in all supporting documentation.              ;
;;;                                                                    ;
;;;  Incorporation of any part of this software into other software,   ;
;;;  except when such incorporation is exclusively for your own use    ;
;;;  or for use by others in your organization in the performance of   ;
;;;  their normal duties, is prohibited without the prior written      ;
;;;  consent of Autodesk, Inc.                                         ;
;;;                                                                    ;
;;;  Copying, modification and distribution of this software or any    ;
;;;  part thereof in any form except as expressly provided herein is   ;
;;;  prohibited without the prior written consent of Autodesk, Inc.    ;
;;;                                                                    ;
;;;  AUTODESK PROVIDES THIS SOFTWARE "AS IS" AND WITH ALL FAULTS.      ;
;;;  AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF           ;
;;;  MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK,       ;
;;;  INC. DOES NOT WARRANT THAT THE OPERATION OF THE SOFTWARE          ;
;;;  WILL BE UNINTERRUPTED OR ERROR FREE.                              ;
;;;                                                                    ;
;;;  Restricted Rights for US Government Users.  This software         ;
;;;  and Documentation are provided with RESTRICTED RIGHTS for US      ;
;;;  US Government users.  Use, duplication, or disclosure by the      ;
;;;  Government is subject to restrictions as set forth in FAR         ;
;;;  12.212 (Commercial Computer Software-Restricted Rights) and       ;
;;;  DFAR 227.7202 (Rights in Technical Data and Computer Software),   ;
;;;  as applicable.  Manufacturer is Autodesk, Inc., 111 McInnis       ;
;;;  Parkway, San Rafael, California 94903.                            ;
;;;                                                                    ;
;;;                                                                    ;
;;;--------------------------------------------------------------------;
;;; General Note:                                                      ;
;;;  This example demonstrates the ability to use the Connectivity     ;
;;;  Automation Object (CAO) API to retrieve linktemplate and link     ;
;;;  information from the drawing, to create/update links, and to use  ;
;;;  ADO to connect to external databases.                             ;
;;;                                                                    ;
;;;  The function names beginning with "cmd" provide similar           ;
;;;  functionality to the same-named routines in the CAOtest.dvb VBA   ;
;;;  code to facilitate comparison of the VBA and LISP examples.       ;
;;;--------------------------------------------------------------------;
;;;                                                                    ;
;;; Example use: Open a drawing with labels, or use the dbConnect      ;
;;; Manager user interface to create a label.  Then, run this CAOTEST  ;
;;; example.  Click the "Update Link" button to change the record to   ;
;;; which this label is linked.  Click the "Reload Labels" button and  ;
;;; you should see the label update to the newly linked record.        ;
;;;--------------------------------------------------------------------;



;;;--------------------------------------------------------------------;
;;; This is the main function for this example.  It loads the ActiveX  ;
;;; dlls, imports the type-libraries from the ADO and CAO dlls, creates;
;;; an ADODB.RecordSet object and a CAO.DbConnect object then it calls ;
;;; the CAOTest dialog.                                                ;
;;;--------------------------------------------------------------------;
(defun c:caotest( / lstLT  linkTemplates linkSel thisvalue thisvaluetype 
		    strkeyname thislink slinktemplate field fields fieldsIndex
		    count strWhere strKeyValue lstAllRows dbConnect rs adoconnect)

  ;; Load the ActiveX stuff for Visual LISP
  (vl-load-com)

  ;; Save a reference to the active drawing document.
  (setq acadObj (vlax-get-acad-object)
	acadDoc (vla-get-ActiveDocument acadObj)  ) ; This acadDoc global is used throughout this file.

  (setq gCmnFiles (getenv "COMMONPROGRAMFILES") 
	gCaoTlb (strcat gCmnFiles "\\AUTODESK SHARED\\" "cao16enu.tlb")
	gADO-DLLpath (strcat gCmnFiles "\\system\\ado\\msado15.dll"))


  (if (null (findfile gADO-DLLpath))
      (progn
	(alert (strcat "Could not load ADO type library from: " gADO-DLLpath)) 
	(exit)
      )
  )
  ;; Import the ADO type library
  ;; Note: We must provide prefixes for these methods and properties because some conflict
  ;; with protected LISP symbols such as APPEND().
  (if (null adok-adStateOpen)            ; Check that ADO type library has not yet been loaded.
      (vlax-import-type-library :tlb-filename gADO-DLLpath
     	  :methods-prefix "adom-"
 	  :properties-prefix "adop-"
 	  :constants-prefix "adok-"
      )
  )


  ;; Import the CAO type library.
  (if (null caom-GetLinkTemplates)           ; Check that CAO type library has not yet been loaded.
      (vlax-import-type-library :tlb-filename gCaoTlb
     	  :methods-prefix "caom-"
 	  :properties-prefix "caop-"
 	  :constants-prefix "caok-"
   ))


  ;; Define a global variable, dbConnect
  (cmdInitDbConnect_Click)


  (if rs
      (if (/= adok-adStateClosed (vlax-get-property rs "State"))
	  (vlax-invoke-method rs "CLOSE")))
  (setq rs nil)		; In VBA, setting recordSet to 'Nothing' disconnects. Not needed in LISP?
  (setq rs (vlax-create-object "ADODB.Recordset"))


  (caotest-dialog)

)


 
;;;--------------------------------------------------------------------;
;;; Export these functions so that they will be available to use from the
;;; drawing document namespace even when this routine gets compiled
;;; into a separate-namespace .vlx application
;;;--------------------------------------------------------------------;
(mapcar 
 'vl-doc-export '( c:caotest
		   cmdInitDbConnect_Click
		   cmdConnectDataSource_click
		   cmdErrors_Click

		   cmdGetDrawings_Click
		   cmdGetLinkTemps_click
		   cmdGetLinkTempData_click

		   cmdGetLinks_click

		   cmdMakeLink_click
		   cmdUpdateLink_click
		   cmdReloadLabels_click

		   cmdShowLinkData_click
		   cmdShowLinkedRow_click
		   cmdShowTable_click
		 )
)



;;;--------------------------------------------------------------------;
;;; This is the main dialog-handling function for the "Select" dialogs.;
;;;  This requires that the caotest.dcl be on the working path.
;;;--------------------------------------------------------------------;
(defun selectItem( lstItems dcl-fileName dialogName / dcl_id LT lstLinkTemplates selectedItem)

  (setq dcl_id (load_dialog dcl-filename))      ; Load the DCL file 

  (if (not (new_dialog dialogName dcl_id))      ; Initialize the dialog 
      (progn
	(princ (strcat "\n " dcl-filename " dialog not found.\n"))
	(exit)                                   ; Exit if this doesn't work
      )
  )


  ;; Construct list of items
  (start_list "selections")                  ; Specify the name of the list box 
      (mapcar 'add_list lstItems)            ; Specify the AutoLISP list 
  (end_list)

  ;;
  ;;
  (defun getItemSelection()
    (setq selectedItem (nth (atoi $value) lstItems)  )
    (if (equal 4 $reason)		; Then user double-clicked item.
	(done_dialog))
  )

  ;;
  ;; Confirms that user selected an item and that the item exists in list
  ;;
  (defun check_i()
    (if (member selectedItem lstItems)
      (progn 
        (set_tile "error" "") 
        T
      )
      (progn 
        (set_tile "error" "Please select an item.")
        nil
      )
    )
  )

   (action_tile "selections" "(getItemselection)")
  (action_tile "accept"     "(if (check_i)(done_dialog 1))")
  (action_tile "cancel"     "(setq selectedItem nil)(done_dialog 0)(exit)")


  (start_dialog)                             ; Display the dialog box 

  (unload_dialog dcl_id)                     ; Unload the DCL file 

  (princ "\n")

  selectedItem				; Return this value.
)


;;;--------------------------------------------------------------------;
;;; This is the main dialog-handling function for the CAO Test dialog. ;
;;;  This requires that the caotest.dcl be on the working path.        ;
;;;--------------------------------------------------------------------;
(defun caotest-dialog (  / dcl-filename dialogName dcl_id retval what_next error_string)
  (setq lstOutput '(" ")			; A global
	dcl-filename "caotest.dcl"
	dialogName   "caotest_dialog")
  (setq status_string "")
  (setq dcl_id (load_dialog dcl-filename))      ; Load the DCL file 
  
  ;; The linkType checkboxes
  ;; Default to have them all checked, so set corresponding globals:
  (setq bENTITYLINK    T
	bFSLABEL       T
	bATTACHEDLABEL T)

  (setq what_next 2)
  (while (>= what_next 2)                    ;Begin display loop 

    (if (not (new_dialog dialogName dcl_id))	; Initialize dialog
	(progn
	  (princ (strcat "\n " dcl-filename " dialog not found.\n"))
	  (exit)                                   ; Exit if this doesn't work
        )
    )


    (if (> (strlen status_string) 80)
	(setq status_string (strcat (substr status_string 1 78) "...)")))
    (set_tile "ID_STATUS" status_string)	; This status_string global is set in each routine.


    ;; Construct list of items
    (start_list "ID_RECORDS")		; Specify the name of the list box 
    (mapcar 'add_list lstOutput)	; Specify the LISP list 
    (end_list)


    ;; The linkType checkboxes
    (action_tile "ID_TOGGL_ENTITYLINK" "(if (= $value \"1\") (setq bENTITYLINK T) (setq bENTITYLINK nil) )" )
    (action_tile "ID_TOGGL_FSLABEL" "(if (= $value \"1\") (setq bFSLABEL T) (setq bFSLABEL nil ) )" )
    (action_tile "ID_TOGGL_ATTACHEDLABEL" "(if (= $value \"1\") (setq bATTACHEDLABEL T) (setq bATTACHEDLABEL nil ) )" )
    (action_tile "ID_CMDGETLINKS_CLICK" "(done_dialog 4)") ;4 == (cmdGetLinks_click nil) Set done_dialog to >=2 so we can reinvoke dialog in while() loop


    ;; Set state of linktype checkboxes to the accociated global boolean variables.
    (set_tile "ID_TOGGL_ENTITYLINK" (if bENTITYLINK "1" "0"))
    (set_tile "ID_TOGGL_FSLABEL" (if bFSLABEL "1" "0"))
    (set_tile "ID_TOGGL_ATTACHEDLABEL" (if bATTACHEDLABEL "1" "0"))



    (action_tile "ID_CMDINITDBCONNECT_CLICK" "(cmdInitDbConnect_Click)" )
    (action_tile "ID_CMDCONNECTDATASOURCE_CLICK" "(done_dialog 2)") ;2==(cmdConnectDataSource_click nil)
    (action_tile "ID_CMDERRORS_CLICK" "(done_dialog 3)" ) ;3 == (cmdErrors_Click)" )
    ;;(action_tile "ID_RESETCAOERRORS_CLICK" "(done_dialog 3)" ) ;3 == (cmdErrors_Click)" )


    ;; Links group
    (action_tile "ID_CMDMAKELINK_CLICK" "(done_dialog 5)" ) ;5==(cmdMakeLink_click)
    (action_tile "ID_CMDUPDATELINK_CLICK" "(done_dialog 6)" ) ;6 = (cmdUpdateLink_click)
    (action_tile "ID_CMDRELOADLABELS_CLICK" "(done_dialog 12)") ;12==(cmdReloadLabels_click)


    ;; Link Templates
    (action_tile "ID_CMDGETLINKTEMPS_CLICK" "(done_dialog 7)" ) ;7 ==(cmdGetLinkTemps_click)
    (action_tile "ID_CMDGETLINKTEMPDATA_CLICK" "(done_dialog 8)" ) ;8 ==(cmdGetLinkTempData_click)"


  
    ;; Show data
    (action_tile "ID_CMDSHOWLINKDATA_CLICK" "(done_dialog 9)" ) ; 9==(cmdShowLinkData_click nil nil nil)
    (action_tile "ID_CMDSHOWLINKEDROW_CLICK" "(done_dialog 10)" ) ;10 = (cmdShowLinkedRow_click nil nil)
    (action_tile "ID_CMDSHOWTABLE_CLICK" "(done_dialog 11)" ) ;11==(cmdShowTable_click nil)


    (action_tile "ID_BUTT_EXIT" "(setq retval 'CANCEL)(disconnectADOConnect)(term_dialog)" ) ; retval not currently used

    (action_tile "ID_RECORDS" "(setq selectedItem (atoi $value))"  )


    (setq what_next (start_dialog))          ;Display dialog box 

    (cond
      ((= what_next 2)
       (cmdConnectDataSource_click nil)
      )
      ((= what_next 3)
       (cmdErrors_click)		; Display each Error object code/description in CAO.Errors collection
       (resetCAOErrors)			; Reset CAO Errors collection by calling it's Clear() method.
      )
      ((= what_next 4)
       (cmdGetLinks_click nil)
      )
      ((= what_next 5)
       (cmdMakeLink_click)
      )
      ((= what_next 6)
       (cmdUpdateLink_click)
      )
      ((= what_next 7)
       (cmdGetLinkTemps_click)
      )
      ((= what_next 8)
       (cmdGetLinkTempData_click)
      )
      ((= what_next 9)
       (cmdShowLinkData_click nil nil nil)
      )
      ((= what_next 10)
       (cmdShowLinkedRow_click nil nil)
      )
      ((= what_next 11)
       (cmdShowTable_click nil)
      )
      ((= what_next 12)
       (cmdReloadLabels_click)
      )
    )
  );_while (>= what_next 2)

  (unload_dialog dcl_id)                     ; Unload the DCL file 

  (princ "\n")

  retval
)





;;;--------------------------------------------------------------------;
;;; Return a single VLA ObjectID based on prompting user to select a   ;
;;; single object on screen.                                           ;
;;;--------------------------------------------------------------------;
(defun getOneObjID( / ss selectedEname selectedObj selectedObjID )
  (setq ss (ssadd))		; Initialize selection set.
  (while (= 0 (sslength ss)) ; continue prompting until user picks 1 object
    (princ "\nSelect one linked object.") 
    (setq ss (ssget))
    (if (/= 1 (sslength ss))	; continue prompting until user picks 1 object
	(progn
	  (setq ss (ssadd))		; Initialize selection set.
	  (princ "\nSelect only one linked object.") 
	)
    )
   )
  (setq selectedEname (ssname ss 0))
  (setq selectedobj (vlax-ename->vla-object selectedEname))
  (setq selectedObjID (vlax-get-property selectedObj "ObjectID"))
  selectedObjID
)




;;;--------------------------------------------------------------------;
;;; Return a lisp SafeArray filled with VLA ObjectIDs based on         ;
;;; prompting the user to select objects on screen.                    ;
;;;--------------------------------------------------------------------;
(defun getObjIds( / ss lstObjIDs vlaObj ssCount ssIndex ename
		    entid objid objids)

  (setq ss (ssadd))		; Initialize selection set.

  (while (= 0 (sslength ss))	; continue prompting until user picks >1 object
    (setq ss (ssget))
  )

  (setq lstObjIDs nil
	vlaObj nil
	ssCount (sslength ss)
	ssIndex 0
  )

  ;; Establish an empty SafeArray to hold ObjectIDs
  (setq ObjIDs (vlax-make-safearray vlax-vbLong (cons 0 ssCount)))

  (while (< ssIndex ssCount)
    (setq entID (ssname ss ssIndex))
    (setq vlaobj (vlax-ename->vla-object entID))
    (setq objID (vlax-get-property vlaobj "ObjectID"))
    (setq lstobjIDs (cons objID lstobjIDs))
    
    (vlax-safearray-put-element ObjIDs ssIndex objID)
    
    (setq ssIndex (1+ ssIndex))
  )

  ObjIDs				; return this value.
)







;;;--------------------------------------------------------------------;
;;; c:basictest() is an example of code which will be common to many   ;
;;; CAO test code.   This is not used by CAOTEST dialog.               ;
;;;--------------------------------------------------------------------;
(defun c:basictest( / LTs LTsCount LT dbConnect)
  (vl-load-com)
  (if (null GetLinkTemplates)
      (vlax-import-type-library :tlb-filename gCaoTlb)
  )
  (setq dbConnect (vla-getinterfaceobject (vlax-get-acad-object) "CAO.DbConnect.16"))
  (setq LTs (vlax-invoke-method dbConnect "GetLinkTemplates"))
  (setq LTsCount (vlax-get-property LTs "Count"))
  (if (= 0 LTsCount)
      (vlax-dump-object LTs)
    (progn
      (setq LT (vlax-invoke-method LTs "Item" 0))
      (vlax-dump-object LT)
    )
  )
)


;;;--------------------------------------------------------------------;
;;; c:dumpLinks() is an example of code which will be common to many   ;
;;; CAO test code.   This is not used by CAOTEST dialog.               ;
;;;--------------------------------------------------------------------;
(defun c:dumpLinks( / acadObj acadDocs linkSel linkTypes dbConnect)
  (vl-load-com)
  (setq acadObj (vlax-get-acad-object))
  (setq acadDocs (vlax-get-property acadObj "Documents"))

  (if (null caom-GetLinkTemplates)           ; Check that CAO type library has not yet been loaded.
      (vlax-import-type-library :tlb-filename gCaoTlb
     	  :methods-prefix "caom-"
 	  :properties-prefix "caop-"
 	  :constants-prefix "caok-"
   ))

  (setq dbconnect (vla-getinterfaceobject (vlax-get-acad-object) "CAO.DbConnect.16"))

  ;; Make GetLinks() return all linktypes.
  (setq linkTypes (+ caok-kEntityLinkType caok-kFSLabelType caok-kAttachedLabelType) )

  ;; Could use vlax-invoke-method, but you cannot use error-handling.
  ;;(setq linkSel  (vlax-invoke-method dbConnect "GetLinks" (getLinkTemplate) (getObjIDs) linkTypes))
				     
  ;; But, it's probably best to use the vl-catch-all-apply() function to apply the
  ;; caom-GetLinks function wrapper created during import-type-library.  This way,
  ;; We can test the return value to see if it is an ActiveX error message object.
  (setq linkSel (vl-catch-all-apply 'caom-GetLinks 
				    (list dbConnect (getLinkTemplate) (getObjIDs)  linkTypes ) 
				    ))
			
  (if (vl-catch-all-error-p linkSel)
      (progn
	(print (strcat "An ActiveX error occurred: " (vl-catch-all-error-message linkSel) ))
	(exit)
      )
    ;; Otherwise, continue
    (dumpLinks linkSel)
  )
)

;;;--------------------------------------------------------------------;
;;; dumpLinks() dump all link objects in the linkSel collection        ;
;;; This is not used by CAOTEST dialog.                                ;
;;;--------------------------------------------------------------------;
(defun dumpLinks( linkSel / index KeyValues count)
  (vlax-for thisLink linkSel
	    (princ (vlax-dump-object thisLink))
             (setq index 0
                   KeyValues (vlax-get-property thisLink "KeyValues")
                   count (vlax-get-property KeyValues "Count"))
             (vlax-for thisKeyValue KeyValues
 	              (while (< index count)
 			     (princ (vlax-dump-object thisKeyValue))
 			     (setq index (1+ index))
                       )
             )
  )
)


;;;--------------------------------------------------------------------;
;;; dumpLinks2() dump all links on all linked objects using all link   ;
;;; templates in the current document.  This is not used by CAOTEST    ;
;;; dialog.
;;;--------------------------------------------------------------------;
(defun c:dumpLinks2( / LTs linkTemplate linkSel keyValues keyDescriptions keyDescsCount index)
  (vl-load-com)
  (if (null GetLinkTemplates)
      (vlax-import-type-library :tlb-filename gCaoTlb)
  )
  (setq dbConnect (vla-getinterfaceobject (vlax-get-acad-object) "CAO.DbConnect.16"))
  (setq LTs (vlax-invoke-method dbConnect "GetLinkTemplates")) ; All linkTemplates in current drawing.

  (vlax-for thisLT LTs  
	    ;; Dump the Link Template
	    (princ (vlax-dump-object thisLT))

	    ;; Dump each keydescription in this linkTemplate keydescriptions collection
	    ;; This example uses the Item() method of the keyDescriptions object.
	    ;; But, we could have also done a vlax-for() loop on this collection.
	    (setq keyDescriptions (vlax-get-property thisLT "KeyDescriptions")
		  keyDescsCount (vlax-get-property KeyDescriptions "Count"))
	    (setq index 0)
	    (while (< index keyDescsCount)
	      (princ (strcat "\nkeyDescriptions.Item[" (itoa index) "]"))
	      (princ (vlax-dump-object (vlax-invoke-method keyDescriptions "item" index)))
	      (setq index (1+ index))
	    )

	    ;; Get all links in this drawing which use this link template
	    (setq linkSel (vlax-invoke-method DbConnect "Getlinks" thisLT ))
	    (vlax-for thisLink linkSel
		      (princ (vlax-dump-object thisLink))
		      (setq KeyValues (vlax-get-property thisLink "KeyValues"))
		      (vlax-for thisKey KeyValues
				(princ (vlax-dump-object thisKey))
		      )
            )
  )
)




;;;--------------------------------------------------------------------;
;;; printFieldNames() accepts an ADODB.RecordSet object and returns a  ;
;;; string containing the column names for this RecordSet's Fields.    ;
;;;--------------------------------------------------------------------;
(defun printFieldNames( rs bPrint / strColumnNames fields fieldsIndex thisField padStr
			   fieldsCount thisFieldDefinedSize thisFieldName )
  
  (setq strColumnNames ""
	padStr         "")

  (setq fields (vlax-get-property rs "Fields")
	fieldsCount (1- (vlax-get-property fields "Count")) ; zero-based index
	fieldsIndex 0)

  (while (<= fieldsIndex fieldsCount)
    (setq thisField (vlax-get-property fields "item" fieldsIndex)) ; Item is a property not a method for recordSet fields...
    (setq thisFieldDefinedSize (vlax-get-property thisField "DefinedSize")
	  thisFieldName (vlax-get-property thisField "Name"))

    
    (if (< fieldsIndex fieldsCount) ; Don't output final comma after last fieldname
	(setq strColumnNames (strcat strColumnNames thisFieldName ", "))
      (setq strColumnNames (strcat strColumnNames thisFieldName))
      )
    (setq fieldsIndex (1+ fieldsIndex))
    )

  ;; Print these field-names
    (if bPrint
	(princ (strcat "\n" strColumnNames "\n")))
  

  strColumnNames	; Return this value
)


;;;--------------------------------------------------------------------;
;;; Utility function which accepts an ADODB.RecordSet object and a boolean.
;;; Each record in the recordSet is converted to a list of strings.    ;
;;; This routine returns a list of these list-of-string records.       ;
;;;                                                                    ;
;;; Set 2nd arg, bPrint == T to print records to screen, otherwise nil ;
;;; to just return the recordset fields as a list-of-list-of-string.   ;
;;;--------------------------------------------------------------------;
(defun printFieldValues( rs bPrint / fields fieldsIndex thisField rowIndex lstAllRows
			    fieldsCount strFieldValue strOneRow thisValue )

  (while (not (equal :vlax-true (vlax-get-property rs "EOF")))
    
    (setq fields (vlax-get-property rs "Fields")
	  fieldsCount (1- (vlax-get-property fields "Count")) ; zero-based index
	  fieldsIndex 0
	  strOneRow nil)

    ;; Print out record count:
    ;;  (princ (setq (strcat "\nThere are " (vl-princ-to-string fieldsCount)  " fields:\n\n")))


    (while (<= fieldsIndex fieldsCount)
      (setq thisField (vlax-get-property fields "item" fieldsIndex)) ; Item is a property not a method for recordSet fields...
      (setq thisValue (vlax-variant-value (vlax-get-property thisField "Value")))
      
      (setq strFieldValue (vl-princ-to-string thisValue))
      
      (setq strOneRow (append strOneRow (list strFieldValue)))
      (if (< fieldsIndex fieldsCount ) ; Don't output final comma after last fieldname
	  (setq strOneRow (append strOneRow (list ", ")))
	)
      (setq fieldsIndex (1+ fieldsIndex))
     );_while fieldsIndex
    
    
    (setq lstAllRows (cons (apply 'strcat strOneRow) lstAllRows))

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;; Display these field-names
    (if bPrint
	(progn
	  (princ (apply 'strcat strOneRow))
	  (princ "\n")
	)
    )
    
    (vlax-invoke-method rs "MoveNext") ; Move to next row in recordSet.
    
    );_while not yet at RecordSet EOF

  (setq lstAllRows (reverse lstAllRows)) ; Return this big list of all rows.
)




;;;--------------------------------------------------------------------;
;;; Utility function which accepts CAO.Link.KeyValue object and returns;
;;; a string representing the SQL "WHERE" clause needed for an SQL     ;
;;; SELECT statement to select a record matching these key values.     ;
;;;--------------------------------------------------------------------;
(defun buildWhereClause( thisKeyValue / strKeyName thisValue thisValueType 
				      strKeyValue )
  (setq strKeyName (vlax-get-property thisKeyValue "FieldName"))
  (setq thisValue (vlax-variant-value (vlax-get-property thisKeyValue "Value")))
  (setq thisValueType (vlax-variant-type (vlax-get-property thisKeyValue "Value")))
  
  (cond
   ((equal thisValueType vlax-vbString)
    (setq strKeyValue (strcat "'" thisValue  "'" ))
    )
   ((or
     (equal thisValueType vlax-vbInteger)
     (equal thisValueType vlax-vbLong)
     (equal thisValueType vlax-vbSingle)
     (equal thisValueType vlax-vbDouble)
     )
    (setq strKeyValue (vl-princ-to-string thisValue))
   )
   ((equal thisValueType vlax-vbBoolean)
    (if (equal :vlax-true thisValue)
	(setq strKeyValue "True")
      (setq strKeyValue "False"))
   )
   (t
    (setq strKeyValue  "")
   )
  );_cond
  
  (if (and 
       (not (equal strKeyName ""))
       (not (equal strKeyValue ""))
       )
      (if (equal strWhere "")		;strWhere is defined in calling function
	  (setq strWhere 
		(strcat "Where (" strKeyName " = " strKeyValue ")" ))
	(setq strWhere 
	      (strcat strWhere " or (" strKeyName " = " strKeyValue ")"))
	)
  )

  strWhere				; Return this value
);_BuildWhereClause()




;;;--------------------------------------------------------------------;
;;; Closes the open ADODB.RecordRet, rs and ADODB.Connection, ADOConnect
;;;--------------------------------------------------------------------;
(defun disconnectADOConnect()
  (if rs
      (if (/= adok-adStateClosed (vlax-get-property rs "State"))
	  (vlax-invoke-method rs "CLOSE")
      )
  )
  (setq rs nil)

  (if adoConnect
      (if (= adok-adStateOpen (vlax-get-property ADOConnect "State"))
	  (vlax-invoke-method ADOConnect "Close")))
  (setq adoconnect nil)
)





;;;--------------------------------------------------------------------;
;;; Create the CAO.DbConnect object and display it's Version property  ;
;;;--------------------------------------------------------------------;
(defun cmdInitDbConnect_Click()
  (setq dbConnect (vla-getinterfaceobject (vlax-get-acad-object) "CAO.DbConnect.16"))

  (if (null dbConnect)			; Then something serious is wrong!
      (progn
	(alert "Unable to create CAO.DbConnect Automation server! Exiting...")
	(exit)
      )
    ;;Else,
    (alert (strcat "CAO.dbConnect Automation server object is created!"))
  )
)



;;;--------------------------------------------------------------------;
;;; Connect to the datasource and table for this linkTemplate.         ;
;;; Display the ADODB Connection object's Version and Provider info.   ;
;;;--------------------------------------------------------------------;
(defun cmdConnectDataSource_Click( linkTemplate	; Pass in linkTemplate argument, or nil
				   /  strDataSourceLoc strDataSource
				     currConnectionString )

  (if (null linkTemplate) (setq linkTemplate (getLinkTemplate)) )


  (setq strDataSourceLoc (strcat (vlax-get-property dbConnect "DataSourceLocation") "\\")
	strDataSource (vlax-get-property linkTemplate "DataSource"))


  (if adoConnect
      (if (= adok-adStateOpen (vlax-get-property ADOConnect "State"))
	  (vlax-invoke-method ADOConnect "Close")))
  (setq adoconnect nil)
  (setq ADOConnect (vlax-create-object "ADODB.Connection") ) ; ADOConnect is a global

  (setq currConnectionString (strcat "File Name=" strDataSourceLoc strDataSource 
				     ".udl;User ID=;Password=;" ))

  (vlax-put-property ADOConnect "ConnectionString" currConnectionString)
  (vlax-invoke-method ADOConnect "Open" currConnectionString "" "" -1) ; LISP seems to require args which should be optional... 


  (if (= adok-adStateOpen (vlax-get-property ADOConnect "State"))
      (alert (strcat strDataSource " is connected."
		     "\nVersion = " (vlax-get-property ADOCOnnect "Version")
		     " Provider = " (vlax-get-property ADOCOnnect "Provider")))
    (alert (strcat "Unable to connect to " strDataSource))
  )

  (setq status_string (strcat "ADODB.Connection.OPEN( " 
			      "File Name=" strDataSource 
			      ".udl;User ID=;Password=;" ))
)




;;;--------------------------------------------------------------------;
;;; Utility function to invoke the Error object's Clear() method       ;
;;;--------------------------------------------------------------------;
(defun resetCAOErrors( / objErrors )
  ;; Define a global variable, dbConnect
  (setq dbConnect (vla-getinterfaceobject (vlax-get-acad-object) "CAO.DbConnect.16"))
  (setq objErrors (vlax-invoke-method dbConnect "GetErrors"))
  (vlax-invoke-method objErrors "Clear") ; Reset CAO error stack
)


;;;--------------------------------------------------------------------;
;;; Invoke the CAO.DbConnect.GetErrors() method and display each error ;
;;; object's errorcode and description.                                ;
;;;--------------------------------------------------------------------;
(defun cmdErrors_Click( / objErrors index strError)
  (setq objErrors (vlax-invoke-method dbConnect "GetErrors"))

  (setq lstOutput nil)			; can be used by caotest_dialog

  (if (= 0 (vlax-get-property objErrors "Count"))
      (alert "No CAO errors!")

    (progn
      (setq  index     0
	     strError  ""
	     lstOutput nil)		; can be used by caotest_dialog
      (vlax-for OneError objErrors

		(setq lstOutput (append lstOutput 
					(list 
					 (strcat " Error " (itoa index) ":   "
						 (itoa (vlax-get-property OneError "ErrorCode"))
						 ", "  (vlax-get-property OneError "ErrorDescription"))
					)))
		(setq index (1+ index))
       )
    )
  )

  (foreach str lstOutput
	   (princ (strcat "\n" str)))
  (princ "\n")


  (setq status_string "CAO.DbConnect.GetErrors()") 
  objErrors
)






;;;--------------------------------------------------------------------;
;;; This is not CAO-specific, but is useful for selecting drawing      ;
;;;--------------------------------------------------------------------;
(defun cmdGetDrawings_Click( / document documents listDrawings)
  (setq Documents (vlax-get-property acadObj "Documents"))

  (vlax-for Doc Documents
	    (setq listDrawings (cons (vlax-get-property Doc "Name") listDrawings))
  )
  (setq Document (vlax-invoke-method Documents "Item" 
					 (selectItem listDrawings "caotest.dcl" "selectDocdlg")))
)



;;;--------------------------------------------------------------------;
;;; Utility function to show how to use error-handling when making an  ; 
;;; ActiveX function call.  This takes a Drawing Document object and   ; 
;;; if successful, returns the linkTemplates collection from this doc. ;
;;; If an error occurs, it simply presents an Alert dialog with the    ;
;;; error message contents and then exits.                             ;
;;;--------------------------------------------------------------------;
(defun mygetLinkTemplates( doc / linkTemplates )
  (setq linkTemplates (vl-catch-all-apply 'caom-GetLinkTemplates (list dbConnect doc)))

  (cond
   ((vl-catch-all-error-p linkTemplates) ; If the GetLinkTemplates() method raised an ActiveX error
    (alert (strcat "Caught an ActiveX error: " (vl-catch-all-error-message linkTemplates) ))
    (exit)
   )
   ((>= 0 (vlax-get-property linkTemplates "Count"))
    (progn
      (alert "You must first create one or more link templates.")
      (exit)
    )
   )
  );_end cond

  linkTemplates
)

;;;--------------------------------------------------------------------;
;;; Utility function to prompt user to select a link template from the ; 
;;; set of link templates in the active Drawing, acadDoc.              ;
;;;--------------------------------------------------------------------;
(defun getLinkTemplate( / linkTemplates lstLT linkTemplate)

  ;; Next commented-out line shows how to use vlax-invoke-method to call the
  ;; ActiveX method CAO.dbConnect.getLinkTemplates().  But, when an ActiveX
  ;; function fails, your normal lisp *error* function will not catch it.
  ;; 
  ;;(setq linkTemplates (vlax-invoke-method dbConnect "GetLinkTemplates" acadDoc))
  ;;
  ;; Instead of using the vlax-invoke-method above, let us write our own 
  ;; function to get link templates.  The "mygetLinkTemplates()" function
  ;; will use the vl-catch-all-apply() method to provide some error-handling
  ;; capabilities which are not available when using vlax-invoke-method.
  ;;
  (setq linkTemplates (mygetLinkTemplates acadDoc))

  (setq lstLT nil)
  (vlax-for thisLT linkTemplates
	    (setq lstLT (append (list (vlax-get-property thisLT "Name")) lstLT))
  )
  (setq linkTemplate (vlax-invoke-method 
  		      linkTemplates "Item" 
  		      (selectItem lstLT "caotest.dcl" "selectLTdlg")))

  linkTemplate				; Return this value.
)




;;;--------------------------------------------------------------------;
;;; Utility function to return the sum of the CAO constants which will ; 
;;; be used for setting the linktype in the CAO.DbConnect.GetLinks().  ;
;;;--------------------------------------------------------------------;
(defun getLinkTypes( / linkTypes strLinkTypes )
  (setq linkTypes    0
	strLinkTypes "")

  (if bENTITYLINK 
      (progn 
	(setq linkTypes (+ linkTypes caok-kEntityLinkType))
	(setq strLinkTypes (strcat strLinkTypes " kEntityLinkType "))
      )
  )
  (if bFSLABEL 
      (progn
	(setq linkTypes (+ linkTypes caok-kFSLabelType))
	(setq strLinkTypes (strcat strLinkTypes " kFSLabelType "))
      )
  )
  (if bATTACHEDLABEL
      (progn
	(setq linkTypes (+ linkTypes caok-kAttachedLabelType))
	(setq strLinkTypes (strcat strLinkTypes " kAttachedLabelType "))
      )
  )

  (if (equal "" strLinkTypes)
      (setq strLinkTypes "  No link types specified... so no links will be found!")  )

  (cons linkTypes strLinkTypes)		;Return this value
)

;;;--------------------------------------------------------------------;
;;; Call the CAO.DbConnect.GetLinks() object using the selected link   ; 
;;; link template, selected objects and specified link types.          ;
;;;--------------------------------------------------------------------;
(defun cmdGetLinks_Click( linkTemplate / entitySet entityCount entityIndex objIDs linkSel linkTypes strlinkTypes)

  (if (null linkTemplate) (setq linkTemplate (getLinkTemplate)) )

  ;; Create a SafeArray of VLA ObjectIDs
  (setq objIDs (getObjIDs))


  ;; For linktypes, Specify the sum of constants:
  ;;     kEntityLinkType kFSLabelType kAttachedLabelType
  (setq linkTypes     (car (getLinkTypes))
	strLinkTypes (cdr (getLinkTypes)))


  ;; Get All linked entities using this Link Template
  (setq linkSel (vlax-invoke-method DbConnect "Getlinks" linkTemplate objIds linkTypes))  ; objIds, LinkTypes, Document are all optional


  (setq status_string (strcat 
		       "CAO.DbConnect.GetLinks( " 
		       (vlax-get-property linkTemplate "Name")
		       " objIDs " 
		       strLinkTypes " )"))


  (princ (strcat "\n\n Found " (itoa (vlax-get-property linkSel "Count")) 
		 " link(s) for this link template, " (vlax-get-property linkTemplate "Name")
		 " on the selected object(s).\n"))

  (setq lstOutput (list
		   (strcat "Found " (itoa (vlax-get-property linkSel "Count")) " link(s) ")
		   (strcat "for this link template, " (vlax-get-property linkTemplate "Name"))
		   (strcat "on the selected object(s) ")
		   (strcat "for the specified object types, ") strLinkTypes))

  linkSel				; Return this Links Collection value.

);_defun cmdGetLinks_Click








;;;--------------------------------------------------------------------;
;;; Print out LinkTemplate data for the selected link template.        ; 
;;;--------------------------------------------------------------------;
(defun cmdGetLinkTempData_Click( / linkTemplate txtLinkTempData LTKeyDescriptions LTKDsCount
				   strOneKey index strOneKey lstallKeys)
  (setq linkTemplate (getLinkTemplate))


  (if linkTemplate
     (progn

       (setq txtLinkTempData (list 
			      (strcat "Name:       " (vlax-get-property linkTemplate "Name"))
			      (strcat "DataSource: " (vlax-get-property linkTemplate "DataSource"))
			      (strcat "Catalog:    " (vlax-get-property linkTemplate "Catalog"))
			      (strcat "Schema:     " (vlax-get-property linkTemplate "Schema"))
			      (strcat "Table:      " (vlax-get-property linkTemplate "Table") )
			    ))
    

       (setq LTKeyDescriptions (vlax-get-property linkTemplate "KeyDescriptions")
	     LTKDsCount (vlax-get-property LTKeyDescriptions "Count")
	     strOneKey  ""
	     index      0
	     lstOutput  (append nil txtLinkTempData)) ;Initialize lstOutput global
  
       (vlax-for OneKey LTKeyDescriptions
		 (setq index (1+ index))
		 
		 (setq strOneKey (list
				  (strcat "Key " (itoa index) " of " (itoa LTKDsCount)  ": ")
				  (strcat ":      " "FieldName: " (vlax-get-property OneKey "FieldName"))
				  (strcat ":      " "Type: " (vl-princ-to-string (vlax-get-property OneKey "Type")))
				  (strcat ":      " "DefinedSize: " (vl-princ-to-string (vlax-get-property OneKey "DefinedSize")))
				  (strcat ":      " "NumericScale: " (vl-princ-to-string (vlax-get-property OneKey "NumericScale")))
				  (strcat ":      " "Precision: " (vl-princ-to-string (vlax-get-property OneKey "Precision")))
			    ))
		 (setq lstallKeys (append lstallkeys strOnekey))
       )

       (setq lstOutput (append lstOutput lstallKeys)) 	; can be used by caotest_dialog


       ;;(foreach str lstOutput
       ;;	(princ (strcat "\n" str)))
       (princ)

  ))
);_defun cmdGetLinkTempData_Click






;;;--------------------------------------------------------------------;
;;; Call CAO.DbConnect.GetLinkTemplates() specifying the [document]    ; 
;;; argument to retrieve all link templates in this document.          ;
;;;--------------------------------------------------------------------;
(defun cmdGetLinkTemps_Click( / listLinkTemplates Doc linkTemplate linkTemplates listLInkTemplates)

  (setq Doc (cmdGetDrawings_Click))	; Request user to select a Document.

  (setq linkTemplates (vlax-invoke-method dbConnect "GetLinkTemplates" Doc))

  (if (null linkTemplates)
      (alert "Unable to get link templates"))

  (progn
    (if (< 0 (vlax-get-property linkTemplates "Count"))
	(vlax-for linkTemplate linkTemplates
		  (setq listLinkTemplates (cons (vlax-get-property linkTemplate "Name") listLinkTemplates))
	)

    )
  )

  (setq status_string "CAO.DBConnect.GetLinkTemplates( document )")

  (setq lstOutput listLinkTemplates) 	; can be used by caotest_dialog
)







;;;--------------------------------------------------------------------;
;;; Setup a KeyValues collection (based on a linkTemplate.keyDescription) 
;;; and setup an array of VLA ObjectIDs.                               ;
;;; Together, these keyValues and ObjectIDs will be used in the call to;
;;; CAO.linkTemplate.CreateLink()                                      ;
;;;--------------------------------------------------------------------;
(defun cmdMakeLink_Click( / count cmd lstAllRows selectedRow selectedRowIndex
			    currKeyValues OneField LTKeyDescriptions OneKeyValue
			    fields fieldsCount fieldsIndex thisField linkTemplate
			    objIDs objIndex objCount objectID newLink  
			    linkResult)

  ;; Get linkTemplate
  (setq linkTemplate (getLinkTemplate))

  (cmdConnectDataSource_Click linkTemplate) ; Defines global variable, ADOConnect

  (setq Count  0
	cmd (vlax-create-object "ADODB.Command"))
  (vlax-put-property cmd "ActiveConnection" ADOConnect)
  (vlax-put-property cmd "CommandTimeout"  30)
  (vlax-put-property cmd "CommandText" (strcat "SELECT * from " (vlax-get-property linkTemplate "Table") ))


  (if (/= adok-adStateClosed (vlax-get-property rs "State"))
      (vlax-invoke-method rs "CLOSE"))
  (setq rs nil)		; In VBA, setting recordSet to 'Nothing' disconnects. Not needed in LISP?
  (setq rs (vlax-create-object "ADODB.Recordset"))
  (vlax-invoke-method rs "OPEN" cmd nil adok-adOpenDynamic adok-adLockBatchOptimistic adok-adCmdUnknown) ;adok-OpenForwardOnly adok-adLockReadOnly adok-adCmdUnknown) ; 



  ;; Prompt user to select a row in this table.

  ;; Kinda round-about...: we convert recordSet to list-of-list-of-string, then
  ;;  user selects an item, and we find index of this selected item in list
  ;;  and finally we use this index to find the recordSet.fields[fieldIndex]
  (setq lstAllRows (printFieldValues rs nil)) ; side-effect is to return big list of rows.
  (setq selectedRow (selectItem lstAllRows "caotest.dcl" "selectRowdlg"))
  (setq selectedRowIndex (vl-position selectedRow lstAllRows)) ; Both LISP list and recordSet use Zero-based index

  (vlax-invoke-method rs "Movefirst" )
  (vlax-invoke-method rs "Move" selectedRowIndex ) ; adok-adBookmarkFirst)

   
  
  ;; Setup a KeyValues collection to be used when we call linkTemplate.CreateLink()
  ;; These keyvalues will be based on the linkTemplate.KeyDescriptions collection
  ;; 
  (setq currKeyValues (vla-getinterfaceobject (vlax-get-acad-object) "CAO.KeyValues.16")
	OneField (vlax-create-object "ADODB.Field")
	LTKeyDescriptions (vlax-get-property linkTemplate "KeyDescriptions"))

  ;; Get each key description
  (vlax-for OneKeyDesc LTKeyDescriptions
	    (setq fields (vlax-get-property rs "Fields")
		  fieldsCount (1- (vlax-get-property fields "Count")) ; zero-based index
		  fieldsIndex 0)
	    (while (<= fieldsIndex fieldsCount)
	      (setq thisField (vlax-get-property fields "item" fieldsIndex)) ; Item is a property not a method for recordSet fields...

	      (if (equal (strcase (vlax-get-property thisField "Name"))
			 (strcase (vlax-get-property OneKeyDesc "Fieldname")))
		  (progn
		    
		    (vlax-invoke-method currKeyValues "Add"
		      		(vlax-get-property OneKeyDesc "Fieldname")
		      		(vlax-get-property thisField "Value"))
		    (setq fieldsIndex (1+ fieldsCount)) ; So we can break out of the loop
		  )
	      )

	      (setq fieldsIndex (1+ fieldsIndex))
	    )
  );_foreach keyDesc in LinkTemplate.keydescriptions



  (setq objIDs (getobjIDs))		; Select entities in AutoCAD

  ;; Create link to each entity which was selected
  (setq objCount (vlax-safearray-get-u-bound objIDs 1)
	objIndex 0)

  (setq lstOutput '(""))			; can be used by caotest_dialog

  (while (< objIndex ObjCount)
    (setq objectID (vlax-safearray-get-element objIDs objIndex))


    ;; Instead of directly invoking the CAO.LinkTemplate.CreateLink() method,
    ;;(setq newLink (vlax-invoke-method linkTemplate "CreateLink" objectID currKeyValues))

    ;; Instead, use vl-catch-all-apply() to apply this method and trap for ActiveX
    ;; errors.  To see this fail with an error, try clicking the CAO TEST dialog
    ;; button "Make Link" and try to link to an object on a locked layer.
    ;;
    (setq linkResult (vl-catch-all-apply 'caom-CreateLink 
					 (list linkTemplate objectID currKeyValues) ))
			
    (if (vl-catch-all-error-p linkResult) ; If the caom-CreateLink() method caused an ActiveX error...
	 (setq lstOutput 
	       (append (list 
			"Trying to execute CreateLink() caused an ActiveX error: " 
			(vl-catch-all-error-message linkResult) )
		       lstOutput))
    )


    (setq objIndex (1+ objIndex))
  )



  (setq status_string (strcat 
		       "\"" (vlax-get-property linkTemplate "Name") "\""
		       ".CreateLink( objID currKeyValues )" ))

);_defun cmdMakeLink_Click()










;;;--------------------------------------------------------------------;
;;; Call the CAO.DbConnect.ReloadLabels() method                       ;
;;;--------------------------------------------------------------------;
(defun cmdReloadLabels_Click( / Doc )

  ;; Define a global variable, dbConnect
  (setq dbConnect (vla-getinterfaceobject (vlax-get-acad-object) "CAO.DbConnect.16"))

  ;;(setq Doc (cmdGetDrawings_Click))	; Request user to select a Document.

  ;; Connections occur automatically for reloadLabels, so no need
  ;; to force a connection for reloadLabels to work.
  ;;(cmdConnectDataSource_Click nil)	


  ;; Check if there are any linkTemplates in the drawing...
  ;; While a linkTemplate is *not* required as an argument to ReloadLabels(),
  ;; the user must have at least one linkTemplate in the drawing before
  ;; any links or labels can exist... so exit if there are 0 linkTemplates.
  ;;
  (setq linkTemplates (vlax-invoke-method dbConnect "GetLinkTemplates" acadDoc))
  (if (or
       (null linkTemplates)
       (>= 0 (vlax-get-property linkTemplates "Count"))
      )
      (progn
	(alert "You must first create one or more link templates.")
	(exit)
      )
  )

  (vlax-invoke-method dbConnect "ReloadLabels") ; (getObjIDs) doc)

  (setq status_string "CAO.DbConnect.ReloadLabels()" )
  
)






;;;--------------------------------------------------------------------;
;;; Call the CAO.DbConnect.GetLinks() method to retrieve all linked    ;
;;; objects for this link template.                                    ;
;;; Find the first link in the linkSel collection which is linked to   ;
;;; the user-select object.                                            ;
;;; Get and display the Link object's linkType and keyValues           ;
;;;--------------------------------------------------------------------;
(defun cmdShowLinkData_Click( linkTemplate linkSel objId 
			      / link txtLinkData selectedObj selectedObjID linkKeyValues
			      linkindex KeyType KeyValue strKeyValue lstLinkData linkTypes strLinkTypes ss)

  (if (null linkTemplate) (setq linkTemplate (getLinkTemplate)) )

  (setq strLinkTypes "")
  (if (null linkSel)
      (progn
	;; For linktypes, Specify the sum of constants:
	;;     kEntityLinkType kFSLabelType kAttachedLabelType
	(setq linkTypes     (car (getLinkTypes))
	      strLinkTypes (cdr (getLinkTypes)))
	(setq linkSel (vlax-invoke-method DbConnect "Getlinks" linkTemplate nil linkTypes))
      )
  )
  (if (null linkSel)
      (alert "No links in drawing!"))

  (if objID
      (setq selectedObjID objID)
  )
  (if (null selectedObjID)
      (setq selectedObjID (getOneObjID))
  )



  ;; Find the first link in the linkSel collection which is linked to this object
  (vlax-for thisLink linkSel
  	    (if (equal selectedobjId (vlax-get-property thisLink "ObjectID"))
  		(setq link thisLink))
  )


  (if (null link)
      (progn 
	(alert  "Unable to retrieve this link")
	(setq lstOutput nil)			; can be used by caotest_dialog
      )
    ;; Else
    (progn
      (setq lstOutput  (append nil (list                       ;Initialize lstOutput global      
				    (strcat "First Link on selected object: ")
				    (strcat "ObjectID: " (vl-princ-to-string (vlax-get-property link "ObjectID")))
				    (strcat "Link Template Name: ")
				    (strcat (vlax-get-property (vlax-get-property link "LinkTemplate") "Name"))
				   )))


      
      (cond
        ((equal caok-kEntityLinkType (vlax-get-property link "LinkType"))
	 (setq lstOutput  (append lstOutput (list "Link Type: Entity Link")))
	)
	((equal caok-kFSLabelType (vlax-get-property link "LinkType"))
	 (setq lstOutput  (append lstOutput (list "Link Type: Free Standing Label")))
	)
	((equal caok-kAttachedLabelType (vlax-get-property link "LinkType"))
	 (setq lstOutput  (append lstOutput (list "Link Type: AttachedLabel")))
	)
        (t
	 (setq lstOutput  (append lstOutput (list "Link Type: Unknown type")))
	)
      );_cond
  

      (setq lstOutput  (append lstOutput (list "KeyValues:"))) ;global lstOutput


      (setq linkKeyValues (vlax-get-property link "KeyValues"))
      (if (null linkKeyValues)
	  (setq lstOutput  (append lstOutput (list "No key value is defined")))

	(vlax-for OneKeyValue linkKeyValues

		  (setq KeyValue (vlax-variant-value (vlax-get-property OneKeyValue "Value"))
			KeyType (type KeyValue))

		  (cond
		   ((equal vlax-vbString KeyType)
		    (setq strKeyValue (strcat "'" KeyValue  "'" ))
		    )
		   ((or
		     (equal vlax-vbInteger KeyType)
		     (equal vlax-vbLong KeyType)
		     (equal vlax-vbSingle KeyType)
		     (equal vlax-vbDouble KeyType)
		     (equal 'INT KeyType)
		     (equal 'REAL KeyType)
		     )
		    (setq strKeyValue (vl-princ-to-string KeyValue))
		    )
		   ((equal vlax-vbBoolean KeyType)
		    (if (equal :vlax-true KeyValue)
			(setq strKeyValue "True")
		      (setq strKeyValue "False"))
		    )
		   ((equal 'STR KeyType)
		    (setq strKeyValue (strcat "'" KeyValue  "'" ))
		    )
		   (t
		    (setq strKeyValue  "Unknown type")
		    )
		  );_cond



		  (setq lstLinkData (append lstLinkData
					    (list (strcat (vlax-get-property OneKeyValue "FieldName") ": " strKeyValue))))

        );_vlax-for OneKeyValue linkKeyValues
      );_if not null keyValues


      (setq lstOutput (append lstOutput lstLinkData)) 	; can be used by caotest_dialog

  ));_if link


  (setq status_string (strcat 
		       "CAO.DbConnect.GetLinks( " 
		       (vlax-get-property linkTemplate "Name")
		       " nil " 
		       strLinkTypes " )"))


);_cmdShowLinkData_Click






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;--------------------------------------------------------------------;
;;; Call the CAO.DbConnect.GetLinks() method to retrieve linked objects;
;;; for this link template.
;;; Connect to the datasource and table for this linkTemplate.         ;
;;; Construct an SQL "SELECT" statement to retrieve all table records. ;
;;; Display the column(field) names and values for all records.        ;
;;;--------------------------------------------------------------------;
(defun cmdShowLinkedRow_Click( linkTemplate linkSel 
			       /  linkCount strwhere thisLInk cmd strColumnNames lstAllRows linkTypes strLinkTypes)

  (if (null linkTemplate) (setq linkTemplate (getLinkTemplate)) )


  ;; For linktypes, Specify the sum of constants:
  ;;     kEntityLinkType kFSLabelType kAttachedLabelType
  (setq linkTypes     (car (getLinkTypes))
	strLinkTypes (cdr (getLinkTypes)))

  ;; Get All user-selected entities which have links using this Link Template
  (if (null linkSel)
      (setq linkSel (vlax-invoke-method DbConnect "Getlinks" linkTemplate (getObjIDs) linkTypes)))

  (if (null linkSel)
      (alert "No links found for selected objects using this link template.")
    ;; Else,
    (princ (strcat "\n\n Found " (itoa (vlax-get-property linkSel "Count")) 
		   " link(s) for the selected object(s) using link template, " 
		   (vlax-get-property linkTemplate "Name") "\n"))
  )



  (setq strWhere "")
  (vlax-for thisLink linkSel
    ;; Read key values, build up SQL WHERE clause
    (vlax-for thisKeyValue (vlax-get-property thisLink "KeyValues")
	      (setq strWhere (buildWhereClause thisKeyValue))
    )
  )


  (if (not (equal strWhere ""))
      (progn
	  
	;; Query database and get linked rows through ADO
	(cmdConnectDataSource_Click linkTemplate) ; Defines global variable, ADOConnect

	(setq cmd (vlax-create-object "ADODB.Command"))
	(vlax-put-property cmd "ActiveConnection" ADOConnect)
	(vlax-put-property cmd "CommandTimeout"  30)
	(vlax-put-property cmd "CommandText" 
			   (strcat "SELECT * from " 
				   (vlax-get-property linkTemplate "Table")
				   " "  strWhere))
	  
	(if (/= adok-adStateClosed (vlax-get-property rs "State"))
	    (vlax-invoke-method rs "CLOSE"))
	(setq rs nil)		; In VBA, setting recordSet to 'Nothing' disconnects. Not needed in LISP?
	(setq rs (vlax-create-object "ADODB.Recordset"))
	(vlax-invoke-method rs "OPEN" cmd nil adok-adOpenDynamic adok-adLockBatchOptimistic adok-adCmdUnknown) ;adok-OpenForwardOnly adok-adLockReadOnly adok-adCmdUnknown) ; 
	  
	  
	;; get/Print out Field names.
	(setq strColumnNames (printFieldNames rs nil))

	;; get/Print out Field values
	(setq lstAllRows (printFieldValues rs nil))
	  
	  
       );progn strWhere /= ""
   );_if strWhere /= ""
    


  (setq status_string (strcat 
		       "CAO.DbConnect.GetLinks( " 
		       (vlax-get-property linkTemplate "Name")
		       " objIDs " 
		       strLinkTypes " )"))


  (if (or
       (null strColumnNames)
       (null lstAllRows)
      )
      (setq lstOutput '(""))			; can be used by caotest_dialog
  (setq lstOutput (append (list strColumnNames) lstAllRows)) ; Can be used by caotest_dialog
  )
  rs					; Return this recordSet

);_defun cmdShowLinkedRow_Click







;;;--------------------------------------------------------------------;
;;; Connect to the datasource and table for this linkTemplate.         ;
;;; Construct an SQL "SELECT" statement to retrieve all table records. ;
;;; Display the column(field) names and values for all records.        ;
;;;--------------------------------------------------------------------;
(defun cmdShowTable_Click( linkTemplate /  count cmd KeyDescs KeyValues KeyValuesCount
					KeyValuesIndex lstAllRows newLinkRowIndex 
					OneKeyValue selectedRow thisField
					fields fieldsCount  strColumnNames lstAllRows)

  (if (null linkTemplate) (setq linkTemplate (getLinkTemplate)) )

  ;; Connect to the ADOConnect object
  (cmdConnectDataSource_Click linkTemplate) ; Defines global variable, ADOConnect
  
  (setq Count  0
	cmd (vlax-create-object "ADODB.Command"))

  (vlax-put-property cmd "ActiveConnection" ADOConnect)
  (vlax-put-property cmd "CommandTimeout"  30)
  (vlax-put-property cmd "CommandText" (strcat"SELECT * from " 
					      (vlax-get-property linkTemplate "Table")))

  (setq rs nil)		; In VBA, setting recordSet to 'Nothing' disconnects. Not needed in LISP?
  (setq rs (vlax-create-object "ADODB.Recordset"))
  (vlax-invoke-method rs "OPEN" cmd nil adok-adOpenDynamic adok-adLockBatchOptimistic adok-adCmdUnknown) ;adok-OpenForwardOnly adok-adLockReadOnly adok-adCmdUnknown) ; 

  (princ (strcat "\n " (itoa (vlax-get-property rs "recordCount")) " Rows"))


  ;; Print out Field names.
  (setq strColumnNames (printFieldNames rs nil))
  
  ;; Print out Field values
  (setq lstAllRows (printFieldValues rs nil))


  (setq status_string "ADODB.RecordSet.OPEN( cmd nil adOpenDynamic adLockBatchOptimistic )" )



  (setq lstOutput (append (list strColumnNames) lstAllRows)) ; Can be used by caotest_dialog


  nil					; So we don't output the side-effect of the previous line.
)









;;;--------------------------------------------------------------------;
;;; Change the Key Value of an existing Link on an object.             ;
;;; First, gather the LinkTemplate KeyDescriptions.  Then, get links   ;
;;; on user-selected entity which are based on this Link Template.     ; 
;;; Prompt user to select a new record.  Use this record's key field(s);
;;; value(s) as the new link Keyvalue.  End by calling the Update()    ;
;;; method for this link object.  And, call the dbConnect.ReloadLabels ;
;;; method.                                                            ;
;;;                                                                    ;
;;; Currently allows editing only the first link on a selected object. ;
;;;--------------------------------------------------------------------;
(defun cmdUpdateLink_Click( / linkTemplate linkSel link ename vlaobj objID objIDs
			      fields fieldsCount fieldsIndex thisField 
			      keyValues keyValuesCount keyValuesIndex 
			      oneKeyField oneKeyValue
			      keyDescs newLinkRowIndex selectedRow cmd
			      linkTypes strLinkTypes)
			      

  ;; Get link template and keydescriptions
  (setq linkTemplate (getLinkTemplate))
  (setq KeyDescs (vlax-get-property linkTemplate "KeyDescriptions"))

  (setq objID (getOneObjID))
  ;; Establish an empty SafeArray to hold ObjectIDs
  (setq ObjIDs (vlax-make-safearray vlax-vbLong (cons 0 1)))
  (vlax-safearray-put-element ObjIDs 0 objID)


  ;; For linktypes, Specify the sum of constants:
  ;;     kEntityLinkType kFSLabelType kAttachedLabelType
  (setq linkTypes     (car (getLinkTypes))
	strLinkTypes (cdr (getLinkTypes)))

  ;; Get links on user-selected entity which are based on this Link Template
  (setq linkSel (vlax-invoke-method DbConnect "Getlinks" linkTemplate objIds linkTypes))

  (if (or
       (null linkSel)
       (equal 0 (vlax-get-property linkSel "Count"))
      )
      (alert "No links found on selected objects which use this LinkTemplate!")

    ;; Else, we have one or more links...
    (progn

      ;; Defaulting to update just the first link found on this selected object
      (setq link (vlax-invoke-method linkSel "Item" 0))




      ;;This routine is currently Defaulting to update the first link in the linkSel, so skip this next stuff for now.
      ;;
      ;;   (setq rs (cmdShowLinkedRow_Click linkTemplate linkSel))
      ;;   (vlax-invoke-method rs "Movefirst" )
      ;;  
      ;;   ;;Kinda round-about...: we convert recordSet to list-of-list-of-string, then
      ;;   ;;  user selects an item, and we find index of this selected string in list
      ;;   ;;  and finally we use this index to find the recordSet.fields[fieldIndex]
      ;;   (setq lstLinkedRows (printFieldValues rs nil)) ; side-effect is to return big list of rows.
      ;;   (setq selectedRow (selectItem lstLinkedRows "caotest.dcl" "selectRowdlg"))
      ;;   (setq linkToBeEditedRowIndex (vl-position selectedRow lstLinkedRows)) ; Both LISP list and recordSet use Zero-based index
      ;;   (vlax-invoke-method rs"Movefirst" )
      ;;   (vlax-invoke-method rs "Move" linkToBeEditedRowIndex ) ; adok-adBookmarkFirst)
      ;;  
      
    (setq cmd (vlax-create-object "ADODB.Command"))
    (cmdConnectDataSource_Click linkTemplate) ; Defines global variable, ADOConnect
    (vlax-put-property cmd "ActiveConnection" ADOConnect)
    (vlax-put-property cmd "CommandText" 
		       (strcat "SELECT * from " 
			       (vlax-get-property linkTemplate "Table")))
    
    (if (/= adok-adStateClosed (vlax-get-property rs "State"))
	(vlax-invoke-method rs "CLOSE"))
    (setq rs nil)		; In VBA, setting recordSet to 'Nothing' disconnects. Not needed in LISP?

    (setq rs (vlax-create-object "ADODB.Recordset"))
    (vlax-invoke-method rs "OPEN" cmd nil adok-adOpenDynamic adok-adLockBatchOptimistic adok-adCmdUnknown) ;adok-OpenForwardOnly adok-adLockReadOnly adok-adCmdUnknown) ; 
    
    
    (setq lstAllRows (printFieldValues rs nil)) ; side-effect is to return big list of rows.
    (setq selectedRow (selectItem lstAllRows "caotest.dcl" "selectRowdlg"))
    (setq newLinkRowIndex (vl-position selectedRow lstAllRows)) ; Both LISP list and recordSet use Zero-based index
    
    
    ;;Defaulting to update the first link in the linkSel, so skip this for now.
    ;;
    ;; We have a linkSel, we need the one link 
    ;; Iterate through all links in linkSel, 
    ;; find link.KeyValues which match that of rs.fields.linkToBeEditedRowIndex
    
    

    (vlax-invoke-method rs "Movefirst" )
    (vlax-invoke-method rs "Move" newLinkRowIndex ) ; Position recordSet at user-specified field

    (setq fields      (vlax-get-property rs "Fields")
	  fieldsCount  (vlax-get-property fields "Count")
	  KeyValues (vlax-get-property link "KeyValues")
	  KeyValuesCount (1- (vlax-get-property KeyValues "Count")) ; zero-based index
	  newKeyValues(vla-getinterfaceobject (vlax-get-acad-object) "CAO.KeyValues.16") ; 
	  KeyValuesIndex 0
	  
    )
    (while (<= KeyValuesIndex KeyValuesCount)
    
      (setq OneKeyValue (vlax-invoke-method KeyValues "Item" KeyValuesIndex)
	    fieldsIndex 0)
      
      (while (<= fieldsIndex fieldsCount)
	(setq thisField (vlax-get-property fields "Item" fieldsIndex))
	
	(if (equal (strcase (vlax-get-property thisField "Name"))
		   (strcase (vlax-get-property OneKeyValue "FieldName")))
	    (progn
	      (vlax-put-property OneKeyValue "Value" (vlax-variant-value (vlax-get-property thisField "Value")))
	      
	      (setq fieldsIndex (1+ fieldsCount)) ; So we can break out of the loop
	    )
        )
        (vlax-invoke-method newKeyValues "Add" (vlax-get-property thisField "Name") (vlax-variant-value (vlax-get-property thisField "Value")))
	(setq fieldsIndex (1+ fieldsIndex))
       );_while more fields

      (setq KeyValuesIndex (1+ KeyValuesIndex))
    );_while more keyvalues

    ;;(vlax-invoke-method link "Update")
    (vlax-put-property link "KeyValues" newKeyValues)

    (setq status_string "CAO.Links.Item(0).Update()")

  ));_progn, if linkSel is not null
);_defun cmdUpdateLink()



;;; Display a message to let the user know the command name
(princ "\n Type CAOTEST to run the CAO Test dialog.") 
(princ)					;load quietly
