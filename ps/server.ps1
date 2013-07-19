$global:pipe = $null
$global:pipeName = "\\.\control"
$global:pipeStream = $null

#function 

function dispatch-command([string]$command, [system.net.sockets.tcpclient]$client)
{
	$ns = $client.getstream()
	$enc = new-object system.text.asciiEncoding
	$cmdarray = $command.split(" ")

	if($cmdarray[0] -eq "test1")
	{
		$bytes = $enc.getbytes("odpowiedz na test1\n")
		$ns.write($bytes, 0, $bytes.length)
	}
	if($cmdarray[0] -eq "pipe")
	{
		$rest = $cmdarray[1]+" "+$cmdarray[2]+" "+$cmdarray[3]+" "+$cmdarray[4]
		$global:pipeStream.write($rest)
		$global:pipeStream.flush()
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

function get-pipe-stream()
{
	$global:pipe = new-object system.io.pipes.namedpipeclientstream($pipeName)
	$global:pipe.connect()
	$global:pipeStream = new-object system.io.streamwriter($pipe)
}

"getting pipe"
get-pipe-stream

"got it, strating server"
listen-port(12345)

"finishing"
$pipeStream.write("quit")
$pipeStream.flush()
