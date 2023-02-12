$ErrorActionPreference = "Stop"

function Load-Environment 
{
    [CmdletBinding()]
    param (
        [Parameter(Position=0, Mandatory=1)]
        [string]$EnvFile
    )

    Write-Host "Loading environment file $EnvFile.."
    Get-Content $EnvFile | ForEach-Object {
        $name, $value = $_.split('=')

        $name = $name.Trim()
        if ([string]::IsNullOrWhiteSpace($name) || $name.StartsWith('#')) {
            return;
        }

        if (Test-Path "env:$name") {
            $existing_value = [System.Environment]::GetEnvironmentVariable($name)
            Write-Host "    - $name=$value (skipped, current value=$existing_value)"
            return;
        }

        Write-Host "    - $name=$value"
        [Environment]::SetEnvironmentVariable($name, $value, 'Process')
    }
}

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


$nodehost_root_path = Resolve-Path (Join-Path $PSScriptRoot '..')

Load-Environment (Join-Path $nodehost_root_path ".env")

$nodehost_version=$Env:NODEHOST_VERSION
$nodehost_version_tag="v$Env:NODEHOST_VERSION"
$nodehost_node_version=$Env:NODEHOST_NODE_VERSION
$nodehost_node_version_tag="v$nodehost_node_version"
$nodehost_architecture=$Env:NODEHOST_ARCHITECTURE
$nodehost_configuration=$Env:NODEHOST_CONFIGURATION