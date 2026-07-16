# Finance Ledger

Using [Ferret](https://github.com/Anythingsoup01/Ferret), my application framework, I have build a basic application template.

Anyone is free to use this software however they choose, just so long as this file & the license is located __with__ the framework.

# Linux

### 1. Clone the Repository

Pull the repository and all necessary submodules using the following command:

```bash

git clone --recursive https://github.com/Anythingsoup01/FinanceLedger.git
cd FinanceLedger

```

### 2. Ensure ImGui is Proper

This project uses a certain branch in ImGui. Run the following commands to ensure it's properly setup

```bash

cd Ferret/vendor/imgui
git switch docking
cd ../../../

```

### 3. Build with CMake

This project uses CMake for cross-platform builds. Run the following commands to compile:

```bash

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j N

```
### 4. Run (from root folder)

```bash
./build/FerretApp/FerretApp
```

---

# WSL2 Setup Instructions

Follow these steps to install and configure Windows Subsystem for Linux (WSL2) for development.


## 1. Enable WSL and Virtualization Features
Open **PowerShell** as an Administrator and run the following command to install the WSL kernel and the default Ubuntu distribution:

```bash
# This example is using a debian distro, such as Ubuntu
wsl --install

```

*Note: If you already have WSL 1 installed, you can ensure you are using version 2 by running:*
`wsl --set-default-version 2`

---

## 2. Restart Your Computer

Windows requires a reboot to initialize the Virtual Machine Platform and Windows Hypervisor Platform features.

---

## 3. Set Up Your Linux User

After restarting, a terminal window will open automatically to finish the installation.

1. Wait for the decompression to finish.
2. Enter a **Username** (this does not have to match your Windows username).
3. Enter a **Password**.

---

## 4. Install Build Essentials

Once inside the Linux terminal, update your package manager and install the tools needed for C++ development and CMake:

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install build-essential cmake gdb git -y

```

Then follow along with the above Linux process!

Thank you for your support.
