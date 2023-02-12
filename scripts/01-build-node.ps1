. (Join-Path $PSScriptRoot "_common.ps1")

$libs_path = Join-Path $nodehost_root_path 'libs'
$node_path = Join-Path $libs_path 'node'

if (Test-Path $node_path) {
    Remove-Item -Force -Recurse $node_path
}
$null = New-Item $node_path -ItemType directory

""
"Cloning node repository.."
""
Exec { 
    git clone --depth 1 --branch $nodehost_node_version_tag https://github.com/nodejs/node.git $node_path
}

""
"Building node.."
""
if ($IsWindows) {
    $build_arguments = @()
    if ($nodehost_configuration -ieq 'debug') {
        $build_arguments += 'debug';
    }
    if ($nodehost_architecture -ieq 'x64') {
        $build_arguments += $nodehost_architecture;
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
