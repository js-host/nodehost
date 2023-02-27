. (Join-Path $PSScriptRoot "_common.ps1")

$libs_path = Join-Path $nodehost_root_path 'libs'
$node_path = Join-Path $libs_path 'node'
$node_publish_path = Join-Path $node_path 'out' ($nodehost_architecture -eq "debug" ? "Debug" : "Release")
$nodehost_src_path = Join-Path $nodehost_root_path 'src' 'nodehost'
$publish_path = Join-Path $nodehost_root_path 'artifacts'
$publish_headers_path = Join-Path $publish_path 'include' 'nodehost'

if (Test-Path $publish_path) {
    Remove-Item -Force -Recurse $publish_path
}
$null = New-Item $publish_path -ItemType directory

if ($IsWindows) {
    Copy-Item (Join-Path $node_publish_path "libnode.dll") $publish_path
    Copy-Item (Join-Path $node_publish_path "libnode.pdb") $publish_path
    Copy-Item (Join-Path $node_publish_path "libnodehost.dll") $publish_path
    Copy-Item (Join-Path $node_publish_path "libnodehost.pdb") $publish_path
    Copy-Item (Join-Path $node_publish_path "nodehostsample.exe") $publish_path
    Copy-Item (Join-Path $node_publish_path "nodehostsample.pdb") $publish_path
}
if ($IsLinux) {
    Copy-Item (Join-Path $node_publish_path "libnode.so") $publish_path
    Copy-Item (Join-Path $node_publish_path "libnodehost.so") $publish_path
    Copy-Item (Join-Path $node_publish_path "nodehostsample") $publish_path
}
if ($IsMacOS) {
    Copy-Item (Join-Path $node_publish_path "libnode.dylib") $publish_path
    Copy-Item (Join-Path $node_publish_path "libnodehost.dylib") $publish_path
    Copy-Item (Join-Path $node_publish_path "nodehostsample") $publish_path
}

$null = New-Item $publish_headers_path -ItemType directory
Copy-Item (Join-Path $nodehost_src_path "config.h") $publish_headers_path
Copy-Item (Join-Path $nodehost_src_path "libnodehost.h") $publish_headers_path

if ($IsWindows) {
    $zip_name = "libnodehost-$nodehost_version_tag-node_$nodehost_node_version_tag-windows-$nodehost_architecture.zip"
}
if ($IsLinux) {
    $zip_name = "libnodehost-$nodehost_version_tag-node_$nodehost_node_version_tag-linux-$nodehost_architecture.zip"
}
if ($IsMacOS) {
    $zip_name = "libnodehost-$nodehost_version_tag-node_$nodehost_node_version_tag-macos-$nodehost_architecture.zip"
}
if (-Not $zip_name -ieq "") {
    Compress-Archive -Path (Join-Path $publish_path "*") -DestinationPath (Join-Path $publish_path $zip_name)

    # let github actions know the name of generated output file
    if (Test-Path "env:GITHUB_OUTPUT") {
        "Writing output to $env:GITHUB_OUTPUT.."

        "zipname=$zip_name" >> "$env:GITHUB_OUTPUT"
    } else {
        "Running outside of github actions"
    }
}