<h1 align="center">3DView OBJ Viewer for the Numworks Calculator</h1>
<p align="center">
    <img alt="Version" src="https://img.shields.io/badge/Version-0.0.3-blue?style=for-the-badge&color=blue">
    <img alt="Stars" src="https://img.shields.io/github/stars/SaltyMold/3DView-OBJ-Viewer-for-Numworks?style=for-the-badge&color=magenta">
    <img alt="Forks" src="https://img.shields.io/github/forks/SaltyMold/3DView-OBJ-Viewer-for-Numworks?color=cyan&style=for-the-badge&color=purple">
    <img alt="License" src="https://img.shields.io/github/license/SaltyMold/3DView-OBJ-Viewer-for-Numworks?style=for-the-badge&color=blue">
    <br>
    <a href="https://github.com/SaltyMold"><img title="Developer" src="https://img.shields.io/badge/Developer-SaltyMold-red?style=flat-square"></a>
    <img alt="Maintained" src="https://img.shields.io/badge/Maintained-Yes-blue?style=flat-square">
    <img alt="Written In" src="https://img.shields.io/badge/Written%20In-C-yellow?style=flat-square">
</p>

_This app is an **3D OBJ viewer** that runs on the **[NumWorks Calculator](https://www.numworks.com)**._

![3DView](https://github.com/user-attachments/assets/79cc4cd6-515b-41ef-b843-43aae30e8adb)

## 📕 Install the app

To install this app, you'll need to:
1. Go to this page I have created : https://saltymold.github.io/3DView-OBJ-Viewer-for-Numworks/
2. Upload your **`.obj`** file (or load a sample **`.obj`** file) and connect your calculator
3. And you'r done, just click install and enjoy

## ⚙️ How to use the app

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
        <tr>
          <td>Zero 🟣</td>
          <td>Auto Camera Mode</td>
        </tr>
      </table>
    </td>
    <td style="padding-left: 20px;">
      <img src="https://github.com/user-attachments/assets/0e8799f2-e444-443b-827b-d1920631a4e3" width="250" alt="Controls">
    </td>
  </tr>
</table>

> [!CAUTION]
> Installing third-party applications may cause your calculator to crash. This is not dangerous, but you will lose your Python scripts. Please make sure to save them before launching the app..

## 💡 How I created this application

This application works by converting a 3D `.obj` file into a binary format (`.bin`), either using the online converter or the Python script provided in the repository. When launched on the NumWorks calculator, the binary model is loaded into RAM. The app then performs a real-time perspective projection of the 3D model.

## 🛠️ Build the app

I made tutorials here :
- [C-App-Guide-for-Numworks](https://github.com/SaltyMold/C-App-Guide-for-Numworks)
- [Numworks-App-Development-Template](https://github.com/SaltyMold/Numworks-App-Development-Template)
- 
