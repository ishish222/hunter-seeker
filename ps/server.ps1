$global:pipe = $null
$global:pipeName = "\\.\control"
$global:pipeName2 = "\\.\pipe\control"
$global:pipeStream = $null
$enc = new-object system.text.asciiEncoding

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
		$bytes = $enc.getbytes($a)
		$ns.write($bytes, 0, $bytes.length)
	}

	if($cmdarray[0] -eq "inject")
	{
		& ".\p021-instrumentation-server.exe" $cmdarray[1]
		$bytes = $enc.getbytes("Thread injected")
		$ns.write($bytes, 0, $bytes.length)
	}


	if($cmdarray[0] -eq "testmode")
	{
		if($cmdarray[1] -eq "enter")
		{
			$bytes = $enc.getbytes("Entering test mode")
			$ns.write($bytes, 0, $bytes.length)
			while($true)
			{
				$i = $ns.read($buffer, 0, $buffer.length)
				$command = $enc.getstring($buffer, 0, $i-1)
				if($command -eq "testmode exit")
				{
					break
				}
				$bytes = $enc.getbytes($command)
				$global:pipeStream.write($bytes, 0, $bytes.length)
				$global:pipeStream.flush()
			}
			$bytes = $enc.getbytes("Exiting test mode")
			$ns.write($bytes, 0, $bytes.length)
		}
	}

	if($cmdarray[0] -eq "checkpipe")
	{
		if(test-path $global:pipeName2)
		{
			$bytes = $enc.getbytes("Pipe exists")
		}
		else
		{
			$bytes = $enc.getbytes("Pipe does not exist")
		}
		$ns.write($bytes, 0, $bytes.length)
	}

	if($cmdarray[0] -eq "getpipe")
	{
		get-pipe-stream
		$bytes = $enc.getbytes("Assumed control")
		$ns.write($bytes, 0, $bytes.length)
	}

	if($cmdarray[0] -eq "pipe")
	{
		$rest = ""
		for($i = 1; $i -le $cmdarray.count; $i++)
		{
			$rest += $cmdarray[$i]
		}
		$rest = $rest.substring(0, $rest.length)
		$bytes = $enc.getbytes($rest)
		$global:pipeStream.write($bytes, 0, $bytes.length)
#		$global:pipeStream.flush()

		$i = $global:pipeStream.read($recv, 0, 2)
		$ans = $enc.getstring($recv, 0, $i)
		$ans

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
