param([string]$item)

$global:sharedPath = "z:\samples\shared"

invoke-item $global:sharedPath\$item

