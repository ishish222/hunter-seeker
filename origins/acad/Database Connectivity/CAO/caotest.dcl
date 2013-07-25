//----------------------------------------------------------------------------
//
//   CAOTEST.DCL   Version 1.0
//
//     Copyright 1999
//     by Autodesk, Inc. All Rights Reserved. 
//
//  You are hereby granted permission to use, copy and modify this   
//  software without charge, provided you do so exclusively for      
//  your own use or for use by others in your organization in the    
//  performance of their normal duties, and provided further that    
//  the above copyright notice appears in all copies and both that   
//  copyright notice and the limited warranty and restricted rights  
//  notice below appear in all supporting documentation.             
//                                                                   
//  Incorporation of any part of this software into other software,  
//  except when such incorporation is exclusively for your own use   
//  or for use by others in your organization in the performance of  
//  their normal duties, is prohibited without the prior written     
//  consent of Autodesk, Inc.                                        
//                                                                   
//  Copying, modification and distribution of this software or any   
//  part thereof in any form except as expressly provided herein is  
//  prohibited without the prior written consent of Autodesk, Inc.   
//                                                                   
//  AUTODESK PROVIDES THIS SOFTWARE "AS IS" AND WITH ALL FAULTS.     
//  AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF          
//  MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK,      
//  INC. DOES NOT WARRANT THAT THE OPERATION OF THE SOFTWARE         
//  WILL BE UNINTERRUPTED OR ERROR FREE.                             
//                                                                   
//  Restricted Rights for US Government Users.  This software        
//  and Documentation are provided with RESTRICTED RIGHTS for US     
//  US Government users.  Use, duplication, or disclosure by the     
//  Government is subject to restrictions as set forth in FAR        
//  12.212 (Commercial Computer Software-Restricted Rights) and      
//  DFAR 227.7202 (Rights in Technical Data and Computer Software),  
//  as applicable.  Manufacturer is Autodesk, Inc., 111 McInnis      
//  Parkway, San Rafael, California 94903.                           
//                                                                   
//
//
//----------------------------------------------------------------------------
// 
// Corresponding dialogue for CAOTEST.LSP which exercises the CAO,
// the Connectivity Automation Interface API for dbConnect.
// 
//----------------------------------------------------------------------------

//dcl_settings : default_dcl_settings { audit_level = 3; }

selectLTdlg : dialog {
    label = "Select Link Template";
    : text {
      label = "Select one Link Template";
    }

    spacer_1;

    : list_box {
      key = "selections";
      width = 17;
      height = 6;
    }

    spacer_1;
    ok_cancel;
}

selectDocdlg : dialog {
    label = "Select Drawing";
    : text {
      label = "Select one Drawing";
    }

    spacer_1;

    : list_box {
      key = "selections";
      width = 17;
      height = 6;
    }

    spacer_1;
    ok_cancel_err;
}


selectRowdlg : dialog {
    label = "Select Row";
    : text {
      label = "Select one record/row";
    }

    spacer_1;

    : list_box {
      key = "selections";
      width = 80;
      height = 20;
    }

    spacer_1;
    ok_cancel_err;
}


//================================================================
//
//  CAOTEST DIALOG BOX
//
//================================================================
initdbconnect_button: button {
    label	= "&Init DbConnect...";
    key		= "ID_CMDINITDBCONNECT_CLICK";
}
connectdatasource_button: button {
    label	= "&Connect DataSource";
    key		= "ID_CMDCONNECTDATASOURCE_CLICK";
}
//errors_button: button {
//    label	= "&Error Status";
//    key		= "ID_CMDERRORS_CLICK";
//}
getlinks_button: button {
    label	= "&Get Links";
    key		= "ID_CMDGETLINKS_CLICK";
}
getlinktemplates_button: button {
    label	= "Get Link Tem&plates";
    key		= "ID_CMDGETLINKTEMPS_CLICK";
}
getlinktemplatedata_button: button {
    label	= "Get Link Template &Data";
    key		= "ID_CMDGETLINKTEMPDATA_CLICK";
}
makelink_button: button {
    label	= "Make &Link";
    key		= "ID_CMDMAKELINK_CLICK";
}
reloadlabels_button: button {
    label	= "&Reload Labels";
    key		= "ID_CMDRELOADLABELS_CLICK";
}
showlinkdata_button: button {
    label	= "Show L&ink Data";
    key		= "ID_CMDSHOWLINKDATA_CLICK";
}
showlinkedrow_button: button {
    label	= "&Show Linked Rows";
    key		= "ID_CMDSHOWLINKEDROW_CLICK";
}
showtable_button: button {
    label	= "Show &Table";
    key		= "ID_CMDSHOWTABLE_CLICK";
}
updatelink_button: button {
    label	= "&Update Link";
    key		= "ID_CMDUPDATELINK_CLICK";
}


exit_button : cancel_button {
    key 	= "ID_BUTT_EXIT";
    label	= "&CLOSE";
    is_default   =  true;
}



linktype_toggle : boxed_column {
   label = "Get Link Data" ;

   : toggle {
   	label = "E&ntity Link";
   	key   = "ID_TOGGL_ENTITYLINK";
   	value = "1";
   }
   : toggle {
   	label = "&Freestanding Label";
   	key   = "ID_TOGGL_FSLABEL";
   	value = "1";
   }
   : toggle {
   	label = "&Attached Label";
   	key   = "ID_TOGGL_ATTACHEDLABEL";
   	value = "1";
   }

   getlinks_button ;
   showlinkdata_button ;
   showlinkedrow_button ;
}




records_list : boxed_column {
   label = "Last output";
    : list_box {
      key = "ID_RECORDS";
      width = 17;
      height = 6;
    }

    status_string ;
}



init_group : boxed_column {
   label = "Status";
   alignment = left;
   fixed_height = true ;
   fixed_width = false ;

// Connections occur automatically from all other buttons, so 
// leave these commented out.
//
//   initdbconnect_button ;
//   connectdatasource_button ;

  // errors_button ;

   showtable_button ;
}

links_group : boxed_column {
   label = "Create/Update Links";
   alignment = left;
   fixed_height = true ;
   fixed_width = false ;

   makelink_button ;
   updatelink_button ;
   reloadlabels_button ;
}


linktemplate_group : boxed_column {
   label = "Link Templates";
   alignment = right ;
   fixed_height = true ;
   fixed_width = false ;

   getlinktemplates_button ;
   getlinktemplatedata_button ;
}


status_string : text {
	key = "ID_STATUS" ;
        alignment      = left ;
        width = 75;
}

caotest_dialog : dialog {
   label = "CAO Test";

   records_list ;

   spacer_1;

   :row {
      :column {
           links_group ;
           spacer_1;
           init_group ;
      }
      :column {
      }
      :column {
      }
      :column {
           linktype_toggle ;
           spacer_1;
           linktemplate_group ;
      }
   }


   exit_button;
}

