@{
    RootModule = 'AssetManagement.psm1'
    ModuleVersion = '1.0.0'
    GUID = 'f8b0e1a0-5b0a-4b0a-8b0a-5b0a4b0a8b0a'
    Author = 'Shadow Worker'
    CompanyName = 'Shadow Worker'
    Copyright = '(c) 2024 Shadow Worker. All rights reserved.'
    Description = 'Asset management module for Shadow Worker game'
    PowerShellVersion = '5.1'
    FunctionsToExport = @('Initialize-AssetSystem', 'Import-GameAssets', 'Remove-UnusedAssets')
    CmdletsToExport = @()
    VariablesToExport = '*'
    AliasesToExport = @()
    PrivateData = @{
        PSData = @{
            Tags = @('Game', 'Assets', 'Management')
            LicenseUri = ''
            ProjectUri = ''
            ReleaseNotes = 'Initial release'
        }
    }
} 