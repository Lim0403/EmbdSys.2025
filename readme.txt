환영합니다 아갓시들
이것은 우리의 프로젝트를 위한 디렉터리 구성의 설명입니다.
참고해서 수정 및 보완 부탁드려요 0.<


1. embedded_ws_orig
지민이가 만들어준 드라이버
=> 큰일났을 시 박제해둘 원본입니다 이왕이면 만지지 말것

2. embedded_ws
수정 및 개발용 디렉터리
=> 드라이버 테스트나 그런 거 하고 싶을 때 쓰도록 합니다. 저는 자이로 함수 되나 테스트해서 원본이랑 좀 달라요

3. output
합치기 전 각자 만든 실행파일을 올려주세요 통합하기 전에 모아놓는 용입니다.
<목록들>
sound/bmp : 우리가 만들고있는 프로젝트 파일. 수정 시 컴파일 안되는 것 올리지 말기!! 아마 김지은 노트북 아니면 컴파일 안 될거임 alsa때문에... 컴파일하고 싶으면 지피티로 뭐시기뭐시기 잘 다운해야함
ㄴ 유틸리티 다운받으면 됨 sudo apt install alsa-utils
alsa_example.c : alsa 기본 예제. alsa 함수가 들어있어서 우리 프로젝트 내부에 출력까지 구현했고 녹음 함수도 구현 예정. 김지은이 할게요
recording-younhu : 연후가 쓰고있는 녹음 및 출력 코드인데 system 함수 들어가서 전체적인 기능만 참고할듯
hid-multitouch.ko
터치 스크린 드라이버 파일. 타겟에 insmod 해놓기

<alsa 관련 파일들>
lib : library 파일.
alsa : alsa 관련 include 파일.
============================================
SYSROOT = /home/jieun/develop/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi/arm-linux-gnueabi/libc

INCLUDES = -I. -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/alsa
LIBPATH = -L$(SYSROOT)/usr/lib -L$(SYSROOT)/lib
===========================================
=> 김지은 makefile에는 이렇게 되어있는데, 각 경로에 잘 넣어보면 아마 make를 자기 컴퓨터에서 할 수 있을것임.
