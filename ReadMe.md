# 개요

해당 프로젝트는 NSIS 의 스크립트 문법이 제한적인 것이 아쉬워 AutoIt 문법을 이용해 NSIS 와 같은 기능을 구현해보는 프로젝트입니다. 

기반 소스 : AutoIt 3, NSIS 입니다. 

프로젝트의 구성

AutoNSStub 
    스크립트를 실행하는 컴파일된 실행파일 본체입니다. 

MakeAutoNS
    스크립트를 컴파일하는 도구입니다. 

## 컴파일된 EXE 의 구조

-------
| EXE | Signature | HDR | Script | Blob Data 
-------

확장자 : ANS
스크립트 파일 인코딩 : BOM 이 없는 경우 ANSI 로 간주, BOM 이 있으면 해당 인코딩 사용
스크립트 내부 문자열 : UTF16LE 로 구성


전처리기 도입

#define 
#ifdef
#ifndef 
#endif

스크립트 처리 흐름도

파일 읽기
전처리기 처리 후 하나의 파일로 생성
FileInstall 처리
최종 생성된 스크립트를 이용하여 EXE 로 생성

