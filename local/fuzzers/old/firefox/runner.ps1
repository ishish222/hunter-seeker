param([string]$item)

$global:sharedPath = "z:\"

cp index.html $global:sharedPath\index.html
cp $global:sharedPath\$item $global:sharedPath\now.ogv
invoke-item $global:sharedPath\index.html

