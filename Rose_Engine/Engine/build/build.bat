@rem =====================================================
@rem ���� ��ġ ����
@rem 
@rem 2004-01-27 zho
@rem =====================================================


@if "%VS71COMNTOOLS%"=="" goto error

@rem ���־� ��Ʃ��� ȯ�溯�� ����
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
@echo ����: ȯ�溯���� ����� �������� �ʾҽ��ϴ�.
@goto end

:end
