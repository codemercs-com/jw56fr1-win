$search = '*VID_07C0&PID_111A*'
$status = "OK"  #OK,ERROR,DEGRADED,UNKNOWN

Get-PnpDevice -InstanceId $search -PresentOnly -Status $status -ErrorAction SilentlyContinue | Select-Object -Property InstanceId  *>$null

if( -not $? )
{
	Write-Host "No devices with $search found" -ForegroundColor Red
}
else
{
	#Search for present/online devices by InstandeID (Vendor&Product) and remove them
	foreach ($dev in Get-PnpDevice -InstanceId $search -PresentOnly) 
	{
	  pnputil /remove-device $dev.InstanceId 
	}

	#Check for devices changes
	pnputil /scan-devices
}