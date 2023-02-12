param (
    [string]$configuration = "release",
    [string]$destination = "artifacts"
)

$ErrorActionPreference = "Stop"

""
"Parameters .."
"  destination: string, default: publish"
""
"Examples .."
"  .\03-publish.ps1 -destination:artifacts"
""
"Destination:     $destination"
""

$root_path = Resolve-Path (Join-Path $PSScriptRoot '..')
$libs_path = Join-Path $root_path 'libs'
$node_path = Join-Path $libs_path 'node'
$node_publish_path = Join-Path $node_path 'out' ($configuration -eq "debug" ? "Debug" : "Release")
$nodehost_src_path = Join-Path $root_path 'src' 'nodehost'
$publish_path = Join-Path $root_path 'artifacts'
$publish_headers_path = Join-Path $publish_path 'include' 'nodehost'

if (Test-Path $publish_path) {
    Remove-Item -Force -Recurse $publish_path
}
$null = New-Item $publish_path -ItemType directory

Copy-Item (Join-Path $node_publish_path "libnodehost.dll") $publish_path
Copy-Item (Join-Path $node_publish_path "libnodehost.pdb") $publish_path
Copy-Item (Join-Path $node_publish_path "nodehostsample.exe") $publish_path
Copy-Item (Join-Path $node_publish_path "nodehostsample.pdb") $publish_path

New-Item $publish_headers_path -ItemType directory
Copy-Item (Join-Path $nodehost_src_path "config.h") $publish_headers_path
Copy-Item (Join-Path $nodehost_src_path "libnodehost.h") $publish_headers_path
