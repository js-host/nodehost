param (
    [string]$configuration = "release",
    [string]$architecture = "x64"
)

$ErrorActionPreference = "Stop"

""
"Parameters .."
"  -configuration: [debug|release], default: release"
"  -architecture: [x86|x64], default: x64"
""
"Examples .."
"  .\02-build.ps1 -configuration:debug -architecture:x86"
""
"Configuration:  $configuration"
"Architecture:   $architecture"
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

if ($IsWindows) {
    $build_arguments = @()
    if ($configuration -ieq 'debug') {
        $build_arguments += 'debug';
    }
    if ($architecture -ieq 'x64') {
        $build_arguments += $architecture;
    }

    Exec-In-Folder $node_path {
        & ./vcbuild.bat @build_arguments
    }
} else {
    Exec-In-Folder $node_path {
        & ./configure
    }
    Exec-In-Folder $node_path {
        make -j4
    }
}