$start_time = Get-Date

$configurations = @(
	"Debug",
	"Release"
)

$platforms = @(
	"x86",
	"x64"
)

$sln_path = "Code.sln"

$build_count = 0
$build_success_count = 0
$fail_targets = @()
foreach ($platform in $platforms) {
	foreach ($configuration in $configurations) {
		Echo "Building $($sln_file) ($configuration|$platform)..."
		$build_command_line = "msbuild $($sln_path) -p:Configuration=$configuration -p:Platform=$platform"
		Invoke-Expression $build_command_line
		
		if($LASTEXITCODE -ne 0) {
			$fail_targets += "$($sln_file) ($configuration|$platform)"
			Echo "failed to build '$sln_path' in configuration '$configuration' and for platform '$platform'"
		}
		else
		{
			$build_success_count += 1
		}

		$build_count += 1
	}
}

if ($fail_targets.Count -gt 0) {
	Echo "Failed targets:"
	Echo $fail_targets
}

$build_time = New-Timespan -Start $start_time -End (Get-Date)
$result = "Code Build done after {0:c}. $build_success_count of $build_count builds succeded." -f $build_time
echo $result

if ($fail_targets.Count -gt 0) {
	exit 1
}

$package_files = Get-ChildItem Binary\*.exe,Content\*.*,Shader\*.hpp,Shader\*.hlsl,Shader\*.hlsli -Recurse

$package_path = Join-Path $PSScriptRoot "Samples.zip"
Echo "Write $package_path..."

Remove-Item $package_path -ErrorAction Ignore

Add-Type -AssemblyName System.IO.Compression

$zip_stream = [System.IO.File]::Open($package_path, [System.IO.FileMode]::Create)
$zip = New-Object -TypeName System.IO.Compression.ZipArchive ($zip_stream, [System.IO.Compression.ZipArchiveMode]::Create)

foreach ($file in $package_files) {
    $relative_path = $(Resolve-Path -Path $file -Relative)	-replace '\.\\','Samples\'
    echo "Compress: $relative_path..."
	
    $zip_entry = $zip.CreateEntry($relative_path)
    $zip_entry_writer = New-Object -TypeName System.IO.BinaryWriter $zip_entry.Open()
    $zip_entry_writer.Write([System.IO.File]::ReadAllBytes($file))
    $zip_entry_writer.Flush()
    $zip_entry_writer.Close()
}

$zip.Dispose()
$zip_stream.Dispose()