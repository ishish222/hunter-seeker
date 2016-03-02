$global:pipe = $null
$global:pipeName3 = "test"
$global:pipeStream = $null
$global:ns = $null
$global:nsReader = $null
$global:nsWriter = $null
$global:lastProc = $null
$global:spawnDelay = 10
$global:binnerPath = "W:\paimei\binner-opera-1.py"
$recv = new-object system.byte[] 128
$enc = new-object system.text.asciiEncoding

function set-pipe-server($name)
{
	$pipedir = [system.io.pipes.pipedirection]::inout
	$maxinstances = 5
	$pipemode = [system.io.pipes.pipetransmissionmode]::message
	$pipeoptions = [system.io.pipes.pipeoptions]::asynchronous
	$global:pipeStream = new-object system.io.pipes.namedpipeserverstream($name, $pipedir, $maxinstances, $pipemode, $pipeoptions)
	$global:pipeStream.waitforconnection()
	$global:pipeStream
}

function read-pipe($count)
{
	$i = $global:pipeStream.read($recv, 0, $count)
	$ans = $enc.getstring($recv, 0, $i)
	return $ans
}

function write-pipe($string)
{
	$bytes = $enc.getbytes($string)
	$global:pipeStream.write($bytes, 0, $bytes.length)
	$global:pipeStream.flush()
}

[reflection.Assembly]::loadwithpartialname("system.core")


"starting server"

set-pipe-server $global:pipeName3 

$ans = "a"

while($ans -ne "quit")
{
    $ans = read-pipe 100
    $ans
}

"finishing"
$bytes = $enc.getbytes("quit")
$pipeStream.write($bytes, 0, $bytes.length)
$pipeStream.flush()
