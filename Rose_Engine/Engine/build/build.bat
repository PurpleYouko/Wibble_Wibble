@rem =====================================================
@rem 빌드 배치 파일
@rem 
@rem 2004-01-27 zho
@rem =====================================================


@if "%VS71COMNTOOLS%"=="" goto error

@rem 비주얼 스튜디오 환경변수 세팅
@call %VS71COMNTOOLS%\vsvars32.bat

@if "%VSINSTALLDIR%"=="" goto Usage
@SET DEVENV=%VSINSTALLDIR%\devenv.exe

@rem =====================================================
@rem znzin release
@echo building znzin release...
%DEVENV% msvc71\engine.sln /build Release /project znzin
@rem =====================================================

@goto end

:error
@echo 오류: 환경변수가 제대로 설정되지 않았습니다.
@goto end

:end
