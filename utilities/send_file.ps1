$filePath = "H:\My Code\Breadboard Z80\Z80\monitor.bin"
$comPort = "COM2"
$startAddress = 0x8200

$filebuffer = [System.IO.File]::ReadAllBytes($filePath)
$fileSize = $filebuffer.length

$headerBuffer = [System.Byte[]]::new(6)
$headerBuffer[0] = [byte][char]'L'
$headerBuffer[1] = 0x0D
$headerBuffer[2] = ($startAddress -band 0x00FF)
$headerBuffer[3] = ($startAddress -band 0xFF00) -shr 8
$headerBuffer[4] = ($fileSize -band 0x00FF)
$headerBuffer[5] = ($fileSize -band 0xFF00) -shr 8

$outputBuffer = $headerBuffer + $fileBuffer

$port= new-Object System.IO.Ports.SerialPort $comPort,9600,None,8,one
$port.open()
if ($port.IsOpen) {
    Write-Host "Writing file to com port $($comPort)"
    $port.Write($outputBuffer, 0, $outputBuffer.length)
    $port.Close()
}
else {
    Write-Host "Unable to open com port $($comPort)"
}