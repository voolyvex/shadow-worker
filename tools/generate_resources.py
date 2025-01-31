import os
import subprocess
import sys

def run_script(script_name):
    print(f"Running {script_name}...")
    try:
        result = subprocess.run([sys.executable, script_name], 
                              check=True, 
                              capture_output=True, 
                              text=True)
        print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error running {script_name}:")
        print(e.stderr)
        return False

def main():
    # Get the directory of this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Change to the script directory
    os.chdir(script_dir)
    
    # List of scripts to run
    scripts = [
        "generate_sprites.py",
        "generate_font.py",
        "generate_sounds.py"
    ]
    
    # Run each script
    success = True
    for script in scripts:
        if not run_script(script):
            success = False
            print(f"Failed to generate resources in {script}")
    
    if success:
        print("\nAll resources generated successfully!")
    else:
        print("\nSome resources failed to generate. Check the errors above.")
        sys.exit(1)

if __name__ == "__main__":
    main() 