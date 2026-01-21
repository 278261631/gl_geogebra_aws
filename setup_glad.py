import os
import urllib.request
import zipfile
import shutil

def download_glad():
    """
    Download and setup GLAD from a pre-generated source
    """
    print("Setting up GLAD...")
    
    glad_dir = "external/glad"
    
    # Create directories
    os.makedirs(f"{glad_dir}/include/glad", exist_ok=True)
    os.makedirs(f"{glad_dir}/include/KHR", exist_ok=True)
    os.makedirs(f"{glad_dir}/src", exist_ok=True)
    
    print("GLAD directories created")
    print("\nPlease visit https://glad.dav1d.de/ to generate GLAD files")
    print("Configuration:")
    print("  - Language: C/C++")
    print("  - Specification: OpenGL")
    print("  - API gl: Version 3.3+")
    print("  - Profile: Core")
    print("  - Generate a loader: YES")
    print("\nAfter downloading, extract:")
    print(f"  - include/glad/* -> {glad_dir}/include/glad/")
    print(f"  - include/KHR/* -> {glad_dir}/include/KHR/")
    print(f"  - src/glad.c -> {glad_dir}/src/")

if __name__ == "__main__":
    download_glad()

