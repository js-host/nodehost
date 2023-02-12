. (Join-Path $PSScriptRoot "_common.ps1")

$libs_path = Join-Path $nodehost_root_path 'libs'
$node_path = Join-Path $libs_path 'node'
$patches_path = Join-Path $nodehost_root_path 'patches'
$libs_path = Join-Path $nodehost_root_path 'libs'
$node_path = Join-Path $libs_path 'node'

""
"Patching node.."
""
Exec-In-Folder $node_path {
    git checkout 'node.gyp'
}
Exec-In-Folder $node_path {
    git checkout 'vcbuild.bat'
}
Exec-In-Folder $node_path {
    git apply (Join-Path $patches_path 'node.gyp.patch')
}
Exec-In-Folder $node_path {
    git apply (Join-Path $patches_path 'vcbuild.bat.patch')
}

""
"Building nodehost.."
""
if ($IsWindows) {
    $build_arguments = @()
    if ($nodehost_configuration -ieq 'debug') {
        $build_arguments += 'debug';
    }
    if ($architecture -ieq 'x64') {
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