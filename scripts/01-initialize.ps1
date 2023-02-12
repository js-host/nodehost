param (
    [string]$node_tag = "v19.6.0"
)

$ErrorActionPreference = "Stop"

""
"Parameters .."
"  -node_tag: string, default: v19.6.0"
""
"Examples .."
"  .\01-initialize.ps1 -node_tag:v19.6.0"
""
"Node tag:       $node_tag"
""

function Exec
{
    # source: http://joshua.poehls.me/2012/powershell-script-module-boilerplate/
    [CmdletBinding()]
    param (
        [Parameter(Position=0, Mandatory=1)]
        [scriptblock]$Command
    )
    $LastExitCode = 0
    & $Command
    if ($LastExitCode -ne 0) {
        throw "Execution failed:`n`tError code: $LastExitCode`n`tCommand: $Command"
    }
    return
}

function Exec-In-Folder
{
    [CmdletBinding()]
    param (
        [Parameter(Position=0, Mandatory=1)]
        [string]$Folder,
        [Parameter(Position=1, Mandatory=1)]
        [scriptblock]$Command
    )

    Push-Location $Folder
    try {
        Exec $Command
    }
    finally {
        Pop-Location
    }
    return
}

$root_path = Resolve-Path (Join-Path $PSScriptRoot '..')
$patches_path = Join-Path $root_path 'patches'
$libs_path = Join-Path $root_path 'libs'
$node_path = Join-Path $libs_path 'node'

if (Test-Path $node_path) {
    Remove-Item -Force -Recurse $node_path
}
$null = New-Item $node_path -ItemType directory

""
"Cloning nodejs repository.."
""
Exec { 
    git clone --depth 1 --branch $node_tag https://github.com/nodejs/node.git $node_path
}

""
"Patching nodejs repository.."
""
Exec-In-Folder $node_path {
    git apply (Join-Path $patches_path 'node.gyp.patch')
}
Exec-In-Folder $node_path {
    git apply (Join-Path $patches_path 'vcbuild.bat.patch')
}