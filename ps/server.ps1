$version = "ver. 0.4"

$global:pipe = $null
$global:pipeName = "\\.\control"
$global:pipeName2 = "\\.\pipe\control"
$global:pipeName3 = "control"
$global:pipeStream = $null

$global:ns = $null
$global:nsReader = $null
$global:nsWriter = $null

$enc = new-object system.text.asciiEncoding

$lf = new-object system.byte
$lf = 0x0a


function set-pipe-server($name)
{
	"setting pipe server"
	$pipedir = [system.io.pipes.pipedirection]::inout
	$maxinstances = 5
	$pipemode = [system.io.pipes.pipetransmissionmode]::message
	$global:pipeStream = new-object system.io.pipes.namedpipeserverstream($name, $pipedir, $maxinstances, $pipemode)
	$global:pipeStream.waitforconnection()
	"got connection"
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

function read-socket($count)
{
	$i = $global:ns.read($buffer, 0, $buffer.length)
	$ans = $enc.getstring($buffer, 0, $i-1)
	return $ans
}


function write-socket($string)
{
#	$bytes = $enc.getbytes($string)
#	$global:ns.write($bytes, 0, $bytes.length)
	$global:nsWriter.writeLine($string)
	$global:nsWriter.Flush()
}

function get-pipe-stream()
{
	$global:pipe = new-object system.io.pipes.namedpipeclientstream($pipeName)
	$global:pipe.connect()
	$global:pipeStream = new-object system.io.bufferedstream($pipe)
}

function get-socket-streams()
{
	$global:nsReader = new-object system.io.streamreader($global:ns)
	$global:nsWriter = new-object system.io.streamwriter($global:ns)
}

function dispatch-command([string]$command, [system.net.sockets.tcpclient]$client)
{
	$recv = new-object system.byte[] 128
	$cmdarray = $command.split(" ")

	if($cmdarray[0] -eq "ps")
	{
		$a = get-process | out-string
		write-socket $a
	}

	if($cmdarray[0] -eq "inject")
	{
		& ".\p021-instrumentation-server.exe" $cmdarray[1]
		set-pipe-server $global:pipeName3
		write-socket "Thread injected"
	}


	if($cmdarray[0] -eq "testmode")
	{
		if($cmdarray[1] -eq "enter")
		{
			write-socket "Entering testmode"
			while($true)
			{
				$file = read-socket
				
				if($file -eq "testmode exit")
				{
					break
				}
#				mv -force $file "C:\test.dwg"
#				Invoke-item "C:\test.dwg"
				Invoke-item $file

				"1"
				$ans = read-pipe 2
				"2"
#				$ans = $enc.getstring($recv, 0, $i)
				write-socket $ans
			}
			write-socket "Exiting test mode"
		}
	}

	if($cmdarray[0] -eq "checkpipe")
	{
		if(test-path $global:pipeName2)
		{
			write-socket "Pipe exists"
		}
		else
		{
			write-socket "Pipe doesn't exist"
		}
	}

	if($cmdarray[0] -eq "getpipe")
	{
		get-pipe-stream
		write-socket "Assumed control"
	}

	if($cmdarray[0] -eq "pipe")
	{
		$rest = ""
		for($i = 1; $i -le $cmdarray.count; $i++)
		{
			$rest += $cmdarray[$i]
			$rest += " "
		}

		$rest = $rest.substring(0, $rest.length)

		write-pipe $rest

		do
		{	
			$ans = read-pipe 100
			write-socket $ans
		}
		while($ans -ne "OK")
	}

	if($cmdarray[0] -eq "testPipe")
	{
		write-pipe "testPipe"

		$ans = read-pipe 2
		write-socket $ans

		$ans = read-pipe 2
		write-socket $ans
	}
}

function listen-port($port)
{
	$enc = new-object system.text.asciiEncoding
	$buffer = new-object system.byte[] 1024
	$endpoint = new-object system.net.ipendpoint([system.net.ipaddress]::any, $port)
	$listener = new-object system.net.sockets.tcplistener($endpoint)
	$listener.start()
	$client = new-object system.net.sockets.tcpclient
	$client = $listener.AcceptTcpClient()
	$global:ns = $client.getstream()

	get-socket-streams
	"connected"

	write-socket $version

	while($true)
	{
		$command = read-socket
		if($command -eq "quit")
		{
			break
		}
		dispatch-command $command $client
	}
	$listener.stop()
}

[reflection.Assembly]::loadwithpartialname("system.core")

"got it, strating server"
listen-port(12345)

"finishing"
$bytes = $enc.getbytes("quit")
$pipeStream.write($bytes, 0, $bytes.length)
$pipeStream.flush()
