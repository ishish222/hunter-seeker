param([string]$item)

$global:sharedPath = "z:\samples\shared"
cp $global:sharedPath\$item c:\
invoke-item c:\$item

