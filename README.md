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

## 🚀 Getting Started (Build Guide)

이 프로젝트를 빌드하고 실행하기 위해 다음 단계를 순서대로 진행해 주세요.

### 1. Prerequisites (사전 준비)
**vcpkg**가 설치되어 있고, Visual Studio와 통합되어 있어야 합니다.

1.  **Visual Studio 설치 시:** "C++를 사용한 데스크톱 개발(Desktop development with C++)" 워크로드가 설치되어 있어야 합니다.
2.  **vcpkg 설정:** (아직 설정하지 않았다면) PowerShell 또는 CMD에서 아래 명령어를 실행하세요.

```powershell
# vcpkg 폴더로 이동 후
.\vcpkg integrate install