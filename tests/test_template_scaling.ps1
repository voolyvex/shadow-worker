# Test Template Scaling Script
param(
    [Parameter(Mandatory=$true)]
    [string]$TestTemplate
)

# Import the module
$modulePath = Join-Path $PSScriptRoot "..\scripts\asset_management.psm1"
Import-Module $modulePath -Force

function Write-TestResult {
    param(
        [string]$TestName,
        [bool]$Success,
        [string]$Message = ""
    )
    
    $status = if ($Success) { "[PASS]" } else { "[FAIL]" }
    Write-Host "$status $TestName"
    if ($Message) {
        Write-Host "       $Message"
    }
}

Write-Host "`nRunning template scaling tests...`n"

# Test basic template validation
try {
    $template = Get-Content $TestTemplate | ConvertFrom-Json
    $isValid = $template.type -and $template.size -and $template.connections
    Write-TestResult "Basic Template Validation" $isValid "Template structure is valid"
}
catch {
    Write-TestResult "Basic Template Validation" $false "Failed to parse template: $_"
}

# Test scaling to different sizes
foreach ($size in @("medium", "large", "boss")) {
    try {
        $scaled = Get-ScaledTemplate -TemplatePath $TestTemplate -TargetSize $size
        Write-TestResult "Scale to $size" $true "Successfully scaled to $($scaled.size.x)x$($scaled.size.y)"
    }
    catch {
        Write-TestResult "Scale to $size" $false $_
    }
}

# Test transition creation
$transitionSizes = @{
    Source = "medium"
    Target = "large"
}

# Create a scaled version for transition testing
try {
    $sourcePath = Join-Path $PSScriptRoot "temp_source.json"
    $targetPath = Join-Path $PSScriptRoot "temp_target.json"
    
    $sourceTemplate = Get-ScaledTemplate -TemplatePath $TestTemplate -TargetSize $transitionSizes.Source
    $targetTemplate = Get-ScaledTemplate -TemplatePath $TestTemplate -TargetSize $transitionSizes.Target
    
    $sourceTemplate | ConvertTo-Json -Depth 10 | Set-Content $sourcePath
    $targetTemplate | ConvertTo-Json -Depth 10 | Set-Content $targetPath
    
    # Test smooth transition
    $smoothTransition = New-TemplateTransition -SourcePath $sourcePath -TargetPath $targetPath -TransitionType "Smooth"
    Write-TestResult "Create smooth transition" ($smoothTransition -ne $null) "Successfully created smooth transition"
    
    # Test sharp transition
    $sharpTransition = New-TemplateTransition -SourcePath $sourcePath -TargetPath $targetPath -TransitionType "Sharp"
    Write-TestResult "Create sharp transition" ($sharpTransition -ne $null) "Successfully created sharp transition"
    
    # Cleanup temporary files
    Remove-Item $sourcePath -ErrorAction SilentlyContinue
    Remove-Item $targetPath -ErrorAction SilentlyContinue
}
catch {
    Write-TestResult "Create transitions" $false $_
}

# Test compatibility check
try {
    $compatibility = Test-TemplateCompatibility -TemplatePath $TestTemplate -TargetSize "large"
    Write-TestResult "Compatibility Check" $compatibility "Template is compatible with target size"
}
catch {
    Write-TestResult "Compatibility Check" $false $_
}

Write-Host "`nTemplate scaling tests completed.`n" 