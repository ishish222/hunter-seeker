param([string]$item)

#$global:serverPath = "z:\server"
$global:sharedPath = "z:\samples\shared"

#cp $global:serverPath\index2.html $global:sharedPath\index.html
#cp $global:sharedPath\$item $global:sharedPath\now.ogv
invoke-item $global:sharedPath\index.html

