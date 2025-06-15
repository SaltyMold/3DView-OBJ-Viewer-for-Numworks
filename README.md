<h1 align="center">3DView OBJ Viewer for the Numworks Calculator</h1>
<p align="center">
    <img alt="Version" src="https://img.shields.io/badge/Version-0.0.1-blue?style=for-the-badge&color=blue">
    <img alt="Stars" src="https://img.shields.io/github/stars/SaltyMold/3DView-OBJ-Viewer-for-Numworks?style=for-the-badge&color=magenta">
    <img alt="Forks" src="https://img.shields.io/github/forks/SaltyMold/3DView-OBJ-Viewer-for-Numworks?color=cyan&style=for-the-badge&color=purple">
    <img alt="License" src="https://img.shields.io/github/license/SaltyMold/3DView-OBJ-Viewer-for-Numworks?style=for-the-badge&color=blue">
    <br>
    <a href="https://github.com/SaltyMold"><img title="Developer" src="https://img.shields.io/badge/Developer-SaltyMold-red?style=flat-square"></a>
    <img alt="Maintained" src="https://img.shields.io/badge/Maintained-Yes-blue?style=flat-square">
    <img alt="Written In" src="https://img.shields.io/badge/Written%20In-C-yellow?style=flat-square">
</p>

_This app is an **3D OBJ viewer** that runs on the **[NumWorks Calculator](https://www.numworks.com)**._

## Install the app

To install this app, you'll need to:
1. Go to this page I have created : https://saltymold.github.io/3DView-OBJ-Viewer-for-Numworks/
2. Upload your **`.obj`** file (or load a sample **`.obj`** file) and connect your calculator
3. And you'r done, just click install and enjoy

## How to use the app

<table>
  <tr>
    <td>
      <table>
        <tr>
          <th>Key</th>
          <th>Action</th>
        </tr>
        <tr>
          <td>Home</td>
          <td>Quit</td>
        </tr>
        <tr>
          <td>Arrow Down 🟠</td>
          <td>Go Down</td>
        </tr>
        <tr>
          <td>Arrow Up 🟠</td>
          <td>Go Up</td>
        </tr>
        <tr>
        <td>Arrow Right 🟠</td>
          <td>Go Right</td>
        </tr>
        <td>Arrow Left 🟠</td>
          <td>Go Left</td>
        </tr>
         <tr>
          <td>OK 🟢</td>
          <td>Zoom In</td>
        </tr>
        <tr>
          <td>Back 🟢</td>
          <td>Zoom Out</td>
        </tr>
        <tr>
          <td>ToolBox 🔵</td>
          <td>Camera Up</td>
        </tr>
        <tr>
          <td>SQRT 🔵</td>
          <td>Camera Down</td>
        </tr>
        <tr>
          <td>Imaginary 🔵</td>
          <td>Camera left</td>
        </tr>
        <tr>
          <td>Power 🔵</td>
          <td>Camera Right</td>
        </tr>
        <tr>
          <td>Shift 🔴</td>
          <td>Debud Mode</td>
        </tr>
      </table>
    </td>
    <td style="padding-left: 20px;">
      <img src="https://github.com/user-attachments/assets/4b8f30cd-dd9e-433c-9a84-17c8da3230cd" width="250" alt="Controls">
    </td>
  </tr>
</table>

> [!CAUTION]
> Installing third-party applications may cause your calculator to crash. This is not dangerous, but you will lose your Python scripts. Please make sure to save them before launching the app..

## How I created this application

This application works by converting a 3D `.obj` file into a binary format (`.bin`), either using the online converter or the Python script provided in the repository. When launched on the NumWorks calculator, the binary model is loaded into RAM. The app then performs a real-time perspective projection of the 3D model.

## Build the app

### Requirements:

- **arm-none-eabi-gcc**
- **nodejs 18.20.7 (install n for 18.20.7 version)**
- **nwlink (with npm)**
- **make 4.3**
- **git (optional)**

To build this sample app, you will need to install the **[embedded ARM toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)** and **[Node.js](https://nodejs.org/en/) 18**. The C SDK for Epsilon apps is shipped as an **npm module called [nwlink](https://www.npmjs.com/package/nwlink) v0.0.16**.

### Debian

```sh
sudo apt install -y build-essential git gcc-arm-none-eabi binutils-arm-none-eabi nodejs npm && npm install -g n && sudo n 18 && npm install -g nwlink@0.0.16
git clone https://github.com/SaltyMold/3DView-OBJ-Viewer-for-Numworks.git
cd 3DView-OBJ-Viewer-for-Numworks
make clean && make build
```

### Windows

You can install MSYS2 with all dependencies preinstalled from my [C-App-Guide-for-Numworks](https://github.com/SaltyMold/C-App-Guide-for-Numworks) or install all manually with the instructions bellow.

Install msys2 from [the MSYS2 Github](https://github.com/msys2/msys2-installer/releases/download/2025-02-21/msys2-x86_64-20250221.exe) and open the msys2.exe file.
Download the .zip from [the Node Github](https://github.com/actions/node-versions/releases/download/18.20.7-13438827950/node-18.20.7-win32-x64.7z), and extract it.

```sh
#MSYS2

pacman -Syu

#Replace with the reel node path
echo 'export PATH="/c/Users/UserName/AppData/Local/Programs/node-18.20.7-win32-x64:$PATH"' >> ~/.bashrc
source ~/.bashrc

npm install -g nwlink@0.0.16
nwlink --version
```

```ps
#PowerShell

#You can chose a diferent path
$env:ChocolateyInstall = "$env:LOCALAPPDATA\Programs\choco"
[System.Environment]::SetEnvironmentVariable("ChocolateyInstall", $env:ChocolateyInstall, "User")

Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

$env:Path += ";$env:ChocolateyInstall\bin"
[System.Environment]::SetEnvironmentVariable("Path", $env:Path, "User")
choco --version

choco install make --version=4.3 -y --force
make --version
```

```sh
#MSYS2

#Replace with the reel make path
echo 'export PATH="/c/Users/UserName/AppData/Local/Programs/choco/make/bin:$PATH"' >> ~/.bashrc 
source ~/.bashrc

pacman -S --noconfirm mingw-w64-x86_64-arm-none-eabi-gcc
arm-none-eabi-gcc --version

pacman -S --noconfirm git
git --version
git clone https://github.com/SaltyMold/3DView-OBJ-Viewer-for-Numworks.git

cd 3DView-OBJ-Viewer-for-Numworks
make clean && make build
```

You should now have a **`output/app.nwa` file** that you can distribute! Anyone can now install it on their calculator from the **[NumWorks online uploader](https://my.numworks.com/apps)**.

## Build your own app

To build your own app, start by cloning the repository:

```sh
git clone https://github.com/SaltyMold/3DView-OBJ-Viewer-for-Numworks.git
```
Inside the project, you'll find **`eadk.h`**, which provides **essential functions** for interacting with the **[calculator](https://en.wikipedia.org/wiki/NumWorks)**. Modify **`main.c`** to implement your **own code**.
Additionally, make sure to include an **`icon.png`** with dimensions **55×56 pixels** to serve as your **app’s icon**. Once your modifications are done, link the app with **[nwlink](https://www.npmjs.com/package/nwlink)** and enjoy your app!

## Special thanks 

I followed the guide from **[epsilon-sample-app-c](https://github.com/numworks/epsilon-sample-app-c)** to build this app.
