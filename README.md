# DirectX 11 Win32 Game Template (DR + vcpkg)

![DirectX 11](https://img.shields.io/badge/Graphics-DirectX_11-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6)
![License](https://img.shields.io/badge/License-MIT-green)

이 프로젝트는 **DirectX 11**을 기반으로 한 Win32 데스크톱 게임 템플릿입니다.  
Microsoft의 `DeviceResources` 아키텍처를 사용하여 렌더링 파이프라인을 체계적으로 관리하며, **vcpkg**를 통해 외부 라이브러리(DirectXTK) 의존성을 관리하도록 구성되었습니다.

> **Target Audience:** Native C++ 및 DirectX 11 학습자, 게임 클라이언트 개발 입문자

---

## ✨ Features

### Core Framework
- **DeviceResources Architecture:** `Device`, `DeviceContext`, `SwapChain` 등 D3D 필수 자원의 생성 및 생명주기 관리
- **Robust Loop:** `StepTimer`를 이용한 안정적인 게임 루프 및 가변/고정 델타 타임(Delta Time) 처리
- **Window Management:** Win32 API 기반 윈도우 생성, DPI 처리, 리사이즈(Resize) 시 리소스 자동 재생성

### Graphics & Tools (DirectX Tool Kit)
- **SpriteBatch & SpriteFont:** 2D 스프라이트 및 폰트 렌더링
- **Texture Loading:** `DDSTextureLoader` 및 `WICTextureLoader` 내장
- **Input System:** 키보드(Keyboard) 및 마우스(Mouse) 입력 상태 관리 클래스
- **Simple Audio:** (필요시) `AudioEngine`을 통한 사운드 재생 구조 포함

---

## 📦 Dependencies

이 프로젝트는 다음 도구 및 라이브러리를 사용합니다.

- **Visual Studio 2022**
- **Windows 10/11 SDK**
- **vcpkg** (C++ Package Manager)
- **DirectX Tool Kit (DirectXTK)**

---

## 🔎 Check vcpkg Installation (설치 확인)

### ✅ 1) vcpkg가 설치되어 있는지 확인
PowerShell 또는 CMD에서 아래를 실행해 보세요.

```powershell
vcpkg --version
```

- 버전 정보가 출력되면 ✅ 설치되어 있는 상태입니다.
- `'vcpkg' is not recognized...` 같은 오류가 나오면 ❌ 설치되어 있지 않거나 PATH에 등록되지 않은 상태입니다.

또는 vcpkg 폴더에서 직접 확인할 수도 있습니다.

```powershell
.\vcpkg --version
```

---

### ✅ 2) vcpkg가 설치되어 있지 않을 때 (설치 방법)
아래 순서대로 설치하면 됩니다. (처음 1회)

```powershell
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat
```

> ⚠️ `bootstrap-vcpkg.bat` 실행 후 `vcpkg.exe`가 생성됩니다.

---

### ✅ 3) vcpkg를 PATH에 등록 (선택 사항 / 추천)
어느 위치에서든 `vcpkg` 명령을 쓰고 싶다면 Windows 환경 변수 PATH에 vcpkg 경로를 추가하세요.

```
C:\git\vcpkg
```
### ✅ 4) 사용할 vcpkg의 위치 설정
vcpkg.exe 의 위치를 변경 하고 싶다면 Directory.Build.prop 파일을 열어 해당 vcpkg 경로를 수정 하세요. 
```
<Project>
  <PropertyGroup>
    <VcpkgRoot>C:\git\vcpkg</VcpkgRoot>
  </PropertyGroup>
</Project>
```


등록 후 새 터미널을 열고 다음으로 확인합니다.

```powershell
vcpkg --version
```

---

## 🚀 Getting Started (Build Guide)

이 프로젝트는 **vcpkg Manifest 모드**를 사용합니다.  
즉, 프로젝트에 포함된 `vcpkg.json` 파일을 기반으로 빌드 시 필요한 패키지(DirectXTK 등)가 **자동으로 설치/복구(restore)** 됩니다.

> ✅ `vcpkg integrate install` 같은 전역 통합 설정은 필요하지 않습니다.

---

### 1. Prerequisites (사전 준비)

1. **Visual Studio 2022 설치**
    - "C++를 사용한 데스크톱 개발(Desktop development with C++)" 워크로드가 설치되어 있어야 합니다.
    - Windows 10/11 SDK 포함

2. **Git 설치**
    - 레포지토리 Clone 용도

3. **vcpkg 설치**
    - PC에 vcpkg 툴이 설치되어 있어야 합니다.

---

### 2. Clone Repository
```powershell
git clone https://github.com/bong9tutor/Dx11DrVcpkg.git
