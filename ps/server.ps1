$global:pipe = $null
$global:pipeName = "\\.\control"
$global:pipeName2 = "\\.\pipe\control"
$global:pipeStream = $null
$enc = new-object system.text.asciiEncoding

function read-pipe($count)
{
	$i = $global:pipeStream.read($recv, 0, 2)
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
	$i = $ns.read($buffer, 0, $buffer.length)
	$ans = $enc.getstring($buffer, 0, $i-1)
	return $ans
}


function write-socket($string, $ns)
{
	$bytes = $enc.getbytes($string)
	$ns.write($bytes, 0, $bytes.length)
}

function get-pipe-stream()
{
	$global:pipe = new-object system.io.pipes.namedpipeclientstream($pipeName)
	$global:pipe.connect()
	$global:pipeStream = new-object system.io.bufferedstream($pipe)
}

function dispatch-command([string]$command, [system.net.sockets.tcpclient]$client)
{
	$recv = new-object system.byte[] 128
	$ns = $client.getstream()
	$cmdarray = $command.split(" ")

	if($cmdarray[0] -eq "ps")
	{
		$a = get-process | out-string
		write-socket $a $ns
	}

	if($cmdarray[0] -eq "inject")
	{
		& ".\p021-instrumentation-server.exe" $cmdarray[1]
		write-socket "Thread injected" $ns
	}


	if($cmdarray[0] -eq "testmode")
	{
		if($cmdarray[1] -eq "enter")
		{
			write-socket "Entering testmode" $ns
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
				write-socket $ans $ns
			}
			write-socket "Exiting test mode" $ns
		}
	}

	if($cmdarray[0] -eq "checkpipe")
	{
		if(test-path $global:pipeName2)
		{
			write-socket "Pipe exists" $ns
		}
		else
		{
			write-socket "Pipe doesn't exist" $ns
		}
	}

	if($cmdarray[0] -eq "getpipe")
	{
		get-pipe-stream
		write-socket "Assumed control" $ns
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
		$ans = read-pipe 2
	}

	if($cmdarray[0] -eq "testPipe")
	{
		write-pipe "testPipe"

		$ans = read-pipe 2
		write-socket $ans $ns

		$ans = read-pipe 2
		write-socket $ans $ns
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
	"connected"
	$ns = $client.getstream()
	while($true)
	{
		$i = $ns.read($buffer, 0, $buffer.length)
		$command = $enc.getstring($buffer, 0, $i-1)
		if($command -eq "quit")
		{
			break
		}
		dispatch-command $command $client
	}
	$listener.stop()
}


"got it, strating server"
listen-port(12345)

"finishing"
$bytes = $enc.getbytes("quit")
$pipeStream.write($bytes, 0, $bytes.length)
$pipeStream.flush()
